//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <sstream>
#include <utility>

#include <pybind11/numpy.h>
#include "runtime/dtypes.h"
#include "runtime/utils.h"
#include "utils.h"
#include "common/logging.h"

namespace py = pybind11;
using namespace py::literals;
using namespace vox;

enum PyScalarT {
    pyint = 1,
    pyfloat = 2,
};

template<typename T>
py::list to_list(Array &a, size_t index, int dim) {
    py::list pl;
    auto stride = a.strides()[dim];
    for (int i = 0; i < a.shape(dim); ++i) {
        if (dim == a.ndim() - 1) {
            pl.append((a.data<T>()[index]));
        } else {
            pl.append(to_list<T>(a, index, dim + 1));
        }
        index += stride;
    }
    return pl;
}

auto to_scalar(Array &a) {
    switch (a.dtype()) {
        case uint8:
            return py::cast(a.data<uint8_t>(0));
        case uint16:
            return py::cast(a.data<uint16_t>(0));
        case uint32:
            return py::cast(a.data<uint32_t>(0));
        case uint64:
            return py::cast(a.data<uint64_t>(0));
        case int8:
            return py::cast(a.data<int8_t>(0));
        case int16:
            return py::cast(a.data<int16_t>(0));
        case int32:
            return py::cast(a.data<int32_t>(0));
        case int64:
            return py::cast(a.data<int64_t>(0));
        case float16:
            return py::cast(static_cast<float>(a.data<float16_t>(0)));
        case float32:
            return py::cast(a.data<float>(0));
        case quat:
        case float2:
        case float3:
        case float4:
        case float2x2:
        case float3x3:
        case float4x4:
        case transform:
        case spatial_vector:
        case spatial_matrix:
            ERROR("not implemented");
    }
}

py::object tolist(Array &a) {
    if (a.ndim() == 0) {
        return to_scalar(a);
    }
    py::object pl;
    switch (a.dtype()) {
        case uint8:
            return to_list<uint8_t>(a, 0, 0);
        case uint16:
            return to_list<uint16_t>(a, 0, 0);
        case uint32:
            return to_list<uint32_t>(a, 0, 0);
        case uint64:
            return to_list<uint64_t>(a, 0, 0);
        case int8:
            return to_list<int8_t>(a, 0, 0);
        case int16:
            return to_list<int16_t>(a, 0, 0);
        case int32:
            return to_list<int32_t>(a, 0, 0);
        case int64:
            return to_list<int64_t>(a, 0, 0);
        case float16:
            return to_list<float16_t>(a, 0, 0);
        case float32:
            return to_list<float>(a, 0, 0);
        case quat:
        case float2:
        case float3:
        case float4:
        case float2x2:
        case float3x3:
        case float4x4:
        case transform:
        case spatial_vector:
        case spatial_matrix:
            ERROR("not implemented");
    }
}

template<typename T, typename U>
void fill_vector(T list, std::vector<U> &vals) {
    for (auto l : list) {
        if (py::isinstance<py::list>(l)) {
            fill_vector(l.template cast<py::list>(), vals);
        } else if (py::isinstance<py::tuple>(*list.begin())) {
            fill_vector(l.template cast<py::tuple>(), vals);
        } else {
            vals.push_back(l.template cast<U>());
        }
    }
}

template<typename T>
PyScalarT validate_shape(
    T list,
    const std::vector<int> &shape,
    int idx,
    bool &all_python_primitive_elements) {
    if (idx >= shape.size()) {
        throw std::invalid_argument("Initialization encountered extra dimension.");
    }
    auto s = shape[idx];
    if (py::len(list) != s) {
        throw std::invalid_argument(
            "Initialization encountered non-uniform length.");
    }

    if (s == 0) {
        return pyfloat;
    }

    PyScalarT type = pyint;
    for (auto l : list) {
        PyScalarT t;
        if (py::isinstance<py::list>(l)) {
            t = validate_shape(
                l.template cast<py::list>(),
                shape,
                idx + 1,
                all_python_primitive_elements);
        } else if (py::isinstance<py::tuple>(*list.begin())) {
            t = validate_shape(
                l.template cast<py::tuple>(),
                shape,
                idx + 1,
                all_python_primitive_elements);
        } else if (py::isinstance<py::int_>(l)) {
            t = pyint;
        } else if (py::isinstance<py::float_>(l)) {
            t = pyfloat;
        } else {
            std::ostringstream msg;
            msg << "Invalid type in array initialization" << l.get_type() << ".";
            throw std::invalid_argument(msg.str());
        }
        type = std::max(type, t);
    }
    return type;
}

template<typename T>
void get_shape(T list, std::vector<int> &shape) {
    shape.push_back(py::len(list));
    if (shape.back() > 0) {
        auto &l = *list.begin();
        if (py::isinstance<py::list>(l)) {
            return get_shape(l.template cast<py::list>(), shape);
        } else if (py::isinstance<py::tuple>(l)) {
            return get_shape(l.template cast<py::tuple>(), shape);
        }
    }
}

using array_init_type = std::variant<
    py::int_,
    py::float_,
    py::list,
    py::tuple,
    py::array,
    py::buffer,
    py::object>;

// Forward declaration
Array create_array(array_init_type v, std::optional<Dtype> t);

template<typename T>
Array array_from_list(
    T pl,
    const PyScalarT &inferred_type,
    std::optional<Dtype> specified_type,
    const std::vector<int> &shape) {
    // Make the array
    switch (inferred_type) {
        case pyint: {
            auto dtype = specified_type.value_or(int32);
            if (dtype == int64) {
                std::vector<int64_t> vals;
                fill_vector(pl, vals);
                return Array(vals.data(), shape, dtype);
            } else if (dtype == uint64) {
                std::vector<uint64_t> vals;
                fill_vector(pl, vals);
                return Array(vals.data(), shape, dtype);
            } else if (dtype == uint32) {
                std::vector<uint32_t> vals;
                fill_vector(pl, vals);
                return Array(vals.data(), shape, dtype);
            } else if (is_floating_point(dtype)) {
                std::vector<float> vals;
                fill_vector(pl, vals);
                return Array(vals.data(), shape, dtype);
            } else {
                std::vector<int> vals;
                fill_vector(pl, vals);
                return Array(vals.data(), shape, dtype);
            }
        }
        case pyfloat: {
            std::vector<float> vals;
            fill_vector(pl, vals);
            return Array(vals.data(), shape, specified_type.value_or(float32));
        }
        default: {
            std::ostringstream msg;
            msg << "Should not happen, inferred: " << inferred_type
                << " on subarray made of only python primitive types.";
            throw std::runtime_error(msg.str());
        }
    }
}

template<typename T>
Array array_from_list(T pl, std::optional<Dtype> dtype) {
    // Compute the shape
    std::vector<int> shape;
    get_shape(pl, shape);

    // Validate the shape and type
    bool all_python_primitive_elements = true;
    auto type = validate_shape(pl, shape, 0, all_python_primitive_elements);

    if (all_python_primitive_elements) {
        // `pl` does not contain mlx arrays
        return array_from_list(pl, type, dtype, shape);
    }

    // `pl` contains mlx arrays
    ERROR("not implemented");
}

///////////////////////////////////////////////////////////////////////////////
// Numpy -> MLX
///////////////////////////////////////////////////////////////////////////////

template<typename T>
Array np_array_to_mlx_contiguous(
    py::array_t<T, py::array::c_style | py::array::forcecast> np_array,
    const std::vector<int> &shape,
    Dtype dtype) {
    // Make a copy of the numpy buffer
    // Get buffer ptr pass to array constructor
    py::buffer_info buf = np_array.request();
    const T *data_ptr = static_cast<T *>(buf.ptr);
    return Array(data_ptr, shape, dtype);

    // Note: Leaving the following memoryless copy from np to mx commented
    // out for the time being since it is unsafe given that the incoming
    // numpy array may change the underlying data

    // // Share underlying numpy buffer
    // // Copy to increase ref count
    // auto deleter = [np_array](void*) {};
    // void* data_ptr = np_array.mutable_data();
    // // Use buffer from numpy
    // return array(data_ptr, deleter, shape, dtype);
}

Array np_array_to_mlx(const py::array &np_array, std::optional<Dtype> dtype) {
    // Compute the shape and size
    std::vector<int> shape;
    for (int i = 0; i < np_array.ndim(); i++) {
        shape.push_back(np_array.shape(i));
    }

    // Get dtype
    auto type = np_array.dtype();

    // Copy data and make array
    if (type.is(py::dtype::of<int>())) {
        return np_array_to_mlx_contiguous<int32_t>(
            np_array, shape, dtype.value_or(int32));
    } else if (type.is(py::dtype::of<uint32_t>())) {
        return np_array_to_mlx_contiguous<uint32_t>(
            np_array, shape, dtype.value_or(uint32));
    } else if (type.is(py::dtype::of<double>())) {
        return np_array_to_mlx_contiguous<double>(
            np_array, shape, dtype.value_or(float32));
    } else if (type.is(py::dtype::of<float>())) {
        return np_array_to_mlx_contiguous<float>(
            np_array, shape, dtype.value_or(float32));
    } else if (type.is(py::dtype("float16"))) {
        return np_array_to_mlx_contiguous<float>(
            np_array, shape, dtype.value_or(float16));
    } else if (type.is(py::dtype::of<uint8_t>())) {
        return np_array_to_mlx_contiguous<uint8_t>(
            np_array, shape, dtype.value_or(uint8));
    } else if (type.is(py::dtype::of<uint16_t>())) {
        return np_array_to_mlx_contiguous<uint16_t>(
            np_array, shape, dtype.value_or(uint16));
    } else if (type.is(py::dtype::of<uint64_t>())) {
        return np_array_to_mlx_contiguous<uint64_t>(
            np_array, shape, dtype.value_or(uint64));
    } else if (type.is(py::dtype::of<int8_t>())) {
        return np_array_to_mlx_contiguous<int8_t>(
            np_array, shape, dtype.value_or(int8));
    } else if (type.is(py::dtype::of<int16_t>())) {
        return np_array_to_mlx_contiguous<int16_t>(
            np_array, shape, dtype.value_or(int16));
    } else if (type.is(py::dtype::of<int64_t>())) {
        return np_array_to_mlx_contiguous<int64_t>(
            np_array, shape, dtype.value_or(int64));
    } else {
        std::ostringstream msg;
        msg << "Cannot convert numpy array of type " << type << " to mlx array.";
        throw std::invalid_argument(msg.str());
    }
}

///////////////////////////////////////////////////////////////////////////////
// Python Buffer Protocol (MLX -> Numpy)
///////////////////////////////////////////////////////////////////////////////

std::optional<std::string> buffer_format(const Array &a) {
    // https://docs.python.org/3.10/library/struct.html#format-characters
    switch (a.dtype()) {
        case uint8:
            return pybind11::format_descriptor<uint8_t>::format();
        case uint16:
            return pybind11::format_descriptor<uint16_t>::format();
        case uint32:
            return pybind11::format_descriptor<uint32_t>::format();
        case uint64:
            return pybind11::format_descriptor<uint64_t>::format();
        case int8:
            return pybind11::format_descriptor<int8_t>::format();
        case int16:
            return pybind11::format_descriptor<int16_t>::format();
        case int32:
            return pybind11::format_descriptor<int32_t>::format();
        case int64:
            return pybind11::format_descriptor<int64_t>::format();
        case float16:
            // https://github.com/pybind/pybind11/issues/4998
            return "e";
        case float32: {
            return pybind11::format_descriptor<float>::format();
        }
        default: {
            std::ostringstream os;
            os << "bad dtype: " << a.dtype();
            throw std::runtime_error(os.str());
        }
    }
}

std::vector<size_t> buffer_strides(const Array &a) {
    std::vector<size_t> py_strides;
    py_strides.reserve(a.strides().size());
    for (const size_t stride : a.strides()) {
        py_strides.push_back(stride * a.itemsize());
    }
    return py_strides;
}

///////////////////////////////////////////////////////////////////////////////
// Module
///////////////////////////////////////////////////////////////////////////////
Array create_array(array_init_type v, std::optional<Dtype> t) {
    if (auto pv = std::get_if<py::int_>(&v); pv) {
        return Array(py::cast<int>(*pv), t.value_or(int32));
    } else if (auto pv = std::get_if<py::float_>(&v); pv) {
        return Array(py::cast<float>(*pv), t.value_or(float32));
    } else if (auto pv = std::get_if<py::list>(&v); pv) {
        return array_from_list(*pv, t);
    } else if (auto pv = std::get_if<py::tuple>(&v); pv) {
        return array_from_list(*pv, t);
    } else if (auto pv = std::get_if<py::array>(&v); pv) {
        return np_array_to_mlx(*pv, t);
    } else if (auto pv = std::get_if<py::buffer>(&v); pv) {
        return np_array_to_mlx(*pv, t);
    } else {
        ERROR("not implemented");
    }
}

void init_array(py::module_ &m) {
    // Types
    py::class_<Dtype>(
        m,
        "Dtype",
        R"pbdoc(
      An object to hold the type of a :class:`array`.

      See the :ref:`list of types <data_types>` for more details
      on available data types.
      )pbdoc")
        .def_readonly(
            "size", &Dtype::size, R"pbdoc(Size of the type in bytes.)pbdoc")
        .def(
            "__repr__",
            [](const Dtype &t) {
                std::ostringstream os;
                os << "arc.";
                os << t;
                return os.str();
            })
        .def("__eq__", [](const Dtype &t1, const Dtype &t2) { return t1 == t2; })
        .def("__hash__", [](const Dtype &t) {
            return static_cast<int64_t>(t.val);
        });
    m.attr("uint8") = py::cast(uint8);
    m.attr("uint16") = py::cast(uint16);
    m.attr("uint32") = py::cast(uint32);
    m.attr("uint64") = py::cast(uint64);
    m.attr("int8") = py::cast(int8);
    m.attr("int16") = py::cast(int16);
    m.attr("int32") = py::cast(int32);
    m.attr("int64") = py::cast(int64);
    m.attr("float16") = py::cast(float16);
    m.attr("float32") = py::cast(float32);
    m.attr("quat") = py::cast(quat);
    m.attr("float2") = py::cast(float2);
    m.attr("float3") = py::cast(float3);
    m.attr("float4") = py::cast(float4);
    m.attr("float2x2") = py::cast(float2x2);
    m.attr("float3x3") = py::cast(float3x3);
    m.attr("float4x4") = py::cast(float4x4);
    m.attr("transform") = py::cast(transform);
    m.attr("spatial_vector") = py::cast(spatial_vector);
    m.attr("spatial_matrix") = py::cast(spatial_matrix);

    auto array_class = py::class_<Array>(
        m,
        "Array",
        R"pbdoc(An N-dimensional array object.)pbdoc",
        py::buffer_protocol());

    {
        py::options options;
        options.disable_function_signatures();

        array_class.def(
            py::init([](array_init_type v, std::optional<Dtype> t) {
                return create_array(std::move(v), t);
            }),
            "val"_a,
            "dtype"_a = std::nullopt,
            R"pbdoc(
            __init__(self: array, val: Union[scalar, list, tuple, numpy.ndarray, array], dtype: Optional[Dtype] = None)
          )pbdoc");
    }

    array_class
        .def_buffer([](Array &a) {
            return pybind11::buffer_info(
                a.data<void>(),
                a.itemsize(),
                buffer_format(a).value_or("B"),// we use "B" because pybind uses a
                                               // std::string which can't be null
                a.ndim(),
                a.shape(),
                buffer_strides(a));
        })
        .def_property_readonly(
            "size", &Array::size, R"pbdoc(Number of elements in the array.)pbdoc")
        .def_property_readonly(
            "ndim", &Array::ndim, R"pbdoc(The array's dimension.)pbdoc")
        .def_property_readonly(
            "itemsize",
            &Array::itemsize,
            R"pbdoc(The size of the array's datatype in bytes.)pbdoc")
        .def_property_readonly(
            "nbytes",
            &Array::nbytes,
            R"pbdoc(The number of bytes in the array.)pbdoc")
        .def_property_readonly(
            "shape",
            [](const Array &a) { return py::tuple(py::cast(a.shape())); },
            R"pbdoc(
          The shape of the array as a Python list.

          Returns:
            tuple(int): A tuple containing the sizes of each dimension.
        )pbdoc")
        .def_property_readonly(
            "dtype",
            &Array::dtype,
            R"pbdoc(
            The array's :class:`Dtype`.
          )pbdoc")
        .def(
            "item",
            &to_scalar,
            R"pbdoc(
            Access the value of a scalar array.

            Returns:
                Standard Python scalar.
          )pbdoc")
        .def(
            "tolist",
            &tolist,
            R"pbdoc(
            Convert the array to a Python :class:`list`.

            Returns:
                list: The Python list.

                If the array is a scalar then a standard Python scalar is returned.

                If the array has more than one dimension then the result is a nested
                list of lists.

                The value type of the list corresponding to the last dimension is either
                ``int`` or ``float`` depending on the ``dtype`` of the array.
          )pbdoc")
        .def(
            "__len__",
            [](const Array &a) {
                if (a.ndim() == 0) {
                    throw py::type_error("len() 0-dimensional array.");
                }
                return a.shape(0);
            });
}

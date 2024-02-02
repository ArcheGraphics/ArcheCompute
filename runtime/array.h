//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "dtypes.h"
#include "allocator.h"

namespace vox {
class Array {
public:
    /** Construct a scalar Array with zero dimensions. */
    template<typename T>
    explicit Array(T val, Dtype dtype = TypeToDtype<T>());

    template<typename T>
    explicit Array(std::vector<T> val, Dtype dtype = TypeToDtype<T>());

    template<typename T, size_t N>
    explicit Array(std::array<T, N> val, Dtype dtype = TypeToDtype<T>());

    template<typename It>
    Array(It data,
          const std::vector<int> &shape,
          Dtype dtype = TypeToDtype<typename std::iterator_traits<It>::value_type>());

    template<typename T>
    Array(std::initializer_list<T> data, Dtype dtype = TypeToDtype<T>());

    /* Special case so empty lists default to float32. */
    Array(std::initializer_list<float> data);

    template<typename T>
    Array(std::initializer_list<T> data,
          const std::vector<int> &shape,
          Dtype dtype = TypeToDtype<T>());

    /** Assignment to rvalue does not compile. */
    Array &operator=(const Array &other) && = delete;
    Array &operator=(Array &&other) && = delete;

    /** Default copy and move constructors otherwise. */
    Array &operator=(Array &&other) & = default;
    Array(const Array &other) = default;
    Array(Array &&other) = default;

    Array &operator=(const Array &other) & {
        if (this->id() != other.id()) {
            this->array_desc_ = other.array_desc_;
        }
        return *this;
    };

    /** The size of the Array's datatype in bytes. */
    [[nodiscard]] size_t itemsize() const {
        return size_of(dtype());
    };

    /** The number of elements in the Array. */
    [[nodiscard]] size_t size() const {
        return array_desc_->size;
    };

    /** The number of bytes in the Array. */
    [[nodiscard]] size_t nbytes() const {
        return size() * itemsize();
    };

    /** The number of dimensions of the Array. */
    [[nodiscard]] size_t ndim() const {
        return array_desc_->shape.size();
    };

    /** The shape of the Array as a vector of integers. */
    [[nodiscard]] const std::vector<int> &shape() const {
        return array_desc_->shape;
    };

    /**
   *  Get the size of the corresponding dimension.
   *
   *  This function supports negative indexing and provides
   *  bounds checking. */
    [[nodiscard]] int shape(int dim) const {
        return shape().at(dim < 0 ? dim + ndim() : dim);
    };

    /** The strides of the Array. */
    [[nodiscard]] const std::vector<size_t> &strides() const {
        return array_desc_->strides;
    };

    /** Get the Arrays data type. */
    [[nodiscard]] Dtype dtype() const {
        return array_desc_->dtype;
    };

    /** A unique identifier for an Array. */
    [[nodiscard]] std::uintptr_t id() const {
        return reinterpret_cast<std::uintptr_t>(array_desc_.get());
    }

    struct Data {
        Buffer buffer;
        explicit Data(size_t size) : buffer(malloc(size)){};
        // Not copyable
        Data(const Data &d) = delete;
        Data &operator=(const Data &d) = delete;
        ~Data() {
            free(buffer);
        }
    };

    Buffer &buffer() {
        return array_desc_->data->buffer;
    };
    [[nodiscard]] const Buffer &buffer() const {
        return array_desc_->data->buffer;
    };

    // Return a copy of the shared pointer
    // to the Array::Data struct
    [[nodiscard]] std::shared_ptr<Data> data_shared_ptr() const {
        return array_desc_->data;
    }
    // Return a raw pointer to the Arrays data
    template<typename T>
    T *data() {
        return static_cast<T *>(array_desc_->data_ptr);
    };
    template<typename T>
    T &data(size_t index) {
        return static_cast<T *>(array_desc_->data_ptr)[index];
    };
    template<typename T>
    [[nodiscard]] const T *data() const {
        return static_cast<T *>(array_desc_->data_ptr);
    };
    template<typename T>
    [[nodiscard]] const T &data(size_t index) const {
        return static_cast<T *>(array_desc_->data_ptr)[index];
    };

private:
    // Initialize the Arrays data
    template<typename It>
    void init(It src);

    struct ArrayDesc {
        Dtype dtype;
        std::vector<int> shape;
        std::vector<size_t> strides;
        size_t size{};

        // This is a shared pointer so that *different* Arrays
        // can share the underlying data buffer.
        std::shared_ptr<Data> data{nullptr};

        // Properly offset data pointer
        void *data_ptr{nullptr};

        explicit ArrayDesc(const std::vector<int> &shape, Dtype dtype);
    };

    // The ArrayDesc contains the details of the materialized Array including the
    // shape, strides, the data type. It also includes
    // the primitive which knows how to compute the Array's data from its inputs
    // and the list of Array's inputs for the primitive.
    std::shared_ptr<ArrayDesc> array_desc_{nullptr};
};

template<typename T>
Array::Array(T val, Dtype dtype /* = TypeToDtype<T>() */)
    : array_desc_(std::make_shared<ArrayDesc>(std::vector<int>{}, dtype)) {
    init(&val);
}

template<typename T>
Array::Array(std::vector<T> val, Dtype dtype)
    : Array(val.data(), {(int)val.size()}, dtype) {
}

template<typename T, size_t N>
Array::Array(std::array<T, N> val, Dtype dtype)
    : Array(val.data(), {(int)N}, dtype) {
}

template<typename It>
Array::Array(It data,
             const std::vector<int> &shape,
             Dtype dtype) : array_desc_(std::make_shared<ArrayDesc>(shape, dtype)) {
    init(data);
}

template<typename T>
Array::Array(std::initializer_list<T> data,
             Dtype dtype /* = TypeToDtype<T>() */)
    : array_desc_(std::make_shared<ArrayDesc>(
          std::vector<int>{static_cast<int>(data.size())},
          dtype)) {
    init(data.begin());
}

template<typename T>
Array::Array(std::initializer_list<T> data,
             const std::vector<int> &shape,
             Dtype dtype /* = TypeToDtype<T>() */)
    : array_desc_(std::make_shared<ArrayDesc>(shape, dtype)) {
    if (data.size() != size()) {
        throw std::invalid_argument(
            "Data size and provided shape mismatch in Array construction.");
    }
    init(data.begin());
}

template<typename It>
void Array::init(It src) {
    array_desc_->data = std::make_shared<Data>(size() * size_of(dtype()));
    array_desc_->data_ptr = array_desc_->data->buffer.raw_ptr();
    std::memcpy(array_desc_->data_ptr, src, nbytes());
}

}// namespace vox

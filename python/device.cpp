//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pybind11/pybind11.h>
#include "metal/metal_device.h"
#include "cuda/cuda_device.h"

namespace py = pybind11;

std::unique_ptr<vox::Device> create_device(const std::string &name) {
    if (name == "metal") {
        return std::make_unique<vox::MetalDevice>();
    } else if (name == "cuda") {
        return std::make_unique<vox::CUDADevice>();
    }
    return nullptr;
}

PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        ArcheCompute python module
        -----------------------

        .. currentmodule:: arche_compute

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    m.def("create_device", &create_device);

    py::class_<vox::Device>(m, "Device")
        .def("name", &vox::Device::name);
}

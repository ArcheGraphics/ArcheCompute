//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pybind11/pybind11.h>
#ifdef __APPLE__
#include "metal/metal_device.h"
#else
#include "cuda/cuda_device.h"
#endif

namespace py = pybind11;

enum class DeviceType {
    CPU,
    GPU
};

std::unique_ptr<vox::Device> create_device(DeviceType type) {
    switch (type) {
        case DeviceType::CPU: return nullptr;
        case DeviceType::GPU: {
#ifdef __APPLE__
            return std::make_unique<vox::MetalDevice>();
#else
            return std::make_unique<vox::CUDADevice>();
#endif
        }
    }
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

    py::enum_<DeviceType>(m, "DeviceType")
        .value("CPU", DeviceType::CPU)
        .value("GPU", DeviceType::GPU)
        .export_values();

    m.def("create_device", &create_device);

    py::class_<vox::Device>(m, "Device")
        .def("name", &vox::Device::name);
}

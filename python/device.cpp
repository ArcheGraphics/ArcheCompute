//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pybind11/pybind11.h>
#ifdef __APPLE__
#include "metal/metal_device.h"
#include "metal/metal_stream.h"
#include "metal/metal_buffer.h"
#include "metal/metal_kernel.h"
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
        .def("name", &vox::Device::name)
        .def("create_stream", &vox::Device::create_stream)
        .def("create_buffer", &vox::Device::create_buffer)
        .def("create_kernel", &vox::Device::create_kernel);

    py::class_<vox::Buffer>(m, "Buffer")
        .def(py::init<>());

    py::class_<vox::Kernel>(m, "Kernel")
        .def(py::init<>());

    py::class_<vox::Stream>(m, "Stream")
        .def("synchronize", &vox::Stream::synchronize)
        .def("dispatch_thread_groups", &vox::Stream::dispatch_thread_groups);

#ifdef __APPLE__
    py::class_<vox::MetalBuffer, vox::Buffer>(m, "MetalBuffer")
        .def(py::init<MTL::Device *, size_t>());

    py::class_<vox::MetalKernel, vox::Kernel>(m, "MetalKernel")
        .def(py::init<MTL::Device *, const std::string &, const std::string &>());

    py::class_<vox::MetalStream, vox::Stream>(m, "MetalStream")
        .def(py::init<MTL::Device *>());
#endif
}
//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "common/logging.h"

#ifdef __APPLE__
#include "metal/metal_device.h"
#include "metal/metal_stream.h"
#include "metal/metal_buffer.h"
#include "metal/metal_kernel.h"
#include "metal/extension/metal_debug_capture_ext.h"
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

    py::class_<vox::Command, std::shared_ptr<vox::Command>> give_me_a_name(m, "Command");
    {
        py::class_<vox::BufferDownloadCommand, vox::Command, std::shared_ptr<vox::BufferDownloadCommand>>(m, "BufferDownloadCommand")
            .def(py::init<std::shared_ptr<const vox::Buffer>, size_t, size_t, void *>());
        py::class_<vox::BufferUploadCommand, vox::Command, std::shared_ptr<vox::BufferUploadCommand>>(m, "BufferUploadCommand")
            .def(py::init<std::shared_ptr<const vox::Buffer>, size_t, size_t, const void *>());
        py::class_<vox::BufferCopyCommand, vox::Command, std::shared_ptr<vox::BufferCopyCommand>>(m, "BufferCopyCommand")
            .def(py::init<std::shared_ptr<const vox::Buffer>, std::shared_ptr<const vox::Buffer>, size_t, size_t, size_t>());
        py::class_<vox::ShaderDispatchCommand, vox::Command, std::shared_ptr<vox::ShaderDispatchCommand>>(m, "ShaderDispatchCommand")
            .def(py::init<std::shared_ptr<const vox::Kernel>, std::array<uint32_t, 3>, std::array<uint32_t, 3>, const std::vector<vox::Argument> &>());
    }

    py::class_<vox::Buffer, std::shared_ptr<vox::Buffer>>(m, "Buffer", py::buffer_protocol())
        .def("copy_to", [](const vox::Buffer *self, const py::buffer &compressed_data) {
            py::buffer_info in_inf = compressed_data.request();
            ASSERT(in_inf.itemsize == self->stride(), "data stride size mismatch");
            return self->copy_to(in_inf.ptr);
        })
        .def("copy_from", [](const vox::Buffer *self, const py::buffer &compressed_data) {
            py::buffer_info in_inf = compressed_data.request();
            ASSERT(in_inf.itemsize == self->stride(), "data stride size mismatch");
            return self->copy_from(in_inf.ptr);
        })
        .def("copy_from", static_cast<std::shared_ptr<vox::BufferCopyCommand> (vox::Buffer::*)(const vox::BufferView &) const noexcept>(&vox::Buffer::copy_from), "copy from gpu buffer");

    py::class_<vox::Kernel, std::shared_ptr<vox::Kernel>>(m, "Kernel")
        .def("launch_thread_groups", &vox::Kernel::launch_thread_groups);

    py::class_<vox::Stream, std::shared_ptr<vox::Stream>>(m, "Stream")
        .def("synchronize", &vox::Stream::synchronize)
        .def("dispatch", &vox::Stream::dispatch);

    // debug capture
    {
        py::class_<vox::DebugCaptureExt>(m, "DebugCaptureExt")
            .def("create_scope", static_cast<std::unique_ptr<vox::DebugCaptureScope> (vox::DebugCaptureExt::*)(std::string_view, const vox::DebugCaptureOption &) const noexcept>(&vox::MetalDebugCaptureExt::create_scope), "create with device")
            .def("create_scope", static_cast<std::unique_ptr<vox::DebugCaptureScope> (vox::DebugCaptureExt::*)(const std::shared_ptr<vox::Stream> &, std::string_view, const vox::DebugCaptureOption &) const noexcept>(&vox::MetalDebugCaptureExt::create_scope), "create with stream");

        py::class_<vox::DebugCaptureScope>(m, "DebugCaptureScope")
            .def("start_debug_capture", &vox::DebugCaptureScope::start_debug_capture)
            .def("stop_debug_capture", &vox::DebugCaptureScope::stop_debug_capture)
            .def("mark_begin", &vox::DebugCaptureScope::mark_begin)
            .def("mark_end", &vox::DebugCaptureScope::mark_end);

        py::enum_<vox::DebugCaptureOption::Output>(m, "DebugCaptureOptionOutput")
            .value("DEVELOPER_TOOLS", vox::DebugCaptureOption::Output::DEVELOPER_TOOLS)
            .value("GPU_TRACE_DOCUMENT", vox::DebugCaptureOption::Output::GPU_TRACE_DOCUMENT)
            .export_values();

        py::class_<vox::DebugCaptureOption>(m, "DebugCaptureOption")
            .def(py::init<>())
            .def_readwrite("output", &vox::DebugCaptureOption::output)
            .def_readwrite("file_name", &vox::DebugCaptureOption::file_name);
    }

#ifdef __APPLE__
    py::class_<vox::MetalDevice, vox::Device>(m, "MetalDevice")
        .def("name", &vox::MetalDevice::name)
        .def("create_stream", &vox::MetalDevice::create_stream)
        .def("create_buffer", &vox::MetalDevice::create_buffer)
        .def("create_kernel", &vox::MetalDevice::create_kernel)
        .def("debug_capture", &vox::MetalDevice::debug_capture);

    py::class_<vox::MetalBuffer, vox::Buffer, std::shared_ptr<vox::MetalBuffer>>(m, "MetalBuffer")
        .def(py::init<MTL::Device *, size_t, size_t>());

    py::class_<vox::MetalKernel, vox::Kernel, std::shared_ptr<vox::MetalKernel>>(m, "MetalKernel")
        .def(py::init<MTL::Device *, const std::string &, const std::string &>());

    py::class_<vox::MetalStream, vox::Stream, std::shared_ptr<vox::MetalStream>>(m, "MetalStream")
        .def(py::init<MTL::Device *>());

    py::class_<vox::MetalDebugCaptureExt, vox::DebugCaptureExt>(m, "MetalDebugCaptureExt")
        .def(py::init<MTL::Device *>())
        .def("create_scope", static_cast<std::unique_ptr<vox::DebugCaptureScope> (vox::MetalDebugCaptureExt::*)(std::string_view, const vox::DebugCaptureOption &) const noexcept>(&vox::MetalDebugCaptureExt::create_scope), "create with device")
        .def("create_scope", static_cast<std::unique_ptr<vox::DebugCaptureScope> (vox::MetalDebugCaptureExt::*)(const std::shared_ptr<vox::Stream> &, std::string_view, const vox::DebugCaptureOption &) const noexcept>(&vox::MetalDebugCaptureExt::create_scope), "create with stream");

    py::class_<vox::MetalDebugCaptureScope, vox::DebugCaptureScope>(m, "MetalDebugCaptureScope")
        .def("start_debug_capture", &vox::MetalDebugCaptureScope::start_debug_capture)
        .def("stop_debug_capture", &vox::MetalDebugCaptureScope::stop_debug_capture)
        .def("mark_begin", &vox::MetalDebugCaptureScope::mark_begin)
        .def("mark_end", &vox::MetalDebugCaptureScope::mark_end);
#endif
}
//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/cast.h>

#include "runtime/array.h"
#include "runtime/kernel.h"
#include "runtime/extension/debug_capture_ext.h"

namespace py = pybind11;
using namespace py::literals;

void init_array(py::module_ &);

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

    init_array(m);

    m.def("synchronize",
          &vox::synchronize,
          "wait"_a,
          "stream"_a = 0);

    // pre define
    auto kernel = py::class_<vox::Kernel>(m, "Kernel");
    py::class_<vox::Kernel::Builder>(m, "KernelBuilder")
        .def("hash_name", &vox::Kernel::Builder::hash_name)
        .def("lib_name", &vox::Kernel::Builder::lib_name)
        .def("entry", &vox::Kernel::Builder::entry)
        .def("source", &vox::Kernel::Builder::source)
        .def("build", &vox::Kernel::Builder::build);
    kernel.def_static("builder", &vox::Kernel::builder)
        .def("launch", &vox::Kernel::operator(),
             "thread_groups_per_grid"_a,
             "threads_per_thread_group"_a,
             "args"_a,
             "stream"_a = 0);

    // debug capture
    {
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
}
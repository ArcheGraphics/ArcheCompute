//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/device.h"
#include <benchmark/benchmark.h>

namespace vox {
enum class LatencyMeasureMode {
    // time spent from queue submit to returning from queue wait
    kSystemSubmit,
};

class BenchmarkAPI {
public:
    // Registers all Vulkan benchmarks for the current benchmark binary.
    //
    // The |overhead_seconds| field in |latency_measure| should subtracted from the
    // latency measured by the registered benchmarks for
    // LatencyMeasureMode::kSystemDispatch.
    virtual void register_benchmarks(Device *device, std::shared_ptr<Stream> &stream, LatencyMeasureMode mode) = 0;
};
}// namespace vox
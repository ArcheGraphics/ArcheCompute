//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <benchmark/benchmark.h>

namespace vox::benchmark {
enum class LatencyMeasureMode {
    // time spent from queue submit to returning from queue wait
    kSystemSubmit,
    // Timestamp difference measured on GPU
    kGpuTimestamp,
};

class BenchmarkAPI {
public:
    // Registers all Vulkan benchmarks for the current benchmark binary.
    //
    // The |overhead_seconds| field in |latency_measure| should subtracted from the
    // latency measured by the registered benchmarks for
    // LatencyMeasureMode::kSystemDispatch.
    virtual void register_benchmarks(LatencyMeasureMode mode) = 0;
};

class MADThroughPut : public BenchmarkAPI {
public:
    void register_benchmarks(LatencyMeasureMode mode) override;
};

class Reduce : public BenchmarkAPI {
public:
    void register_benchmarks(LatencyMeasureMode mode) override;
};
}// namespace vox::benchmark
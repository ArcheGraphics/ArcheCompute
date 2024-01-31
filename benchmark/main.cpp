//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <benchmark/benchmark.h>
#include "metal/metal_device.h"
#include "mad_throughput.h"

int main(int argc, char **argv) {
    ::benchmark::Initialize(&argc, argv);
    auto device = std::make_unique<vox::MetalDevice>();
    auto stream = device->create_stream();

    vox::LatencyMeasureMode mode = vox::LatencyMeasureMode::kSystemSubmit;
    auto app = std::make_unique<vox::benchmark::MADThroughPut>();
    app->register_benchmarks(device.get(), stream, mode);

    ::benchmark::RunSpecifiedBenchmarks();
}
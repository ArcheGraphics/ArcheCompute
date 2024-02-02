//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "benchmark_api.h"

int main(int argc, char **argv) {
    ::benchmark::Initialize(&argc, argv);

    auto app = std::make_unique<vox::benchmark::MADThroughPut>();
    app->register_benchmarks(vox::benchmark::LatencyMeasureMode::kSystemSubmit);

    ::benchmark::RunSpecifiedBenchmarks();
}
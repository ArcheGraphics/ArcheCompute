//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "benchmark_api.h"
#include "runtime/device.h"
#include "runtime/array.h"
#include "runtime/kernel.h"
#include "runtime/extension/debug_capture_ext.h"
#include <spdlog/fmt/fmt.h>
#include <gtest/gtest.h>

namespace vox::benchmark {
template<typename T>
T generate_data(size_t i);

template<>
float generate_data(size_t i) {
    return float(i % 9 - 4) * 0.5f;
}

template<>
size_t generate_data(size_t i) {
    return i % 13 - 7;
}

template<typename T>
static void reduce(::benchmark::State &state,
                   LatencyMeasureMode mode,
                   const std::string &kernel_name,
                   uint num_element) {
    auto reduce = Kernel::builder().entry(kernel_name).build();

    //===-------------------------------------------------------------------===/
    // Set source buffer data
    //===-------------------------------------------------------------------===/
    std::vector<T> init(num_element, 0);
    auto src_buffer = Array(init, TypeToDtype<T>());
    auto dst_buffer = Array(init, TypeToDtype<T>());

    for (size_t i = 0; i < num_element; i++) {
        src_buffer.data<T>(i) = generate_data<T>(i);
    }

    //===-------------------------------------------------------------------===/
    // Dispatch
    //===-------------------------------------------------------------------===/
    reduce({src_buffer, dst_buffer});
    synchronize(true);

    //===-------------------------------------------------------------------===/
    // Verify destination buffer data
    //===-------------------------------------------------------------------===/
    T total = 0;
    for (size_t i = 0; i < num_element; i++) {
        total += generate_data<T>(i);
    };
    EXPECT_NEAR(dst_buffer.data<T>(0), total, 0.01f)
        << "destination buffer element #0 has incorrect value: "
           "expected to be "
        << total << " but found " << dst_buffer.data<T>(0);

    //===-------------------------------------------------------------------===/
    // Benchmarking
    //===-------------------------------------------------------------------===/
    for ([[maybe_unused]] auto _ : state) {
        auto capture_scope = DebugCaptureExt::create_scope("arche-capture");

        auto start_time = std::chrono::high_resolution_clock::now();

        capture_scope.start_debug_capture();
        capture_scope.mark_begin();
        reduce({src_buffer, dst_buffer});
        synchronize();
        capture_scope.mark_end();
        capture_scope.stop_debug_capture();

        // prepare convert
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        switch (mode) {
            case LatencyMeasureMode::kSystemSubmit:
                state.SetIterationTime(elapsed_seconds.count());
                break;
            case LatencyMeasureMode::kGpuTimestamp:
                break;
        }
    }
    state.SetBytesProcessed(state.iterations() * num_element * sizeof(T));
    state.counters["FLOps"] =
        ::benchmark::Counter(num_element,
                             ::benchmark::Counter::kIsIterationInvariant |
                                 ::benchmark::Counter::kIsRate,
                             ::benchmark::Counter::kIs1000);
}

void Reduce::register_benchmarks(LatencyMeasureMode mode) {
    const size_t total_elements = 1 << 22;// 4M

    std::string test_name = fmt::format("{}/{}/{}", device().name(), "reduce", "xf32");
    ::benchmark::RegisterBenchmark(test_name, reduce<float>, mode, fmt::format("reduce"),
                                   total_elements)
        ->UseManualTime()
        ->Unit(::benchmark::kMicrosecond)
        ->MinTime(std::numeric_limits<float>::epsilon());// use cache make calculation fast after warmup
}
}// namespace vox::benchmark
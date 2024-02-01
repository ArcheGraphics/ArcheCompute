//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mad_throughput.h"
#include "runtime/device.h"
#include "runtime/array.h"
#include "runtime/kernel.h"
#include "runtime/extension/debug_capture_ext.h"
#include "data_type_util.h"
#include "counter.h"
#include <spdlog/fmt/fmt.h>
#include <gtest/gtest.h>

namespace vox::benchmark {
static void throughput(::benchmark::State &state,
                       LatencyMeasureMode mode,
                       const std::string &kernel_name,
                       int num_element, int loop_count, Dtype data_type) {
    auto kernel = Kernel::builder().entry(kernel_name).build();

    //===-------------------------------------------------------------------===/
    // Create buffers
    //===-------------------------------------------------------------------===/
    Array src0_buffer(data_type, {num_element});
    Array src1_buffer(data_type, {num_element});
    Array dst_buffer(data_type, {num_element});

    //===-------------------------------------------------------------------===/
    // Set source buffer data
    //===-------------------------------------------------------------------===/
    auto getSrc0 = [](size_t i) {
        float v = float((i % 9) + 1) * 0.1f;
        return v;
    };
    auto getSrc1 = [](size_t i) {
        float v = float((i % 5) + 1) * 1.f;
        return v;
    };

    if (data_type == float16) {
        std::vector<uint16_t> src_float_buffer0(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer0[i] = fp16(getSrc0(i)).get_value();
        }

        std::vector<uint16_t> src_float_buffer1(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer1[i] = fp16(getSrc1(i)).get_value();
        }

        src0_buffer.copy_from(src_float_buffer0.data());
        src1_buffer.copy_from(src_float_buffer1.data());
    } else if (data_type == float32) {
        std::vector<float> src_float_buffer0(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer0[i] = getSrc0(i);
        }

        std::vector<float> src_float_buffer1(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer1[i] = getSrc1(i);
        }

        src0_buffer.copy_from(src_float_buffer0.data());
        src1_buffer.copy_from(src_float_buffer1.data());
    }

    //===-------------------------------------------------------------------===/
    // Dispatch
    //===-------------------------------------------------------------------===/
    kernel(
        {(uint32_t)num_element / 4 / 32, 1, 1},
        {32, 1, 1},
        {src0_buffer, src1_buffer, dst_buffer});

    //===-------------------------------------------------------------------===/
    // Verify destination buffer data
    //===-------------------------------------------------------------------===/

    if (data_type == float16) {
        std::vector<uint16_t> dst_float_buffer(num_element);
        dst_buffer.copy_to(dst_float_buffer.data());

        for (size_t i = 0; i < num_element; i++) {
            float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
            EXPECT_NEAR(fp16(dst_float_buffer[i]).to_float(), limit, 0.5f)
                << "destination buffer element #" << i
                << " has incorrect value: expected to be " << limit
                << " but found " << fp16(dst_float_buffer[i]).to_float();
        }
    } else if (data_type == float32) {
        std::vector<float> dst_float_buffer(num_element);
        dst_buffer.copy_to(dst_float_buffer.data());

        for (size_t i = 0; i < num_element; i++) {
            float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
            EXPECT_NEAR(dst_float_buffer[i], limit, 0.01f)
                << "destination buffer element #" << i
                << " has incorrect value: expected to be " << limit
                << " but found " << dst_float_buffer[i];
        }
    }

    //===-------------------------------------------------------------------===/
    // Benchmarking
    //===-------------------------------------------------------------------===/
    {
        std::unique_ptr<Counter> gpu_counter;
        bool use_timestamp = mode == LatencyMeasureMode::kGpuTimestamp;
        if (use_timestamp) {
            gpu_counter = std::make_unique<Counter>(2);
        }

        for ([[maybe_unused]] auto _ : state) {
            auto capture_scope = DebugCaptureExt::create_scope("arche-capture");

            auto start_time = std::chrono::high_resolution_clock::now();

            capture_scope.start_debug_capture();
            capture_scope.mark_begin();
            if (use_timestamp) {
                gpu_counter->sample_counters_in_buffer(0);
            }
            kernel({(uint32_t)num_element / 4 / 32, 1, 1},
                   {32, 1, 1},
                   {src0_buffer, src1_buffer, dst_buffer});
            if (use_timestamp) {
                gpu_counter->sample_counters_in_buffer(1);
                gpu_counter->update_start_times();
            }
            synchronize();
            capture_scope.mark_end();
            capture_scope.stop_debug_capture();

            // prepare convert
            if (use_timestamp) {
                gpu_counter->update_final_times();
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
            switch (mode) {
                case LatencyMeasureMode::kSystemSubmit:
                    state.SetIterationTime(elapsed_seconds.count());
                    break;
                case LatencyMeasureMode::kGpuTimestamp:
                    state.SetIterationTime(gpu_counter->calculate_elapsed_seconds_between(0, 1));
                    break;
            }
        }
        double numOperation = double(num_element) * 2. /*fma*/ *
                              10. /*10 elements per loop iteration*/ *
                              double(loop_count);
        state.counters["FLOps"] =
            ::benchmark::Counter(numOperation,
                                 ::benchmark::Counter::kIsIterationInvariant |
                                     ::benchmark::Counter::kIsRate,
                                 ::benchmark::Counter::kIs1000);
    }
}

void MADThroughPut::register_benchmarks(LatencyMeasureMode mode) {
    const size_t num_element = 1024 * 1024;
    const int min_loop_count = 100000;
    const int max_loop_count = min_loop_count * 2;

    for (int loop_count = min_loop_count; loop_count <= max_loop_count;
         loop_count += min_loop_count) {
        std::string test_name = fmt::format("{}/{}/{}/{}", device().name(), "mad_throughput", num_element, loop_count);

        ::benchmark::RegisterBenchmark(test_name, throughput, mode, fmt::format("mad_throughput_{}", loop_count),
                                       num_element, loop_count, float32)
            ->UseManualTime()
            ->Unit(::benchmark::kMicrosecond)
            ->MinTime(std::numeric_limits<float>::epsilon());// use cache make calculation fast after warmup
    }
}

}// namespace vox::benchmark
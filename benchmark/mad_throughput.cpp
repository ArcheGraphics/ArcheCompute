//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mad_throughput.h"
#include "data_type_util.h"
#include "rhi/stream.h"
#include "rhi/buffer.h"
#include "rhi/counter.h"
#include "metal_device.h"
#include "metal/extension/metal_debug_capture_ext.h"
#include <spdlog/fmt/fmt.h>
#include <gtest/gtest.h>

namespace vox::benchmark {
static void throughput(::benchmark::State &state,
                       LatencyMeasureMode mode,
                       Device *device,
                       const std::shared_ptr<Stream> &stream,
                       const std::shared_ptr<Kernel> &kernel,
                       size_t num_element, int loop_count, DataType data_type) {
    //===-------------------------------------------------------------------===/
    // Create buffers
    //===-------------------------------------------------------------------===/
    auto src0_buffer = device->create_buffer(get_size(data_type), num_element);
    auto src1_buffer = device->create_buffer(get_size(data_type), num_element);
    auto dst_buffer = device->create_buffer(get_size(data_type), num_element);

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

    if (data_type == DataType::fp16) {
        std::vector<uint16_t> src_float_buffer0(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer0[i] = fp16(getSrc0(i)).get_value();
        }

        std::vector<uint16_t> src_float_buffer1(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer1[i] = fp16(getSrc1(i)).get_value();
        }

        stream->dispatch({src0_buffer->copy_from(src_float_buffer0.data()),
                          src1_buffer->copy_from(src_float_buffer1.data())});
        stream->synchronize();
    } else if (data_type == DataType::fp32) {
        std::vector<float> src_float_buffer0(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer0[i] = getSrc0(i);
        }

        std::vector<float> src_float_buffer1(num_element);
        for (size_t i = 0; i < num_element; i++) {
            src_float_buffer1[i] = getSrc1(i);
        }

        stream->dispatch({src0_buffer->copy_from(src_float_buffer0.data()),
                          src1_buffer->copy_from(src_float_buffer1.data())});
        stream->synchronize();
    }

    //===-------------------------------------------------------------------===/
    // Dispatch
    //===-------------------------------------------------------------------===/
    stream->dispatch({kernel->launch_thread_groups(
        {(uint32_t)num_element / 4 / 32, 1, 1},
        {32, 1, 1},
        {src0_buffer, src1_buffer, dst_buffer})});
    stream->synchronize();

    //===-------------------------------------------------------------------===/
    // Verify destination buffer data
    //===-------------------------------------------------------------------===/

    if (data_type == DataType::fp16) {
        std::vector<uint16_t> dst_float_buffer(num_element);
        stream->dispatch({dst_buffer->copy_to(dst_float_buffer.data())});
        stream->synchronize();

        for (size_t i = 0; i < num_element; i++) {
            float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
            EXPECT_NEAR(fp16(dst_float_buffer[i]).to_float(), limit, 0.5f)
                << "destination buffer element #" << i
                << " has incorrect value: expected to be " << limit
                << " but found " << fp16(dst_float_buffer[i]).to_float();
        }
    } else if (data_type == DataType::fp32) {
        std::vector<float> dst_float_buffer(num_element);
        stream->dispatch({dst_buffer->copy_to(dst_float_buffer.data())});
        stream->synchronize();

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
        for ([[maybe_unused]] auto _ : state) {
            auto capture = static_cast<vox::MetalDevice *>(device)->debug_capture();
            auto capture_scope = capture->create_scope("arche-capture");

            auto start_time = std::chrono::high_resolution_clock::now();

            capture_scope->start_debug_capture();
            capture_scope->mark_begin();
            stream->dispatch({kernel->launch_thread_groups(
                {(uint32_t)num_element / 4 / 32, 1, 1},
                {32, 1, 1},
                {src0_buffer, src1_buffer, dst_buffer})});
            capture_scope->mark_end();
            capture_scope->stop_debug_capture();

            stream->synchronize();

            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
            switch (mode) {
                case LatencyMeasureMode::kSystemSubmit:
                    state.SetIterationTime(elapsed_seconds.count());
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

void MADThroughPut::register_benchmarks(Device *device, std::shared_ptr<Stream> &stream, LatencyMeasureMode mode) {
    const size_t num_element = 1024 * 1024;
    const int min_loop_count = 100000;
    const int max_loop_count = min_loop_count * 2;

    for (int loop_count = min_loop_count; loop_count <= max_loop_count;
         loop_count += min_loop_count) {
        auto kernel = device->create_kernel_from_library(device->builtin_lib_name(),
                                                         fmt::format("mad_throughput_{}", loop_count));

        std::string test_name = fmt::format("{}/{}/{}/{}", device->name(), "mad_throughput", num_element, loop_count);

        ::benchmark::RegisterBenchmark(test_name, throughput, mode, device, stream, kernel,
                                       num_element, loop_count, DataType::fp32)
            ->UseManualTime()
            ->Unit(::benchmark::kMicrosecond)
            ->MinTime(std::numeric_limits<float>::epsilon());// use cache make calculation fast after warmup
    }
}

}// namespace vox::benchmark
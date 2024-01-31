//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>
#include "metal/metal_device.h"
#include "metal/metal_buffer.h"
#include "metal/metal_stream.h"
#include "metal/extension/metal_debug_capture_ext.h"
#include <fmt/format.h>

TEST(Metal, Metallib) {
    auto device = std::make_unique<vox::MetalDevice>();
    auto stream = device->create_stream();
    auto capture = device->debug_capture();
    auto capture_scope = capture->create_scope("arche-capture");

    const size_t num_element = 1024;
    auto src0_buffer = device->create_buffer(sizeof(float), num_element);
    auto src1_buffer = device->create_buffer(sizeof(float), num_element);
    auto dst_buffer = device->create_buffer(sizeof(float), num_element);
    auto getSrc0 = [](size_t i) {
        float v = float((i % 9) + 1) * 0.1f;
        return v;
    };
    auto getSrc1 = [](size_t i) {
        float v = float((i % 5) + 1) * 1.f;
        return v;
    };
    {
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

    auto kernel = device->create_kernel_from_library("metal_kernel",
                                                     fmt::format("mad_throughput_{}", 100000));

    capture_scope->start_debug_capture();
    capture_scope->mark_begin();
    {
        stream->dispatch({kernel->launch_thread_groups(
            {(uint32_t)num_element / 4 / 32, 1, 1},
            {32, 1, 1},
            {src0_buffer, src1_buffer, dst_buffer})});
        stream->synchronize();
    }
    capture_scope->mark_end();
    capture_scope->stop_debug_capture();

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

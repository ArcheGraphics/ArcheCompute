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
#include "common/helpers.h"

TEST(Metal, Base) {
    auto device = std::make_unique<vox::MetalDevice>();
    auto stream = device->create_stream();
    auto capture = device->debug_capture();
    auto capture_scope = capture->create_scope("arche-capture");

    const char *kernelSrc = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct alignas(8) Arguments {
            device float* buffer;
        };


        kernel void kernel_main(constant Arguments &args,
                                uint2 index [[thread_position_in_grid]],
                                uint2 gridSize [[threads_per_grid]])
        {
            args.buffer[0] = 10.0;
        })";

    std::vector<float> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto bytes_data = vox::to_bytes(data);
    auto buffer = device->create_buffer(sizeof(float), 10);
    auto kernel = device->create_kernel_with_source(kernelSrc, "kernel_main");

    capture_scope->start_debug_capture();
    capture_scope->mark_begin();
    {
        stream->dispatch({buffer->copy_from(bytes_data.data()),
                          kernel->launch_thread_groups({1, 1, 1}, {1, 1, 1}, {buffer}),
                          buffer->copy_to(bytes_data.data())});
        stream->synchronize();
    }
    capture_scope->mark_end();
    capture_scope->stop_debug_capture();

    vox::from_bytes(bytes_data, data);
    for (int i = 0; i < 10; ++i) {
        if (i == 0) {
            EXPECT_EQ(data[i], 10);
        } else {
            EXPECT_EQ(data[i], i + 1);
        }
    }
}

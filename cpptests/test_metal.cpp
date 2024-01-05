//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>
#include "metal/metal_device.h"
#include "metal/metal_stream.h"
#include "metal/extension/metal_debug_capture_ext.h"

TEST(Metal, Base) {
    auto device = std::make_unique<vox::MetalDevice>();
    auto stream = device->create_stream();
    auto capture = device->debug_capture();
    auto capture_scope = capture->create_scope("arche-capture");

    const char *kernelSrc = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct alignas(16) Arguments {
            device float* buffer;
        };


        kernel void kernel_main(constant Arguments &args,
                                uint2 index [[thread_position_in_grid]],
                                uint2 gridSize [[threads_per_grid]])
        {
            args.buffer[0] = 10.0;
        })";

    auto buffer = device->create_buffer(10 * sizeof(float));
    auto kernel = device->create_kernel(kernelSrc, "kernel_main");

    capture_scope->start_debug_capture();
    capture_scope->mark_begin();
    stream->dispatch_thread_groups(kernel, {1, 1, 1}, {1, 1, 1}, {buffer});
    stream->synchronize();
    capture_scope->mark_end();
    capture_scope->stop_debug_capture();
}

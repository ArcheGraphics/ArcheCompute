//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>
#include "runtime/device.h"
#include "runtime/array.h"
#include "runtime/extension/debug_capture_ext.h"
#include "runtime/kernel.h"

using namespace vox;

TEST(Metal, Base) {
    auto capture_scope = DebugCaptureExt::create_scope("arche-capture");

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

    auto array = Array({1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f}, float32);
    auto kernel = Kernel::builder()
                      .entry("kernel_main")
                      .lib_name("custom_lib")
                      .source(kernelSrc)
                      .build();
    kernel.set_thread_groups(1);
    kernel.set_threads_per_thread_group(1);

    capture_scope.start_debug_capture();
    capture_scope.mark_begin();
    {
        kernel({array});
        synchronize(true);
    }
    capture_scope.mark_end();
    capture_scope.stop_debug_capture();

    for (int i = 0; i < 10; ++i) {
        if (i == 0) {
            EXPECT_EQ(array.data<float>(i), 10);
        } else {
            EXPECT_EQ(array.data<float>(i), i + 1);
        }
    }
}

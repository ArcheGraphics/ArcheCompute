//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>
#include "runtime/device.h"
#include "runtime/array.h"
#include "runtime/extension/debug_capture_ext.h"
#include "common/helpers.h"
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

    std::vector<float> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto bytes_data = vox::to_bytes(data);
    auto array = Array({10}, float32);
    auto kernel = Kernel::builder()
                      .entry("kernel_main")
                      .lib_name("custom_lib")
                      .source(kernelSrc)
                      .build();

    capture_scope.start_debug_capture();
    capture_scope.mark_begin();
    {
        array.copy_from(bytes_data.data());
        kernel({1, 1, 1}, {1, 1, 1}, {array});
        array.copy_to(bytes_data.data());
    }
    capture_scope.mark_end();
    capture_scope.stop_debug_capture();

    vox::from_bytes(bytes_data, data);
    for (int i = 0; i < 10; ++i) {
        if (i == 0) {
            EXPECT_EQ(data[i], 10);
        } else {
            EXPECT_EQ(data[i], i + 1);
        }
    }
}

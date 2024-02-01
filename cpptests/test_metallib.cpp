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
#include <fmt/format.h>

using namespace vox;

TEST(Metal, Metallib) {
    auto capture_scope = DebugCaptureExt::create_scope("arche-capture");

    const size_t num_element = 1024;
    Array src0_array(float32, {num_element});
    Array src1_array(float32, {num_element});
    Array dst_array(float32, {num_element});
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
        src0_array.copy_from(src_float_buffer0.data());
        src1_array.copy_from(src_float_buffer1.data());
    }

    auto kernel = Kernel::builder()
                      .entry(fmt::format("mad_throughput_{}", 100000))
                      .build();

    capture_scope.start_debug_capture();
    capture_scope.mark_begin();
    {
        kernel({(uint32_t)num_element / 4 / 32, 1, 1},
               {32, 1, 1},
               {src0_array, src1_array, dst_array});
    }
    std::vector<float> dst_float_buffer(num_element);
    dst_array.copy_to(dst_float_buffer.data());

    capture_scope.mark_end();
    capture_scope.stop_debug_capture();

    for (size_t i = 0; i < num_element; i++) {
        float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
        EXPECT_NEAR(dst_float_buffer[i], limit, 0.01f)
            << "destination buffer element #" << i
            << " has incorrect value: expected to be " << limit
            << " but found " << dst_float_buffer[i];
    }
}

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
    std::vector<float> init(num_element, 0);
    Array src0_array(init, float32);
    Array src1_array(init, float32);
    Array dst_array(init, float32);
    auto getSrc0 = [](size_t i) {
        float v = float((i % 9) + 1) * 0.1f;
        return v;
    };
    auto getSrc1 = [](size_t i) {
        float v = float((i % 5) + 1) * 1.f;
        return v;
    };
    {
        for (size_t i = 0; i < num_element; i++) {
            src0_array.data<float>(i) = getSrc0(i);
        }

        for (size_t i = 0; i < num_element; i++) {
            src1_array.data<float>(i) = float((i % 5) + 1) * 1.f;
        }
    }

    auto kernel = Kernel::builder()
                      .entry(fmt::format("mad_throughput_{}", 100000))
                      .build();
    kernel.set_threads(num_element / 4);
    kernel.set_threads_per_thread_group(32);

    capture_scope.start_debug_capture();
    capture_scope.mark_begin();
    {
        kernel({src0_array, src1_array, dst_array});
        synchronize(true);
    }
    capture_scope.mark_end();
    capture_scope.stop_debug_capture();

    for (size_t i = 0; i < num_element; i++) {
        float limit = getSrc1(i) * (1.f / (1.f - getSrc0(i)));
        EXPECT_NEAR(dst_array.data<float>(i), limit, 0.01f)
            << "destination buffer element #" << i
            << " has incorrect value: expected to be " << limit
            << " but found " << dst_array.data<float>(i);
    }
}

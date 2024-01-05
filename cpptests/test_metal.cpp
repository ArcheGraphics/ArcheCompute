//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>
#include "metal/metal_device.h"
#include "metal/metal_stream.h"

TEST(Metal, Base) {
    auto device = std::make_unique<vox::MetalDevice>();
    auto stream = device->create_stream();

    auto buffer = device->create_buffer(10);
    auto kernel = device->create_kernel("", "main");
    stream->dispatch_thread_groups(kernel, {1, 1, 1}, {1, 1, 1}, {});
}
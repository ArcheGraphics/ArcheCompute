//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal.h"
#include "device.h"

namespace vox {
std::shared_ptr<void> new_scoped_memory_pool() {
    auto dtor = [](void *ptr) {
        static_cast<NS::AutoreleasePool *>(ptr)->release();
    };
    return {NS::AutoreleasePool::alloc()->init(), dtor};
}

}// namespace vox
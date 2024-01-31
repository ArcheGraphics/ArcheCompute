//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <sstream>

#include "allocator.h"
#include "scheduler.h"

namespace vox {

Buffer malloc(size_t size) {
    auto buffer = allocator().malloc(size, /* allow_swap */ true);
    if (size && !buffer.ptr()) {
        std::ostringstream msg;
        msg << "[malloc] Unable to allocate " << size << " bytes.";
        throw std::runtime_error(msg.str());
    }
    return buffer;
}

void free(Buffer buffer) {
    return allocator().free(buffer);
}

Buffer malloc_or_wait(size_t size) {
    auto buffer = allocator().malloc(size);

    while (size && !buffer.ptr() && n_active_tasks() > 0) {
        wait_for_one();
        buffer = allocator().malloc(size);
    }

    // Try swapping if needed
    if (size && !buffer.ptr()) {
        buffer = allocator().malloc(size, /* allow_swap = */ true);
    }

    if (size && !buffer.ptr()) {
        std::ostringstream msg;
        msg << "[malloc_or_wait] Unable to allocate " << size << " bytes.";
        throw std::runtime_error(msg.str());
    }

    return buffer;
}

}// namespace vox

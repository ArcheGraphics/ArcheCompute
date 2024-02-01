//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <sstream>

#include "allocator.h"
#include "device.h"
#include "metal.h"

namespace vox {
void *Buffer::raw_ptr() {
    return ptr_->contents();
}

uint64_t Buffer::address() const {
    return ptr_->gpuAddress();
}

Allocator::Allocator()
    : device_(vox::device().handle()),
      peak_allocated_size_(0),
      block_limit_(1.5 * device_->recommendedMaxWorkingSetSize()),
      gc_limit_(0.95 * device_->recommendedMaxWorkingSetSize()) {}

Buffer Allocator::malloc(size_t size, bool allow_swap /* = false */) {
    // Metal doesn't like empty buffers
    if (size == 0) {
        return Buffer{nullptr};
    }

    // Align up memory
    if (size > vm_page_size) {
        size = vm_page_size * ((size + vm_page_size - 1) / vm_page_size);
    }

    // If there is too much memory pressure, fail (likely causes a wait).
    if (!allow_swap && device_->currentAllocatedSize() + size >= block_limit_) {
        return Buffer{nullptr};
    }

    auto thread_pool = new_scoped_memory_pool();

    // Allocate new buffer if needed
    size_t res_opt = MTL::ResourceStorageModeShared;
    res_opt |= MTL::ResourceHazardTrackingModeTracked;
    auto buf = device_->newBuffer(size, res_opt);

    peak_allocated_size_ =
        std::max(peak_allocated_size_, device_->currentAllocatedSize());

    return Buffer{buf};
}

void Allocator::free(Buffer buffer) {
    buffer.ptr()->release();
}

Allocator &allocator() {
    static Allocator allocator_;
    return allocator_;
}

//----------------------------------------------------------------------------------------------------------------------
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

}// namespace vox

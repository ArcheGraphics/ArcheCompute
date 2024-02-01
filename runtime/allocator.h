//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <map>
#include <cstdlib>
#include "device.h"

namespace vox {

// Simple wrapper around buffer pointers
// WARNING: Only Buffer objects constructed from and those that wrap
//          raw pointers from mlx::allocator are supported.
class Buffer final {
private:
    MTL::Buffer *ptr_;

public:
    explicit Buffer(MTL::Buffer *ptr) : ptr_(ptr){};

    // Get the raw data pointer from the buffer
    void *raw_ptr();

    [[nodiscard]] uint64_t address() const;

    // Get the buffer pointer from the buffer
    [[nodiscard]] const MTL::Buffer *ptr() const {
        return ptr_;
    };
    MTL::Buffer *ptr() {
        return ptr_;
    };
};

class Allocator final {
public:
    Buffer malloc(size_t size, bool allow_swap = false);
    void free(Buffer buffer);

private:
    MTL::Device *device_;
    Allocator();
    friend Allocator &allocator();

    // Allocation stats
    size_t peak_allocated_size_;
    size_t block_limit_;
    size_t gc_limit_;
};

Allocator &allocator();

//----------------------------------------------------------------------------------------------------------------------
Buffer malloc(size_t size);

void free(Buffer buffer);

}// namespace vox

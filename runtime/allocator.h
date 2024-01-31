//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdlib>

namespace vox {

// Simple wrapper around buffer pointers
// WARNING: Only Buffer objects constructed from and those that wrap
//          raw pointers from mlx::allocator are supported.
class Buffer {
private:
    void *ptr_;

public:
    Buffer(void *ptr) : ptr_(ptr){};

    // Get the raw data pointer from the buffer
    void *raw_ptr();

    // Get the buffer pointer from the buffer
    const void *ptr() const {
        return ptr_;
    };
    void *ptr() {
        return ptr_;
    };
};

Buffer malloc(size_t size);

void free(Buffer buffer);

// Wait for running tasks to finish and free up memory
// if allocation fails
Buffer malloc_or_wait(size_t size);

class Allocator {
    /** Abstract base class for a memory allocator. */
public:
    virtual Buffer malloc(size_t size, bool allow_swap = false) = 0;
    virtual void free(Buffer buffer) = 0;

    Allocator() = default;
    Allocator(const Allocator &other) = delete;
    Allocator(Allocator &&other) = delete;
    Allocator &operator=(const Allocator &other) = delete;
    Allocator &operator=(Allocator &&other) = delete;
    virtual ~Allocator() = default;
};

Allocator &allocator();

}// namespace vox

//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <mutex>

namespace vox {
extern "C" void compute_metal_stream_print_function_logs(MTL::LogContainer *logs);

class Stream {
public:
    explicit Stream(uint32_t index);

    inline MTL::CommandQueue *queue() {
        return _queue;
    }

    MTL::CommandBuffer *get_command_buffer();

    MTL::ComputeCommandEncoder *get_command_encoder();

    void synchronize(bool wait = false);

    ~Stream();

private:
    uint32_t _index{};
    MTL::CommandQueue *_queue;
    MTL::CommandBuffer *_command_buffer{};
    MTL::ComputeCommandEncoder *_encoder{};
    std::mutex mtx_;
};
}// namespace vox
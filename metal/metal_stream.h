//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/stream.h"
#include "metal_stage_buffer_pool.h"
#include <Metal/Metal.hpp>
#include <queue>

namespace vox {
extern "C" void compute_metal_stream_print_function_logs(MTL::LogContainer *logs);

class MetalStream : public Stream {
public:
    using CallbackContainer = std::vector<MetalCallbackContext *>;

    explicit MetalStream(MTL::Device *device);

    ~MetalStream() override;

    void synchronize() override;

    [[nodiscard]] MetalStageBufferPool *upload_pool() noexcept;

    [[nodiscard]] MetalStageBufferPool *download_pool() noexcept;

    void dispatch(CommandList &&commandList) override;

    inline MTL::CommandQueue *queue() {
        return _queue;
    }

    virtual void submit(MTL::CommandBuffer *command_buffer, CallbackContainer &&callbacks) noexcept;

private:
    MTL::CommandQueue *_queue{nullptr};
    spin_mutex _upload_pool_creation_mutex;
    spin_mutex _download_pool_creation_mutex;
    spin_mutex _callback_mutex;
    spin_mutex _dispatch_mutex;
    std::unique_ptr<MetalStageBufferPool> _upload_pool;
    std::unique_ptr<MetalStageBufferPool> _download_pool;
    std::queue<CallbackContainer> _callback_lists;
};
}// namespace vox
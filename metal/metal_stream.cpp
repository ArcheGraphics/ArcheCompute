//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_stream.h"
#include "metal_kernel.h"
#include "metal_buffer.h"
#include "metal_command_encoder.h"
#include "common/logging.h"

namespace vox {
MetalStream::MetalStream(MTL::Device *device)
    : _queue{device->newCommandQueue()} {
    _queue->retain();
}

MetalStream::~MetalStream() {
    _queue->release();
}

MetalStageBufferPool *MetalStream::upload_pool() noexcept {
    {
        std::scoped_lock lock{_upload_pool_creation_mutex};
        if (_upload_pool == nullptr) {
            _upload_pool = std::make_unique<MetalStageBufferPool>(
                _queue->device(), 64000000, true);
        }
    }
    return _upload_pool.get();
}

MetalStageBufferPool *MetalStream::download_pool() noexcept {
    {
        std::scoped_lock lock{_download_pool_creation_mutex};
        if (_download_pool == nullptr) {
            _download_pool = std::make_unique<MetalStageBufferPool>(
                _queue->device(), 32000000, false);
        }
    }
    return _download_pool.get();
}

void MetalStream::synchronize() {
    auto command_buffer = _queue->commandBufferWithUnretainedReferences();
    command_buffer->commit();
    command_buffer->waitUntilCompleted();
}

void MetalStream::dispatch(std::vector<std::unique_ptr<Command>> commands) {
    MetalCommandEncoder encoder{this};
    for (auto &command : commands) { command->accept(encoder); }
    encoder.submit();
}

void MetalStream::submit(MTL::CommandBuffer *command_buffer,
                         MetalStream::CallbackContainer &&callbacks) noexcept {
    if (!callbacks.empty()) {
        {
            std::scoped_lock lock{_callback_mutex};
            _callback_lists.emplace(std::move(callbacks));
        }
        command_buffer->addCompletedHandler(^(MTL::CommandBuffer *) noexcept {
            auto callbakcs = [self = this] {
                std::scoped_lock lock{self->_callback_mutex};
                if (self->_callback_lists.empty()) {
                    WARNING_WITH_LOCATION(
                        "MetalStream::submit: Callback list is empty.");
                    return CallbackContainer{};
                }
                auto callbacks = std::move(self->_callback_lists.front());
                self->_callback_lists.pop();
                return callbacks;
            }();
            for (auto callback : callbakcs) { callback->recycle(); }
        });
    }
#ifndef NDEBUG
    command_buffer->addCompletedHandler(^(MTL::CommandBuffer *cb) noexcept {
        if (auto error = cb->error()) {
            WARNING("CommandBuffer execution error: {}.",
                    error->localizedDescription()->utf8String());
        }
        if (auto logs = cb->logs()) {
            compute_metal_stream_print_function_logs(logs);
        }
    });
#endif
    command_buffer->commit();
}

}// namespace vox
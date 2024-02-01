//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "stream.h"
#include "device.h"
#include "metal.h"
#include "common/logging.h"

namespace vox {
Stream::Stream(uint32_t index)
    : _index{index} {
    // Multiple threads can ask the device for queues
    // We lock this as a critical section for safety
    const std::lock_guard<std::mutex> lock(mtx_);
    _queue = device().handle()->newCommandQueue();
    if (!_queue) {
        throw std::runtime_error(
            "[metal::Device] Failed to make new command queue.");
    }
}

Stream::~Stream() {
    synchronize(true);

    auto pool = new_scoped_memory_pool();
    _queue->release();
}

MTL::CommandBuffer *Stream::get_command_buffer() {
    if (!_command_buffer) {
        _command_buffer = _queue->commandBuffer();

        if (!_command_buffer) {
            throw std::runtime_error(
                "[metal::Device] Unable to create new command buffer");
        }
    }
    return _command_buffer;
}

MTL::ComputeCommandEncoder *Stream::get_command_encoder() {
    if (!_encoder) {
        auto cb = get_command_buffer();
        _encoder = cb->computeCommandEncoder();
    }
    return _encoder;
}

void Stream::synchronize(bool wait) {
    if (_encoder) {
        _encoder->endEncoding();
        _encoder->release();
        _encoder = nullptr;

#ifndef NDEBUG
        _command_buffer->addCompletedHandler(^(MTL::CommandBuffer *cb) noexcept {
            if (auto error = cb->error()) {
                WARNING("CommandBuffer execution error: {}.",
                        error->localizedDescription()->utf8String());
            }
            if (auto logs = cb->logs()) {
                compute_metal_stream_print_function_logs(logs);
            }
        });
#endif

        _command_buffer->commit();
        if (wait) {
            _command_buffer->waitUntilCompleted();
        }
        _command_buffer->release();
        _command_buffer = nullptr;
    }
}

}// namespace vox
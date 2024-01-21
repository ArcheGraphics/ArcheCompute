//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_command_encoder.h"
#include "metal_stream.h"
#include "metal_buffer.h"
#include "metal_kernel.h"
#include "metal_counter.h"

namespace vox {
MetalCommandEncoder::MetalCommandEncoder(MetalStream *stream) noexcept
    : _stream{stream} {}

void MetalCommandEncoder::_prepare_command_buffer() noexcept {
    if (_command_buffer == nullptr) {
        auto desc = MTL::CommandBufferDescriptor::alloc()->init();
        desc->setRetainedReferences(false);
#ifndef NDEBUG
        desc->setErrorOptions(MTL::CommandBufferErrorOptionEncoderExecutionStatus);
#else
        desc->setErrorOptions(MTL::CommandBufferErrorOptionNone);
#endif
        _command_buffer = _stream->queue()->commandBuffer(desc);
        desc->release();
    }
}

void MetalCommandEncoder::visit(const BufferCopyCommand *command) noexcept {
    _prepare_command_buffer();
    auto src_buffer = std::static_pointer_cast<const MetalBuffer>(command->src_handle());
    auto dst_buffer = std::static_pointer_cast<const MetalBuffer>(command->dst_handle());
    auto src_offset = command->src_offset();
    auto dst_offset = command->dst_offset();
    auto size = command->size();
    auto encoder = _command_buffer->blitCommandEncoder();
    encoder->copyFromBuffer(src_buffer->handle(), src_offset, dst_buffer->handle(), dst_offset, size);
    encoder->endEncoding();
}

void MetalCommandEncoder::visit(const BufferDownloadCommand *command) noexcept {
    auto buffer = std::static_pointer_cast<const MetalBuffer>(command->handle());
    auto offset = command->offset();
    auto size = command->size();
    auto data = command->data();

    // copy from download buffer to user buffer
    add_callback(FunctionCallbackContext::create([buffer, offset, size, data] {
        std::memcpy(data, buffer->handle()->contents(), size);
    }));
}

void MetalCommandEncoder::visit(const BufferUploadCommand *command) noexcept {
    auto buffer = std::static_pointer_cast<const MetalBuffer>(command->handle());
    auto offset = command->offset();
    auto size = command->size();
    auto data = command->data();
    std::memcpy(buffer->handle()->contents(), data, size);
}

void MetalCommandEncoder::visit(const ShaderDispatchCommand *command) noexcept {
    _prepare_command_buffer();
    auto kernel = std::static_pointer_cast<const MetalKernel>(command->handle());
    kernel->launch(*this, command);
}

void MetalCommandEncoder::visit(const vox::CounterSampleCommand *command) noexcept {
    _prepare_command_buffer();
    auto counter = std::static_pointer_cast<const MetalCounter>(command->handle());
    auto encoder = _command_buffer->computeCommandEncoder();
    encoder->sampleCountersInBuffer(counter->get_handle(), command->index(), false);
    encoder->endEncoding();
}

void MetalCommandEncoder::visit(const CustomCommand *command) noexcept {
    _prepare_command_buffer();
}

void MetalCommandEncoder::add_callback(MetalCallbackContext *cb) noexcept {
    _callbacks.emplace_back(cb);
}

void MetalCommandEncoder::submit() noexcept {
    auto callbacks = std::exchange(_callbacks, {});
    if (!callbacks.empty()) { _prepare_command_buffer(); }
    auto command_buffer = std::exchange(_command_buffer, nullptr);
    if (command_buffer != nullptr) {
        _stream->submit(command_buffer, std::move(callbacks));
    }
}
}// namespace vox
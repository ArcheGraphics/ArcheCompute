//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_command_encoder.h"
#include "metal_stream.h"

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

void MetalCommandEncoder::visit(const BufferCopyCommand *) noexcept {
    _prepare_command_buffer();
}

void MetalCommandEncoder::visit(const BufferDownloadCommand *) noexcept {
    _prepare_command_buffer();
}

void MetalCommandEncoder::visit(const BufferUploadCommand *) noexcept {
    _prepare_command_buffer();
}

void MetalCommandEncoder::visit(const CustomCommand *) noexcept {
    _prepare_command_buffer();
}
}// namespace vox
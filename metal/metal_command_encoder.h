//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/command.h"
#include "metal_stream.h"

namespace vox {
class MetalCommandEncoder : public CommandVisitor {
public:
    explicit MetalCommandEncoder(MetalStream *stream) noexcept;

    void visit(const BufferCopyCommand *) noexcept override;
    void visit(const BufferDownloadCommand *) noexcept override;
    void visit(const BufferUploadCommand *) noexcept override;
    void visit(const CustomCommand *) noexcept override;
    void visit(const ShaderDispatchCommand *) noexcept override;
    void add_callback(MetalCallbackContext *cb) noexcept;
    void submit() noexcept;

    [[nodiscard]] inline MTL::CommandBuffer *command_buffer() noexcept {
        return _command_buffer;
    }

private:
    MetalStream *_stream;
    MTL::CommandBuffer *_command_buffer{nullptr};
    std::vector<MetalCallbackContext *> _callbacks;

    void _prepare_command_buffer() noexcept;
};
}// namespace vox
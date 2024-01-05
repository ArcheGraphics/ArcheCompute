//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "rhi/command.h"
#include <Metal/Metal.hpp>

namespace vox {
class MetalStream;

class MetalCommandEncoder : public CommandVisitor {
public:
    explicit MetalCommandEncoder(MetalStream *stream) noexcept;

    void visit(const BufferCopyCommand *) noexcept override;
    void visit(const BufferDownloadCommand *) noexcept override;
    void visit(const BufferUploadCommand *) noexcept override;
    void visit(const CustomCommand *) noexcept override;

private:
    MetalStream *_stream;
    MTL::CommandBuffer *_command_buffer{nullptr};

    void _prepare_command_buffer() noexcept;
};
}// namespace vox
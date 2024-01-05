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
    void add_callback(MetalCallbackContext *cb) noexcept;
    void submit() noexcept;

    template<typename F>
    void with_upload_buffer(size_t size, F &&f) noexcept {
        _prepare_command_buffer();
        auto upload_buffer = _stream->upload_pool()->allocate(size);
        f(upload_buffer);
        add_callback(upload_buffer);
    }

    template<typename F>
    void with_download_buffer(size_t size, F &&f) noexcept {
        _prepare_command_buffer();
        auto download_buffer = _stream->download_pool()->allocate(size);
        f(download_buffer);
        add_callback(download_buffer);
    }

private:
    MetalStream *_stream;
    MTL::CommandBuffer *_command_buffer{nullptr};
    std::vector<MetalCallbackContext *> _callbacks;

    void _prepare_command_buffer() noexcept;
};
}// namespace vox
//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "command.h"

namespace vox {
class BufferView {
public:
    BufferView(std::shared_ptr<const Buffer> handle, size_t offset_bytes, size_t size_elements) noexcept;

    [[nodiscard]] BufferView subview(size_t offset_elements, size_t size_elements) const noexcept;

    [[nodiscard]] size_t offset_bytes() const noexcept;

    [[nodiscard]] size_t size_bytes() const noexcept;

    // commands
    // copy buffer's data to pointer
    [[nodiscard]] std::shared_ptr<BufferDownloadCommand> copy_to(void *data) const noexcept;

    // copy pointer's data to buffer
    [[nodiscard]] std::shared_ptr<BufferUploadCommand> copy_from(const void *data) noexcept;

    // copy source buffer's data to buffer
    [[nodiscard]] std::shared_ptr<BufferCopyCommand> copy_from(const BufferView &source) noexcept;

private:
    const std::shared_ptr<const Buffer> _handle;
    size_t _offset_bytes;
    size_t _size_elements;
};

class Buffer : public std::enable_shared_from_this<Buffer> {
public:
    Buffer(size_t element_stride, size_t size_elements);
    virtual ~Buffer() = default;

    [[nodiscard]] size_t size_elements() const noexcept;
    [[nodiscard]] size_t stride() const noexcept;
    [[nodiscard]] size_t size_bytes() const noexcept;
    [[nodiscard]] BufferView view() const noexcept;
    [[nodiscard]] BufferView view(size_t offset, size_t count) const noexcept;

    // copy buffer's data to pointer
    [[nodiscard]] std::shared_ptr<BufferDownloadCommand> copy_to(void *data) const noexcept;

    // copy pointer's data to buffer
    [[nodiscard]] std::shared_ptr<BufferUploadCommand> copy_from(const void *data) const noexcept;

    // copy source buffer's data to buffer
    [[nodiscard]] std::shared_ptr<BufferCopyCommand> copy_from(const BufferView &source) const noexcept;

private:
    size_t _size_elements{};
    size_t _element_stride{};
};

}// namespace vox

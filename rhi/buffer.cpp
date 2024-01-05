//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "buffer.h"

namespace vox {
BufferView::BufferView(std::shared_ptr<const Buffer> handle, size_t offset_bytes, size_t size_elements) noexcept
    : _handle{std::move(handle)}, _offset_bytes{offset_bytes}, _size_elements{size_elements} {
}

[[nodiscard]] BufferView BufferView::subview(size_t offset_elements, size_t size_elements) const noexcept {
    return BufferView{_handle, _offset_bytes + offset_elements * _handle->stride(), size_elements};
}

size_t BufferView::offset_bytes() const noexcept { return _offset_bytes; }

size_t BufferView::size_bytes() const noexcept { return _size_elements * _handle->stride(); }

std::unique_ptr<BufferDownloadCommand> BufferView::copy_to(void *data) const noexcept {
    return std::make_unique<BufferDownloadCommand>(_handle, offset_bytes(), size_bytes(), data);
}

std::unique_ptr<BufferUploadCommand> BufferView::copy_from(const void *data) noexcept {
    return std::make_unique<BufferUploadCommand>(_handle, offset_bytes(), size_bytes(), data);
}

std::unique_ptr<BufferCopyCommand> BufferView::copy_from(const BufferView &source) noexcept {
    return std::make_unique<BufferCopyCommand>(
        source._handle, _handle,
        source.offset_bytes(), offset_bytes(),
        size_bytes());
}

Buffer::Buffer(size_t element_stride, size_t size_elements)
    : _size_elements{size_elements}, _element_stride{element_stride} {}

size_t Buffer::size_elements() const noexcept {
    return _size_elements;
}
size_t Buffer::stride() const noexcept {
    return _element_stride;
}
size_t Buffer::size_bytes() const noexcept {
    return _size_elements * _element_stride;
}
BufferView Buffer::view() const noexcept {
    return BufferView{shared_from_this(), 0u, _size_elements};
}
BufferView Buffer::view(size_t offset, size_t count) const noexcept {
    return view().subview(offset, count);
}

std::unique_ptr<BufferDownloadCommand> Buffer::copy_to(void *data) const noexcept {
    return this->view().copy_to(data);
}

std::unique_ptr<BufferUploadCommand> Buffer::copy_from(const void *data) const noexcept {
    return this->view().copy_from(data);
}

std::unique_ptr<BufferCopyCommand> Buffer::copy_from(const BufferView &source) const noexcept {
    return this->view().copy_from(source);
}

}// namespace vox
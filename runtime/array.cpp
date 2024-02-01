//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "array.h"

namespace vox {

namespace {

std::pair<size_t, std::vector<size_t>> cum_prod(const std::vector<int> &shape) {
    std::vector<size_t> strides(shape.size());
    size_t cum_prod = 1;
    for (int i = shape.size() - 1; i >= 0; --i) {
        strides[i] = cum_prod;
        cum_prod *= shape[i];
    }
    return {cum_prod, strides};
}

}// namespace

Array::Array(std::initializer_list<float> data)
    : array_desc_(std::make_shared<ArrayDesc>(
          std::vector<int>{static_cast<int>(data.size())},
          float32)) {
    init(data.begin());
}

/* Build an Array from a shared buffer */
Array::Array(Dtype dtype, const std::vector<int> &shape)
    : array_desc_(std::make_shared<ArrayDesc>(shape, dtype)) {
    set_data(malloc(size() * size_of(dtype)));
}

void Array::set_data(Buffer buffer) {
    array_desc_->data = std::make_shared<Data>(buffer);
    array_desc_->data_ptr = buffer.raw_ptr();
    array_desc_->flags.contiguous = true;
    array_desc_->flags.row_contiguous = true;
    auto max_dim = std::max_element(shape().begin(), shape().end());
    array_desc_->flags.col_contiguous = size() <= 1 || size() == *max_dim;
}

void Array::set_data(Buffer buffer,
                     size_t data_size,
                     std::vector<size_t> strides,
                     Flags flags) {
    array_desc_->data = std::make_shared<Data>(buffer);
    array_desc_->data_ptr = buffer.raw_ptr();
    array_desc_->strides = std::move(strides);
    array_desc_->flags = flags;
}

void Array::copy_shared_buffer(
    const Array &other,
    const std::vector<size_t> &strides,
    Flags flags,
    size_t data_size,
    size_t offset /* = 0 */) {
    array_desc_->data = other.array_desc_->data;
    array_desc_->strides = strides;
    array_desc_->flags = flags;
    auto char_offset = sizeof(char) * itemsize() * offset;
    array_desc_->data_ptr = static_cast<void *>(
        static_cast<char *>(other.array_desc_->data_ptr) + char_offset);
}

void Array::copy_shared_buffer(const Array &other) {
    copy_shared_buffer(other, other.strides(), other.flags(), other.size());
}

void Array::move_shared_buffer(const Array &other) {
    array_desc_->data = std::move(other.array_desc_->data);
    array_desc_->strides = other.strides();
    array_desc_->flags = other.flags();
    array_desc_->data_ptr = other.array_desc_->data_ptr;
}

Array::ArrayDesc::ArrayDesc(const std::vector<int> &shape, Dtype dtype)
    : shape(shape), dtype(dtype) {
    std::tie(size, strides) = cum_prod(shape);
}

void Array::copy_to(void *data, uint32_t stream) const noexcept {
    synchronize(true, stream);
    std::memcpy(data, array_desc_->data_ptr, array_desc_->size * size_of(dtype()));
}

void Array::copy_from(const void *data) noexcept {
    std::memcpy(array_desc_->data_ptr, data, array_desc_->size * size_of(dtype()));
}

}// namespace vox

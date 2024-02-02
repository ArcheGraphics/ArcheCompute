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

Array::ArrayDesc::ArrayDesc(const std::vector<int> &shape, Dtype dtype)
    : shape(shape), dtype(dtype) {
    std::tie(size, strides) = cum_prod(shape);
}

}// namespace vox

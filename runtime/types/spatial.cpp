//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spatial.h"

namespace vox {
SpatialMatrix &SpatialMatrix::operator=(const float &other) {
    for (auto &col : columns) {
        col = other;
    }
    return *this;
}

SpatialMatrix &SpatialMatrix::operator=(float &&other) {
    for (auto &col : columns) {
        col = other;
    }
    return *this;
}

Transform &Transform::operator=(const float &other) {
    p = other;
    return *this;
}

Transform &Transform::operator=(float &&other) {
    p = other;
    return *this;
}

}// namespace vox
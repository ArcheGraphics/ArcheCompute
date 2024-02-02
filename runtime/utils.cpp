//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <sstream>
#include <vector>

#include "utils.h"

namespace vox {

bool PrintFormatter::capitalize_bool = false;

void PrintFormatter::print(std::ostream &os, bool val) {
    if (capitalize_bool) {
        os << (val ? "True" : "False");
    } else {
        os << val;
    }
}
inline void PrintFormatter::print(std::ostream &os, int16_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, uint16_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, int32_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, uint32_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, int64_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, uint64_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, float16_t val) {
    os << val;
}
inline void PrintFormatter::print(std::ostream &os, float val) {
    os << val;
}

inline void PrintFormatter::print(std::ostream &os, simd::quatf val) {
    os << val.imag().x << "\t" << val.imag().y << "\t" << val.imag().z << "\t" << val.real();
}

inline void PrintFormatter::print(std::ostream &os, simd::float2 val) {
    os << val.x << "\t" << val.y;
}

inline void PrintFormatter::print(std::ostream &os, simd::float3 val) {
    os << val.x << "\t" << val.y << "\t" << val.z;
}

inline void PrintFormatter::print(std::ostream &os, simd::float4 val) {
    os << val.x << "\t" << val.y << "\t" << val.z << "\t" << val.w;
}

inline void PrintFormatter::print(std::ostream &os, simd::float2x2 val) {
    for (auto &column : val.columns) {
        print(os, column);
    }
}

inline void PrintFormatter::print(std::ostream &os, simd::float3x3 val) {
    for (auto &column : val.columns) {
        print(os, column);
    }
}

inline void PrintFormatter::print(std::ostream &os, simd::float4x4 val) {
    for (auto &column : val.columns) {
        print(os, column);
    }
}

inline void PrintFormatter::print(std::ostream &os, Transform val) {
    print(os, val.p);
    print(os, val.q);
}
inline void PrintFormatter::print(std::ostream &os, SpatialVector val) {
    for (int i = 0; i < 6; ++i) {
        os << val[i] << "\t";
    }
}
inline void PrintFormatter::print(std::ostream &os, SpatialMatrix val) {
    for (auto &column : val.columns) {
        print(os, column);
    }
}

std::ostream &operator<<(std::ostream &os, const Dtype &dtype) {
    switch (dtype) {
        case uint8:
            return os << "uint8";
        case uint16:
            return os << "uint16";
        case uint32:
            return os << "uint32";
        case uint64:
            return os << "uint64";
        case int8:
            return os << "int8";
        case int16:
            return os << "int16";
        case int32:
            return os << "int32";
        case int64:
            return os << "int64";
        case float16:
            return os << "float16";
        case float32:
            return os << "float32";
        case quat:
            return os << "quat";
        case float2:
            return os << "float2";
        case float3:
            return os << "float3";
        case float4:
            return os << "float4";
        case float2x2:
            return os << "float2x2";
        case float3x3:
            return os << "float3x3";
        case float4x4:
            return os << "float4x4";
        case transform:
            return os << "transform";
        case spatial_vector:
            return os << "spatial_vector";
        case spatial_matrix:
            return os << "spatial_matrix";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<int> &v) {
    os << "(";
    for (int i = 0; i < v.size(); ++i) {
        os << v[i] << ((i == v.size() - 1) ? "" : ",");
    }
    os << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<size_t> &v) {
    os << "(";
    for (int i = 0; i < v.size(); ++i) {
        os << v[i] << ((i == v.size() - 1) ? "" : ",");
    }
    os << ")";
    return os;
}

}// namespace vox

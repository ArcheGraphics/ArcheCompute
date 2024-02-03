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

std::string type_to_name(const Dtype &d) {
    std::string tname;
    switch (d) {
        case uint8:
            tname = "uint8";
            break;
        case uint16:
            tname = "uint16";
            break;
        case uint32:
            tname = "uint32";
            break;
        case uint64:
            tname = "uint64";
            break;
        case int8:
            tname = "int8";
            break;
        case int16:
            tname = "int16";
            break;
        case int32:
            tname = "int32";
            break;
        case int64:
            tname = "int64";
            break;
        case float16:
            tname = "float16";
            break;
        case float32:
            tname = "float32";
            break;
        case quat:
            tname = "quat";
            break;
        case float2:
            tname = "float2";
            break;
        case float3:
            tname = "float3";
            break;
        case float4:
            tname = "float4";
            break;
        case float2x2:
            tname = "float2x2";
            break;
        case float3x3:
            tname = "float3x3";
            break;
        case float4x4:
            tname = "float4x4";
            break;
        case transform:
            tname = "transform";
            break;
        case spatial_vector:
            tname = "spatial_vector";
            break;
        case spatial_matrix:
            tname = "spatial_matrix";
            break;
    }
    return tname;
}

std::ostream &operator<<(std::ostream &os, const Dtype &dtype) {
    return os << type_to_name(dtype);
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

MTL::Size get_block_dims(int dim0, int dim1, int dim2) {
    int pows[3] = {0, 0, 0};
    int sum = 0;
    while (true) {
        int presum = sum;
        // Check all the pows
        if (dim0 >= (1 << (pows[0] + 1))) {
            pows[0]++;
            sum++;
        }
        if (sum == 10) {
            break;
        }
        if (dim1 >= (1 << (pows[1] + 1))) {
            pows[1]++;
            sum++;
        }
        if (sum == 10) {
            break;
        }
        if (dim2 >= (1 << (pows[2] + 1))) {
            pows[2]++;
            sum++;
        }
        if (sum == presum || sum == 10) {
            break;
        }
    }
    return MTL::Size{1ul << pows[0], 1ul << pows[1], 1ul << pows[2]};
}

}// namespace vox

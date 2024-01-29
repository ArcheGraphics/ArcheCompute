//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
constant int WARP_SIZE = 32;
constant int LOG_WARP_SIZE = 5;

//MARK: - Device-side comparison operators
template<typename T>
struct less {
    bool operator()(T a, T b) { return a < b; }
};
template<typename T>
struct less_equal {
    bool operator()(T a, T b) { return a <= b; }
};
template<typename T>
struct greater {
    bool operator()(T a, T b) { return a > b; }
};
template<typename T>
struct greater_equal {
    bool operator()(T a, T b) { return a >= b; }
};
template<typename T>
struct equal_to {
    bool operator()(T a, T b) { return a == b; }
};
template<typename T>
struct not_equal_to {
    bool operator()(T a, T b) { return a != b; }
};

//MARK: - Device-side arithmetic operators
template<typename T>
struct plus {
    T operator()(T a, T b) { return a + b; }
};

template<typename T>
struct minus {
    T operator()(T a, T b) { return a - b; }
};

template<typename T>
struct multiplies {
    T operator()(T a, T b) { return a * b; }
};

template<typename T>
struct modulus {
    T operator()(T a, T b) { return a % b; }
};

template<typename T>
struct bit_or {
    T operator()(T a, T b) { return a | b; }
};

template<typename T>
struct bit_and {
    T operator()(T a, T b) { return a & b; }
};

template<typename T>
struct bit_xor {
    T operator()(T a, T b) { return a ^ b; }
};

template<typename T>
struct maximum {
    T operator()(T a, T b) { return max(a, b); }
};

template<typename T>
struct minimum {
    T operator()(T a, T b) { return min(a, b); }
};

// MARK: -
template<typename T>
void swap(thread T &a, thread T &b) {
    T c = a;
    a = b;
    b = c;
}

template<typename T>
void swap(threadgroup T &a, threadgroup T &b) {
    T c = a;
    a = b;
    b = c;
}

template<typename T>
void swap(device T &a, device T &b) {
    T c = a;
    a = b;
    b = c;
}

template<typename T>
struct DevicePair {
    T x, y;
};

template<typename T>
DevicePair<T> MakeDevicePair(T x, T y) {
    DevicePair<T> p = {x, y};
    return p;
}

}// namespace vox

//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <metal_stdlib>

using namespace metal;

struct complex64_t;

template<typename T>
static constexpr constant bool can_convert_to_complex64 =
    !is_same_v<T, complex64_t> && is_convertible_v<T, float>;

template<typename T>
static constexpr constant bool can_convert_from_complex64 =
    !is_same_v<T, complex64_t> &&
    (is_convertible_v<float, T> || is_convertible_v<bfloat, T>);

struct complex64_t {
    float real;
    float imag;

    // Constructors
    constexpr complex64_t(float real, float imag) : real(real), imag(imag){};

    // Conversions to complex64_t
    template<
        typename T,
        typename = typename enable_if<can_convert_to_complex64<T>>::type>
    constexpr complex64_t(T x) thread : real(x), imag(0) {}

    template<
        typename T,
        typename = typename enable_if<can_convert_to_complex64<T>>::type>
    constexpr complex64_t(T x) threadgroup : real(x), imag(0) {}

    template<
        typename T,
        typename = typename enable_if<can_convert_to_complex64<T>>::type>
    constexpr complex64_t(T x) device : real(x), imag(0) {}

    template<
        typename T,
        typename = typename enable_if<can_convert_to_complex64<T>>::type>
    constexpr complex64_t(T x) constant : real(x), imag(0) {}

    // Conversions from complex64_t
    template<
        typename T,
        typename = typename enable_if<can_convert_from_complex64<T>>::type>
    constexpr operator T() const thread {
        return static_cast<T>(real);
    }

    template<
        typename T,
        typename = typename enable_if<can_convert_from_complex64<T>>::type>
    constexpr operator T() const threadgroup {
        return static_cast<T>(real);
    }

    template<
        typename T,
        typename = typename enable_if<can_convert_from_complex64<T>>::type>
    constexpr operator T() const device {
        return static_cast<T>(real);
    }

    template<
        typename T,
        typename = typename enable_if<can_convert_from_complex64<T>>::type>
    constexpr operator T() const constant {
        return static_cast<T>(real);
    }
};

constexpr complex64_t operator-(complex64_t x) {
    return {-x.real, -x.imag};
}

constexpr bool operator>=(complex64_t a, complex64_t b) {
    return (a.real > b.real) || (a.real == b.real && a.imag >= b.imag);
}

constexpr bool operator>(complex64_t a, complex64_t b) {
    return (a.real > b.real) || (a.real == b.real && a.imag > b.imag);
}

constexpr bool operator<=(complex64_t a, complex64_t b) {
    return operator>=(b, a);
}

constexpr bool operator<(complex64_t a, complex64_t b) {
    return operator>(b, a);
}

constexpr bool operator==(complex64_t a, complex64_t b) {
    return a.real == b.real && a.imag == b.imag;
}

constexpr complex64_t operator+(complex64_t a, complex64_t b) {
    return {a.real + b.real, a.imag + b.imag};
}

constexpr complex64_t operator-(complex64_t a, complex64_t b) {
    return {a.real - b.real, a.imag - b.imag};
}

constexpr complex64_t operator*(complex64_t a, complex64_t b) {
    return {a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real};
}

constexpr complex64_t operator/(complex64_t a, complex64_t b) {
    auto denom = b.real * b.real + b.imag * b.imag;
    auto x = a.real * b.real + a.imag * b.imag;
    auto y = a.imag * b.real - a.real * b.imag;
    return {x / denom, y / denom};
}

constexpr complex64_t operator%(complex64_t a, complex64_t b) {
    auto real = a.real - (b.real * static_cast<int64_t>(a.real / b.real));
    auto imag = a.imag - (b.imag * static_cast<int64_t>(a.imag / b.imag));
    return {real, imag};
}
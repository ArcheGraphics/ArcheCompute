//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>

namespace vox::benchmark {
// Class to emulate half float on CPU.
class fp16 {
public:
    explicit fp16(uint16_t v) : value_(v) {}
    explicit fp16(float x) { from_float(x); }

    fp16 &operator=(const float &x) {
        from_float(x);
        return *this;
    }
    fp16 &operator=(const int &x) {
        from_float(static_cast<float>(x));
        return *this;
    }
    fp16 &operator+=(const fp16 &x) {
        from_float(to_float() + x.to_float());
        return *this;
    }
    fp16 operator*(const fp16 &rhs) const {
        return fp16(to_float() * rhs.to_float());
    }
    bool operator==(const fp16 &rhs) const { return value_ == rhs.value_; }

    explicit operator float() const { return to_float(); }
    explicit operator uint16_t() const { return get_value(); }

    void from_float(float x);
    [[nodiscard]] float to_float() const;
    [[nodiscard]] uint16_t get_value() const { return value_; }

    friend std::ostream &operator<<(std::ostream &os, const fp16 &value) {
        return os << value.to_float();
    }

private:
    uint16_t value_{};
};

}// namespace vox::benchmark

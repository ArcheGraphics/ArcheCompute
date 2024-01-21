//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#define kEps 0.0f

template<unsigned Length, typename Type>
struct vec_t {
    array<Type, Length> c = {};

    inline vec_t() = default;

    inline vec_t(Type s) {
        for (unsigned i = 0; i < Length; ++i) {
            c[i] = s;
        }
    }

    template<typename OtherType>
    inline explicit vec_t(const thread vec_t<Length, OtherType> &other) {
        for (unsigned i = 0; i < Length; ++i) {
            c[i] = static_cast<Type>(other[i]);
        }
    }

    inline vec_t(Type x, Type y) {
        assert(Length == 2);
        c[0] = x;
        c[1] = y;
    }

    inline vec_t(Type x, Type y, Type z) {
        assert(Length == 3);
        c[0] = x;
        c[1] = y;
        c[2] = z;
    }

    inline vec_t(Type x, Type y, Type z, Type w) {
        assert(Length == 4);
        c[0] = x;
        c[1] = y;
        c[2] = z;
        c[3] = w;
    }

    inline vec_t(const thread initializer_list<Type> &l) {
        for (unsigned i = 0; i < Length; ++i) {
            c[i] = l[i];
        }
    }

    // special screw vector constructor for spatial_vectors:
    inline vec_t(vec_t<3, Type> w, vec_t<3, Type> v) {
        c[0] = w[0];
        c[1] = w[1];
        c[2] = w[2];
        c[3] = v[0];
        c[4] = v[1];
        c[5] = v[2];
    }

    inline Type operator[](int index) const {
        assert(index < Length);
        return c[index];
    }

    inline thread Type &operator[](int index) {
        assert(index < Length);
        return c[index];
    }
    
    inline vec_t<Length, Type> operator+=(vec_t<Length, Type> a) {
        *this = add(*this, a);
        return *this;
    }
};

using vec2b = vec_t<2, int8_t>;
using vec3b = vec_t<3, int8_t>;
using vec4b = vec_t<4, int8_t>;
using vec2ub = vec_t<2, uint8_t>;
using vec3ub = vec_t<3, uint8_t>;
using vec4ub = vec_t<4, uint8_t>;

using vec2s = vec_t<2, int16_t>;
using vec3s = vec_t<3, int16_t>;
using vec4s = vec_t<4, int16_t>;
using vec2us = vec_t<2, uint16_t>;
using vec3us = vec_t<3, uint16_t>;
using vec4us = vec_t<4, uint16_t>;

using vec2i = vec_t<2, int32_t>;
using vec3i = vec_t<3, int32_t>;
using vec4i = vec_t<4, int32_t>;
using vec2ui = vec_t<2, uint32_t>;
using vec3ui = vec_t<3, uint32_t>;
using vec4ui = vec_t<4, uint32_t>;

using vec2l = vec_t<2, int64_t>;
using vec3l = vec_t<3, int64_t>;
using vec4l = vec_t<4, int64_t>;
using vec2ul = vec_t<2, uint64_t>;
using vec3ul = vec_t<3, uint64_t>;
using vec4ul = vec_t<4, uint64_t>;

using vec2h = vec_t<2, half>;
using vec3h = vec_t<3, half>;
using vec4h = vec_t<4, half>;

using vec2 = vec_t<2, float>;
using vec3 = vec_t<3, float>;
using vec4 = vec_t<4, float>;

using vec2f = vec_t<2, float>;
using vec3f = vec_t<3, float>;
using vec4f = vec_t<4, float>;

//--------------
// vec<Length, Type> methods

// MARK: - neg
template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator-(vec_t<Length, Type> a) {
    // NB: this constructor will initialize all ret's components to 0, which is
    // unnecessary...
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = -a[i];
    }

    // Wonder if this does a load of copying when it returns... hopefully not as it's inlined?
    return ret;
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> pos(const thread vec_t<Length, Type> &x) {
    return x;
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> neg(const thread vec_t<Length, Type> &x) {
    return -x;
}

template<typename Type>
inline vec_t<3, Type> neg(const thread vec_t<3, Type> &x) {
    return vec_t<3, Type>(-x.c[0], -x.c[1], -x.c[2]);
}

template<typename Type>
inline vec_t<2, Type> neg(const thread vec_t<2, Type> &x) {
    return vec_t<2, Type>(-x.c[0], -x.c[1]);
}

//MARK: - equality:
template<unsigned Length, typename Type>
inline bool operator==(const thread vec_t<Length, Type> &a, const thread vec_t<Length, Type> &b) {
    for (unsigned i = 0; i < Length; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

//MARK: - scalar multiplication:
template<unsigned Length, typename Type>
inline vec_t<Length, Type> mul(vec_t<Length, Type> a, Type s) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] * s;
    }
    return ret;
}

template<typename Type>
inline vec_t<3, Type> mul(vec_t<3, Type> a, Type s) {
    return vec_t<3, Type>(a.c[0] * s, a.c[1] * s, a.c[2] * s);
}

template<typename Type>
inline vec_t<2, Type> mul(vec_t<2, Type> a, Type s) {
    return vec_t<2, Type>(a.c[0] * s, a.c[1] * s);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> mul(Type s, vec_t<Length, Type> a) {
    return mul(a, s);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator*(Type s, vec_t<Length, Type> a) {
    return mul(a, s);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator*(vec_t<Length, Type> a, Type s) {
    return mul(a, s);
}

// component wise multiplication:
template<unsigned Length, typename Type>
inline vec_t<Length, Type> cw_mul(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] * b[i];
    }
    return ret;
}

//MARK: - division
template<unsigned Length, typename Type>
inline vec_t<Length, Type> div(vec_t<Length, Type> a, Type s) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] / s;
    }
    return ret;
}

template<typename Type>
inline vec_t<3, Type> div(vec_t<3, Type> a, Type s) {
    return vec_t<3, Type>(a.c[0] / s, a.c[1] / s, a.c[2] / s);
}

template<typename Type>
inline vec_t<2, Type> div(vec_t<2, Type> a, Type s) {
    return vec_t<2, Type>(a.c[0] / s, a.c[1] / s);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> div(Type s, vec_t<Length, Type> a) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = s / a[i];
    }
    return ret;
}

template<typename Type>
inline vec_t<3, Type> div(Type s, vec_t<3, Type> a) {
    return vec_t<3, Type>(s / a.c[0], s / a.c[1], s / a.c[2]);
}

template<typename Type>
inline vec_t<2, Type> div(Type s, vec_t<2, Type> a) {
    return vec_t<2, Type>(s / a.c[0], s / a.c[1]);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator/(vec_t<Length, Type> a, Type s) {
    return div(a, s);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator/(Type s, vec_t<Length, Type> a) {
    return div(s, a);
}

// component wise division
template<unsigned Length, typename Type>
inline vec_t<Length, Type> cw_div(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] / b[i];
    }
    return ret;
}

//MARK: - addition
template<unsigned Length, typename Type>
inline vec_t<Length, Type> add(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] + b[i];
    }
    return ret;
}

template<typename Type>
inline vec_t<2, Type> add(vec_t<2, Type> a, vec_t<2, Type> b) {
    return vec_t<2, Type>(a.c[0] + b.c[0], a.c[1] + b.c[1]);
}

template<typename Type>
inline vec_t<3, Type> add(vec_t<3, Type> a, vec_t<3, Type> b) {
    return vec_t<3, Type>(a.c[0] + b.c[0], a.c[1] + b.c[1], a.c[2] + b.c[2]);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator+(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    return add(a, b);
}

//MARK: - subtraction
template<unsigned Length, typename Type>
inline vec_t<Length, Type> sub(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = Type(a[i] - b[i]);
    }
    return ret;
}

template<typename Type>
inline vec_t<2, Type> sub(vec_t<2, Type> a, vec_t<2, Type> b) {
    return vec_t<2, Type>(a.c[0] - b.c[0], a.c[1] - b.c[1]);
}

template<typename Type>
inline vec_t<3, Type> sub(vec_t<3, Type> a, vec_t<3, Type> b) {
    return vec_t<3, Type>(a.c[0] - b.c[0], a.c[1] - b.c[1], a.c[2] - b.c[2]);
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> operator-(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    return sub(a, b);
}

//MARK: - dot product:
template<unsigned Length, typename Type>
inline Type dot(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    Type ret(0);
    for (unsigned i = 0; i < Length; ++i) {
        ret += a[i] * b[i];
    }
    return ret;
}

template<typename Type>
inline Type dot(vec_t<2, Type> a, vec_t<2, Type> b) {
    return a.c[0] * b.c[0] + a.c[1] * b.c[1];
}

template<typename Type>
inline Type dot(vec_t<3, Type> a, vec_t<3, Type> b) {
    return a.c[0] * b.c[0] + a.c[1] * b.c[1] + a.c[2] * b.c[2];
}

template<unsigned Length, typename Type>
inline Type tensordot(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    // corresponds to `np.tensordot()` with all axes being contracted
    return dot(a, b);
}

template<unsigned Length, typename Type>
inline Type extract(const thread vec_t<Length, Type> &a, int idx) {
    return a[idx];
}

template<unsigned Length, typename Type>
inline thread Type *index(thread vec_t<Length, Type> &v, int idx) {
    return &v[idx];
}

template<unsigned Length, typename Type>
inline thread Type *indexref(thread vec_t<Length, Type> *v, int idx) {
    return &((*v)[idx]);
}

//MARK: - length
template<unsigned Length, typename Type>
inline Type length(vec_t<Length, Type> a) {
    return sqrt(dot(a, a));
}

template<unsigned Length, typename Type>
inline Type length_sq(vec_t<Length, Type> a) {
    return dot(a, a);
}

template<typename Type>
inline Type length(vec_t<2, Type> a) {
    return sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1]);
}

template<typename Type>
inline Type length(vec_t<3, Type> a) {
    return sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1] + a.c[2] * a.c[2]);
}

// MARK: - normalize
template<unsigned Length, typename Type>
inline vec_t<Length, Type> normalize(vec_t<Length, Type> a) {
    Type l = length(a);
    if (l > Type(kEps))
        return div(a, l);
    else
        return vec_t<Length, Type>();
}

template<typename Type>
inline vec_t<2, Type> normalize(vec_t<2, Type> a) {
    Type l = sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1]);
    if (l > Type(kEps))
        return vec_t<2, Type>(a.c[0] / l, a.c[1] / l);
    else
        return vec_t<2, Type>();
}

template<typename Type>
inline vec_t<3, Type> normalize(vec_t<3, Type> a) {
    Type l = sqrt(a.c[0] * a.c[0] + a.c[1] * a.c[1] + a.c[2] * a.c[2]);
    if (l > Type(kEps))
        return vec_t<3, Type>(a.c[0] / l, a.c[1] / l, a.c[2] / l);
    else
        return vec_t<3, Type>();
}

template<typename Type>
inline vec_t<3, Type> cross(vec_t<3, Type> a, vec_t<3, Type> b) {
    return {
        Type(a[1] * b[2] - a[2] * b[1]),
        Type(a[2] * b[0] - a[0] * b[2]),
        Type(a[0] * b[1] - a[1] * b[0])};
}

template<unsigned Length, typename Type>
inline bool isfinite(vec_t<Length, Type> x) {
    for (unsigned i = 0; i < Length; ++i) {
        if (!isfinite(x[i])) {
            return false;
        }
    }
    return true;
}

// MARK: - min max
// These two functions seem to compile very slowly
template<unsigned Length, typename Type>
inline vec_t<Length, Type> min(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] < b[i] ? a[i] : b[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> max(vec_t<Length, Type> a, vec_t<Length, Type> b) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = a[i] > b[i] ? a[i] : b[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline Type min(vec_t<Length, Type> v) {
    Type ret = v[0];
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] < ret)
            ret = v[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline Type max(vec_t<Length, Type> v) {
    Type ret = v[0];
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] > ret)
            ret = v[i];
    }
    return ret;
}

template<unsigned Length, typename Type>
inline unsigned argmin(vec_t<Length, Type> v) {
    unsigned ret = 0;
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] < v[ret])
            ret = i;
    }
    return ret;
}

template<unsigned Length, typename Type>
inline unsigned argmax(vec_t<Length, Type> v) {
    unsigned ret = 0;
    for (unsigned i = 1; i < Length; ++i) {
        if (v[i] > v[ret])
            ret = i;
    }
    return ret;
}

// Do I need to specialize these for different lengths?
template<unsigned Length, typename Type>
inline vec_t<Length, Type> atomic_add(thread vec_t<Length, Type> *addr, vec_t<Length, Type> value) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = atomic_add(&(addr->c[i]), value[i]);
    }

    return ret;
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> atomic_min(thread vec_t<Length, Type> *addr, vec_t<Length, Type> value) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = atomic_min(&(addr->c[i]), value[i]);
    }

    return ret;
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> atomic_max(thread vec_t<Length, Type> *addr, vec_t<Length, Type> value) {
    vec_t<Length, Type> ret;
    for (unsigned i = 0; i < Length; ++i) {
        ret[i] = atomic_max(&(addr->c[i]), value[i]);
    }

    return ret;
}

// ok, the original implementation of this didn't take the absolute values.
// I wouldn't consider this expected behavior. It looks like it's only
// being used for bounding boxes at the moment, where this doesn't matter,
// but you often use it for ray tracing where it does. Not sure if the
// fabs() incurs a performance hit...
template<unsigned Length, typename Type>
inline int longest_axis(const thread vec_t<Length, Type> &v) {
    Type lmax = abs(v[0]);
    int ret(0);
    for (unsigned i = 1; i < Length; ++i) {
        Type l = abs(v[i]);
        if (l > lmax) {
            ret = i;
            lmax = l;
        }
    }
    return ret;
}

template<unsigned Length, typename Type>
inline vec_t<Length, Type> lerp(const thread vec_t<Length, Type> &a, const thread vec_t<Length, Type> &b, Type t) {
    return a * (Type(1) - t) + b * t;
}

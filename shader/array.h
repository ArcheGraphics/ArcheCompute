//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// must match constant in types.py
constant int ARRAY_MAX_DIMS = 4;

struct shape_t {
    array<int, ARRAY_MAX_DIMS> dims;

    inline shape_t() : dims() {}

    inline int operator[](int i) const thread {
        assert(i < ARRAY_MAX_DIMS);
        return dims[i];
    }
    
    inline int operator[](int i) const device {
        assert(i < ARRAY_MAX_DIMS);
        return dims[i];
    }
};

inline int extract(shape_t s, int i) {
    return s.dims[i];
}

template<typename T>
struct array_t {
    inline array_t() {}
    inline array_t(int) {}// for backward a = 0 initialization syntax

    array_t(device T *data, int size, device T *grad = nullptr) : data(data), grad(grad) {
        // constructor for 1d array
        shape.dims[0] = size;
        shape.dims[1] = 0;
        shape.dims[2] = 0;
        shape.dims[3] = 0;
        ndim = 1;
        strides[0] = sizeof(T);
        strides[1] = 0;
        strides[2] = 0;
        strides[3] = 0;
    }
    array_t(device T *data, int dim0, int dim1, device T *grad = nullptr) : data(data), grad(grad) {
        // constructor for 2d array
        shape.dims[0] = dim0;
        shape.dims[1] = dim1;
        shape.dims[2] = 0;
        shape.dims[3] = 0;
        ndim = 2;
        strides[0] = dim1 * sizeof(T);
        strides[1] = sizeof(T);
        strides[2] = 0;
        strides[3] = 0;
    }
    array_t(device T *data, int dim0, int dim1, int dim2, device T *grad = nullptr) : data(data), grad(grad) {
        // constructor for 3d array
        shape.dims[0] = dim0;
        shape.dims[1] = dim1;
        shape.dims[2] = dim2;
        shape.dims[3] = 0;
        ndim = 3;
        strides[0] = dim1 * dim2 * sizeof(T);
        strides[1] = dim2 * sizeof(T);
        strides[2] = sizeof(T);
        strides[3] = 0;
    }
    array_t(device T *data, int dim0, int dim1, int dim2, int dim3, device T *grad = nullptr) : data(data), grad(grad) {
        // constructor for 4d array
        shape.dims[0] = dim0;
        shape.dims[1] = dim1;
        shape.dims[2] = dim2;
        shape.dims[3] = dim3;
        ndim = 4;
        strides[0] = dim1 * dim2 * dim3 * sizeof(T);
        strides[1] = dim2 * dim3 * sizeof(T);
        strides[2] = dim3 * sizeof(T);
        strides[3] = sizeof(T);
    }

    inline bool empty() const { return !data; }

    device T *data{nullptr};
    device T *grad{nullptr};
    shape_t shape;
    array<int, ARRAY_MAX_DIMS> strides;
    int ndim;

    inline operator device T *() const { return data; }
    
    inline T operator[](int i) const thread {
        return data[i];
    }
    
    inline T operator[](int i) const device {
        return data[i];
    }
};

template<typename T>
struct indexedarray_t {
    inline indexedarray_t() {}
    inline indexedarray_t(int) {}// for backward a = 0 initialization syntax

    inline bool empty() const { return !arr.data; }

    array_t<T> arr;
    device int *indices[ARRAY_MAX_DIMS];// index array per dimension (can be NULL)
    shape_t shape;                      // element count per dimension (num. indices if indexed, array dim if not)
};

// return stride (in bytes) of the given index
template<typename T>
inline size_t stride(array_t<T> a, int dim) {
    return size_t(a.strides[dim]);
}

// MARK: - byte_offset
template<typename T>
inline device T *data_at_byte_offset(const device array_t<T> &a, size_t byte_offset) {
    return reinterpret_cast<device T *>(reinterpret_cast<device char *>(a.data) + byte_offset);
}

template<typename T>
inline device T *grad_at_byte_offset(const device array_t<T> &a, size_t byte_offset) {
    return reinterpret_cast<device T *>(reinterpret_cast<device char *>(a.grad) + byte_offset);
}

template<typename T>
inline size_t byte_offset(array_t<T> arr, int i) {
    assert(i >= 0 && i < arr.shape[0]);

    return i * stride(arr, 0);
}

template<typename T>
inline size_t byte_offset(array_t<T> arr, int i, int j) {
    assert(i >= 0 && i < arr.shape[0]);
    assert(j >= 0 && j < arr.shape[1]);

    return i * stride(arr, 0) + j * stride(arr, 1);
}

template<typename T>
inline size_t byte_offset(array_t<T> arr, int i, int j, int k) {
    assert(i >= 0 && i < arr.shape[0]);
    assert(j >= 0 && j < arr.shape[1]);
    assert(k >= 0 && k < arr.shape[2]);

    return i * stride(arr, 0) + j * stride(arr, 1) + k * stride(arr, 2);
}

template<typename T>
inline size_t byte_offset(array_t<T> arr, int i, int j, int k, int l) {
    assert(i >= 0 && i < arr.shape[0]);
    assert(j >= 0 && j < arr.shape[1]);
    assert(k >= 0 && k < arr.shape[2]);
    assert(l >= 0 && l < arr.shape[3]);

    return i * stride(arr, 0) + j * stride(arr, 1) + k * stride(arr, 2) + l * stride(arr, 3);
}

// MARK: - index
template<typename T>
inline device T &index(const device array_t<T> &arr, int i) {
    assert(arr.ndim == 1);
    device T &result = *data_at_byte_offset(arr, byte_offset(arr, i));

    return result;
}

template<typename T>
inline device T &index(const device array_t<T> &arr, int i, int j) {
    assert(arr.ndim == 2);
    device T &result = *data_at_byte_offset(arr, byte_offset(arr, i, j));

    return result;
}

template<typename T>
inline device T &index(const device array_t<T> &arr, int i, int j, int k) {
    assert(arr.ndim == 3);
    device T &result = *data_at_byte_offset(arr, byte_offset(arr, i, j, k));

    return result;
}

template<typename T>
inline device T &index(const device array_t<T> &arr, int i, int j, int k, int l) {
    assert(arr.ndim == 4);
    device T &result = *data_at_byte_offset(arr, byte_offset(arr, i, j, k, l));

    return result;
}

template<typename T>
inline device T &index_grad(const device array_t<T> &arr, int i) {
    device T &result = *grad_at_byte_offset(arr, byte_offset(arr, i));

    return result;
}

template<typename T>
inline device T &index_grad(const device array_t<T> &arr, int i, int j) {
    device T &result = *grad_at_byte_offset(arr, byte_offset(arr, i, j));

    return result;
}

template<typename T>
inline device T &index_grad(const device array_t<T> &arr, int i, int j, int k) {
    device T &result = *grad_at_byte_offset(arr, byte_offset(arr, i, j, k));

    return result;
}

template<typename T>
inline device T &index_grad(const device array_t<T> &arr, int i, int j, int k, int l) {
    device T &result = *grad_at_byte_offset(arr, byte_offset(arr, i, j, k, l));

    return result;
}

template<typename T>
inline device T &index(const device indexedarray_t<T> &iarr, int i) {
    assert(iarr.arr.ndim == 1);
    assert(i >= 0 && i < iarr.shape[0]);

    if (iarr.indices[0]) {
        i = iarr.indices[0][i];
        assert(i >= 0 && i < iarr.arr.shape[0]);
    }

    device T &result = *data_at_byte_offset(iarr.arr, byte_offset(iarr.arr, i));

    return result;
}

template<typename T>
inline device T &index(const device indexedarray_t<T> &iarr, int i, int j) {
    assert(iarr.arr.ndim == 2);
    assert(i >= 0 && i < iarr.shape[0]);
    assert(j >= 0 && j < iarr.shape[1]);

    if (iarr.indices[0]) {
        i = iarr.indices[0][i];
        assert(i >= 0 && i < iarr.arr.shape[0]);
    }
    if (iarr.indices[1]) {
        j = iarr.indices[1][j];
        assert(j >= 0 && j < iarr.arr.shape[1]);
    }

    device T &result = *data_at_byte_offset(iarr.arr, byte_offset(iarr.arr, i, j));

    return result;
}

template<typename T>
inline device T &index(const device indexedarray_t<T> &iarr, int i, int j, int k) {
    assert(iarr.arr.ndim == 3);
    assert(i >= 0 && i < iarr.shape[0]);
    assert(j >= 0 && j < iarr.shape[1]);
    assert(k >= 0 && k < iarr.shape[2]);

    if (iarr.indices[0]) {
        i = iarr.indices[0][i];
        assert(i >= 0 && i < iarr.arr.shape[0]);
    }
    if (iarr.indices[1]) {
        j = iarr.indices[1][j];
        assert(j >= 0 && j < iarr.arr.shape[1]);
    }
    if (iarr.indices[2]) {
        k = iarr.indices[2][k];
        assert(k >= 0 && k < iarr.arr.shape[2]);
    }

    device T &result = *data_at_byte_offset(iarr.arr, byte_offset(iarr.arr, i, j, k));

    return result;
}

template<typename T>
inline device T &index(const device indexedarray_t<T> &iarr, int i, int j, int k, int l) {
    assert(iarr.arr.ndim == 4);
    assert(i >= 0 && i < iarr.shape[0]);
    assert(j >= 0 && j < iarr.shape[1]);
    assert(k >= 0 && k < iarr.shape[2]);
    assert(l >= 0 && l < iarr.shape[3]);

    if (iarr.indices[0]) {
        i = iarr.indices[0][i];
        assert(i >= 0 && i < iarr.arr.shape[0]);
    }
    if (iarr.indices[1]) {
        j = iarr.indices[1][j];
        assert(j >= 0 && j < iarr.arr.shape[1]);
    }
    if (iarr.indices[2]) {
        k = iarr.indices[2][k];
        assert(k >= 0 && k < iarr.arr.shape[2]);
    }
    if (iarr.indices[3]) {
        l = iarr.indices[3][l];
        assert(l >= 0 && l < iarr.arr.shape[3]);
    }

    device T &result = *data_at_byte_offset(iarr.arr, byte_offset(iarr.arr, i, j, k, l));

    return result;
}

// MARK: - view
template<typename T>
inline array_t<T> view(device array_t<T> &src, int i) {
    assert(src.ndim > 1);
    assert(i >= 0 && i < src.shape[0]);

    array_t<T> a;
    a.data = data_at_byte_offset(src, byte_offset(src, i));
    a.shape[0] = src.shape[1];
    a.shape[1] = src.shape[2];
    a.shape[2] = src.shape[3];
    a.strides[0] = src.strides[1];
    a.strides[1] = src.strides[2];
    a.strides[2] = src.strides[3];
    a.ndim = src.ndim - 1;

    return a;
}

template<typename T>
inline array_t<T> view(device array_t<T> &src, int i, int j) {
    assert(src.ndim > 2);
    assert(i >= 0 && i < src.shape[0]);
    assert(j >= 0 && j < src.shape[1]);

    array_t<T> a;
    a.data = data_at_byte_offset(src, byte_offset(src, i, j));
    a.shape[0] = src.shape[2];
    a.shape[1] = src.shape[3];
    a.strides[0] = src.strides[2];
    a.strides[1] = src.strides[3];
    a.ndim = src.ndim - 2;

    return a;
}

template<typename T>
inline array_t<T> view(device array_t<T> &src, int i, int j, int k) {
    assert(src.ndim > 3);
    assert(i >= 0 && i < src.shape[0]);
    assert(j >= 0 && j < src.shape[1]);
    assert(k >= 0 && k < src.shape[2]);

    array_t<T> a;
    a.data = data_at_byte_offset(src, byte_offset(src, i, j, k));
    a.shape[0] = src.shape[3];
    a.strides[0] = src.strides[3];
    a.ndim = src.ndim - 3;

    return a;
}

template<typename T>
inline indexedarray_t<T> view(device indexedarray_t<T> &src, int i) {
    assert(src.arr.ndim > 1);

    if (src.indices[0]) {
        assert(i >= 0 && i < src.shape[0]);
        i = src.indices[0][i];
    }

    indexedarray_t<T> a;
    a.arr = view(src.arr, i);
    a.indices[0] = src.indices[1];
    a.indices[1] = src.indices[2];
    a.indices[2] = src.indices[3];
    a.shape[0] = src.shape[1];
    a.shape[1] = src.shape[2];
    a.shape[2] = src.shape[3];

    return a;
}

template<typename T>
inline indexedarray_t<T> view(device indexedarray_t<T> &src, int i, int j) {
    assert(src.arr.ndim > 2);

    if (src.indices[0]) {
        assert(i >= 0 && i < src.shape[0]);
        i = src.indices[0][i];
    }
    if (src.indices[1]) {
        assert(j >= 0 && j < src.shape[1]);
        j = src.indices[1][j];
    }

    indexedarray_t<T> a;
    a.arr = view(src.arr, i, j);
    a.indices[0] = src.indices[2];
    a.indices[1] = src.indices[3];
    a.shape[0] = src.shape[2];
    a.shape[1] = src.shape[3];

    return a;
}

template<typename T>
inline indexedarray_t<T> view(device indexedarray_t<T> &src, int i, int j, int k) {
    assert(src.arr.ndim > 3);

    if (src.indices[0]) {
        assert(i >= 0 && i < src.shape[0]);
        i = src.indices[0][i];
    }
    if (src.indices[1]) {
        assert(j >= 0 && j < src.shape[1]);
        j = src.indices[1][j];
    }
    if (src.indices[2]) {
        assert(k >= 0 && k < src.shape[2]);
        k = src.indices[2][k];
    }

    indexedarray_t<T> a;
    a.arr = view(src.arr, i, j, k);
    a.indices[0] = src.indices[3];
    a.shape[0] = src.shape[3];

    return a;
}

// MARK: - lower_bound
template<typename T>
inline int lower_bound(const device array_t<T> &arr, int arr_begin, int arr_end, T value) {
    assert(arr.ndim == 1);

    int lower = arr_begin;
    int upper = arr_end - 1;

    while (lower < upper) {
        int mid = lower + (upper - lower) / 2;

        if (arr[mid] < value) {
            lower = mid + 1;
        } else {
            upper = mid;
        }
    }

    return lower;
}

template<typename T>
inline int lower_bound(const device array_t<T> &arr, T value) {
    return lower_bound(arr, 0, arr.shape[0], value);
}

// MARK: - atomic_add
template<typename T>
inline T atomic_add(const device array_t<atomic<T>> &buf, int i, T value) {
    return atomic_fetch_add_explicit(&index(buf, i), value, memory_order::memory_order_relaxed);
}

template<typename T>
inline T atomic_add(const device array_t<atomic<T>> &buf, int i, int j, T value) {
    return atomic_fetch_add_explicit(&index(buf, i, j), value, memory_order::memory_order_relaxed);
}

template<typename T>
inline T atomic_add(const device array_t<atomic<T>> &buf, int i, int j, int k, T value) {
    return atomic_fetch_add_explicit(&index(buf, i, j, k), value, memory_order::memory_order_relaxed);
}

template<typename T>
inline T atomic_add(const device array_t<atomic<T>> &buf, int i, int j, int k, int l, T value) {
    return atomic_fetch_add_explicit(&index(buf, i, j, k, l), value, memory_order::memory_order_relaxed);
}

// MARK: - atomic_sub
template<typename T>
inline T atomic_sub(const device array_t<atomic<T>> &buf, int i, T value) {
    return atomic_fetch_sub_explicit(&index(buf, i), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_sub(const device array_t<atomic<T>> &buf, int i, int j, T value) {
    return atomic_fetch_sub_explicit(&index(buf, i, j), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_sub(const device array_t<atomic<T>> &buf, int i, int j, int k, T value) {
    return atomic_fetch_sub_explicit(&index(buf, i, j, k), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_sub(const device array_t<atomic<T>> &buf, int i, int j, int k, int l, T value) {
    return atomic_fetch_sub_explicit(&index(buf, i, j, k, l), value, memory_order::memory_order_relaxed);
}

// MARK: - atomic_min
template<typename T>
inline T atomic_min(const device array_t<atomic<T>> &buf, int i, T value) {
    return atomic_fetch_min_explicit(&index(buf, i), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_min(const device array_t<atomic<T>> &buf, int i, int j, T value) {
    return atomic_fetch_min_explicit(&index(buf, i, j), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_min(const device array_t<atomic<T>> &buf, int i, int j, int k, T value) {
    return atomic_fetch_min_explicit(&index(buf, i, j, k), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_min(const device array_t<atomic<T>> &buf, int i, int j, int k, int l, T value) {
    return atomic_fetch_min_explicit(&index(buf, i, j, k, l), value, memory_order::memory_order_relaxed);
}

// MARK: - atomic_max
template<typename T>
inline T atomic_max(const device array_t<atomic<T>> &buf, int i, T value) {
    return atomic_fetch_max_explicit(&index(buf, i), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_max(const device array_t<atomic<T>> &buf, int i, int j, T value) {
    return atomic_fetch_max_explicit(&index(buf, i, j), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_max(const device array_t<atomic<T>> &buf, int i, int j, int k, T value) {
    return atomic_fetch_max_explicit(&index(buf, i, j, k), value, memory_order::memory_order_relaxed);
}
template<typename T>
inline T atomic_max(const device array_t<atomic<T>> &buf, int i, int j, int k, int l, T value) {
    return atomic_fetch_max_explicit(&index(buf, i, j, k, l), value, memory_order::memory_order_relaxed);
}

// MARK: - address
template<template<typename> class A, typename T>
inline device T *address(const device A<T> &buf, int i) {
    return &index(buf, i);
}
template<template<typename> class A, typename T>
inline device T *address(const device A<T> &buf, int i, int j) {
    return &index(buf, i, j);
}
template<template<typename> class A, typename T>
inline device T *address(const device A<T> &buf, int i, int j, int k) {
    return &index(buf, i, j, k);
}
template<template<typename> class A, typename T>
inline device T *address(const device A<T> &buf, int i, int j, int k, int l) {
    return &index(buf, i, j, k, l);
}

// MARK: - array_store
template<template<typename> class A, typename T>
inline void array_store(const device A<T> &buf, int i, T value) {
    index(buf, i) = value;
}
template<template<typename> class A, typename T>
inline void array_store(const device A<T> &buf, int i, int j, T value) {
    index(buf, i, j) = value;
}
template<template<typename> class A, typename T>
inline void array_store(const device A<T> &buf, int i, int j, int k, T value) {
    index(buf, i, j, k) = value;
}
template<template<typename> class A, typename T>
inline void array_store(const device A<T> &buf, int i, int j, int k, int l, T value) {
    index(buf, i, j, k, l) = value;
}

template<typename T>
inline void store(device T *address, T value) {
    *address = value;
}

template<typename T>
inline T load(device T *address) {
    T value = *address;

    return value;
}

// select operator to check for array being null
template<typename T1, typename T2>
inline T2 select(const device array_t<T1> &arr, const device T2 &a, const device T2 &b) { return arr.data ? b : a; }

// stub for the case where we have an nested array inside a struct and
// atomic add the whole struct onto an array (e.g.: during backwards pass)
template<typename T>
inline void atomic_add(device array_t<T> *, array_t<T>) {}

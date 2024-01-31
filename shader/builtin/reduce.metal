//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_atomic>
#include <metal_simdgroup>

#include "reduce.h"
#include "utils.h"

using namespace metal;

static constant constexpr int REDUCE_N_READS = 16;

static constant uint8_t simd_size = 32;

template <typename T, typename Op>
[[kernel]] void init_reduce(
    device T *out [[buffer(0)]],
    uint tid [[thread_position_in_grid]]) {
  out[tid] = Op::init;
}

#define instantiate_init_reduce(name, otype, op) \
  template [[host_name("i" #name)]] \
    [[kernel]] void init_reduce<otype, op>( \
      device otype *out [[buffer(1)]], \
      uint tid [[thread_position_in_grid]]);

///////////////////////////////////////////////////////////////////////////////
//MARK: - All reduce
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, typename Op, int N_READS=REDUCE_N_READS>
[[kernel]] void all_reduce(
    const device T *in [[buffer(0)]],
    device mlx_atomic<U> *out [[buffer(1)]],
    const device size_t& in_size [[buffer(2)]],
    uint gid [[thread_position_in_grid]],
    uint lid [[thread_position_in_threadgroup]],
    uint grid_size [[threads_per_grid]],
    uint simd_per_group [[simdgroups_per_threadgroup]],
    uint simd_lane_id [[thread_index_in_simdgroup]],
    uint simd_group_id [[simdgroup_index_in_threadgroup]]) {
  // NB: this kernel assumes threads_per_threadgroup is at most
  // 1024. This way with a simd_size of 32, we are guaranteed to
  // complete the reduction in two steps of simd-level reductions.

  Op op;
  threadgroup U local_vals[simd_size];
      
  U total_val = Op::init;

  in += gid * N_READS;
      
  int r = 0;
  for(; r < (int)ceildiv(in_size, grid_size * N_READS) - 1; r++) {
    U vals[N_READS] = {op.init};

    for(int i = 0; i < N_READS; i++) {
      vals[i] = static_cast<U>(in[i]);
    }
    for(int i = 0; i < N_READS; i++) {
      total_val = op(vals[i], total_val);
    }

    in += grid_size * N_READS;
  }

  // Separate case for the last set as we close the reduction size
  size_t curr_idx = (gid + r * (size_t)grid_size) * N_READS;
  if (curr_idx < in_size) {
    int max_reads = in_size - curr_idx;
    T vals[N_READS];

    for(int i = 0, idx = 0; i < N_READS; i++, idx++) {
      idx = idx < max_reads ? idx : max_reads - 1;
      vals[i] = in[idx];
    }
    for(int i = 0; i < N_READS; i++) {
      U val = i < max_reads ? vals[i] : Op::init;
      total_val = op(static_cast<U>(val), total_val);
    }
  }

  // Reduction within simd group
  total_val = op.simd_reduce(total_val);
  if (simd_lane_id == 0) {
    local_vals[simd_group_id] = total_val;
  }
  
  // Reduction within thread group
  threadgroup_barrier(mem_flags::mem_threadgroup);
  total_val = lid < simd_per_group ? local_vals[lid] : op.init;
  total_val = op.simd_reduce(total_val);

  // Reduction across threadgroups
  if (lid == 0) {
    op.atomic_update(out, total_val);
  }
}

#define instantiate_all_reduce(name, itype, otype, op) \
  template [[host_name("all_reduce_" #name)]] \
  [[kernel]] void all_reduce<itype, otype, op>( \
      const device itype *in [[buffer(0)]], \
      device mlx_atomic<otype> *out [[buffer(1)]], \
      const device size_t& in_size [[buffer(2)]], \
      uint gid [[thread_position_in_grid]], \
      uint lid [[thread_position_in_threadgroup]], \
      uint grid_size [[threads_per_grid]], \
      uint simd_per_group [[simdgroups_per_threadgroup]], \
      uint simd_lane_id [[thread_index_in_simdgroup]], \
      uint simd_group_id [[simdgroup_index_in_threadgroup]]);

#define instantiate_same_reduce(name, tname, type, op) \
  instantiate_init_reduce(name ##tname, type, op<type>) \
  instantiate_all_reduce(name ##tname, type, type, op<type>)

instantiate_same_reduce(sum, uint8, uint8_t, Sum)
instantiate_same_reduce(sum, uint16, uint16_t, Sum)
instantiate_same_reduce(sum, uint32, uint32_t, Sum)
instantiate_same_reduce(sum, int8, int8_t, Sum)
instantiate_same_reduce(sum, int16, int16_t, Sum)
instantiate_same_reduce(sum, int32, int32_t, Sum)
instantiate_same_reduce(sum, float16, half, Sum)
instantiate_same_reduce(sum, float32, float, Sum)

instantiate_same_reduce(prod, uint8, uint8_t, Prod)
instantiate_same_reduce(prod, uint16, uint16_t, Prod)
instantiate_same_reduce(prod, uint32, uint32_t, Prod)
instantiate_same_reduce(prod, int8, int8_t, Prod)
instantiate_same_reduce(prod, int16, int16_t, Prod)
instantiate_same_reduce(prod, int32, int32_t, Prod)
instantiate_same_reduce(prod, float16, half, Prod)
instantiate_same_reduce(prod, float32, float, Prod)

instantiate_same_reduce(min_, uint8, uint8_t, Min)
instantiate_same_reduce(min_, uint16, uint16_t, Min)
instantiate_same_reduce(min_, uint32, uint32_t, Min)
instantiate_same_reduce(min_, int8, int8_t, Min)
instantiate_same_reduce(min_, int16, int16_t, Min)
instantiate_same_reduce(min_, int32, int32_t, Min)
instantiate_same_reduce(min_, float16, half, Min)
instantiate_same_reduce(min_, float32, float, Min)

instantiate_same_reduce(max_, uint8, uint8_t, Max)
instantiate_same_reduce(max_, uint16, uint16_t, Max)
instantiate_same_reduce(max_, uint32, uint32_t, Max)
instantiate_same_reduce(max_, int8, int8_t, Max)
instantiate_same_reduce(max_, int16, int16_t, Max)
instantiate_same_reduce(max_, int32, int32_t, Max)
instantiate_same_reduce(max_, float16, half, Max)
instantiate_same_reduce(max_, float32, float, Max)

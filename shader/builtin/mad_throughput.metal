//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

struct alignas(8) Arguments {
    device float4* inputA;
    device float4* inputB;
    device float4* output;
};

template<int kLoopSize>
kernel void mad_throughput(constant Arguments &args,
                           uint3 tpig [[ thread_position_in_grid ]]) {
    float4 a = args.inputA[tpig.x];
    float4 b = args.inputB[tpig.x];
    float4 c = float4(1.f, 1.f, 1.f, 1.f);
    for(int i = 0; i < kLoopSize; i++) {
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
        c = a * c + b;
    }
    args.output[tpig.x] = c;
}

#define mad_throughput(count) \
template [[host_name("mad_throughput_" #count)]] \
    kernel void mad_throughput<count>(constant Arguments &args,\
    uint3 tpig [[ thread_position_in_grid ]]);

mad_throughput(100000)
mad_throughput(200000)

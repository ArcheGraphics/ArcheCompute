//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "reduce.h"
#include "kernel.h"
#include "utils.h"

namespace vox {
void reduce(const Array &src, Array &dst, ReduceType reduce_type, uint32_t stream) {
    std::string op_name;
    switch (reduce_type) {
        case ReduceType::And:
            op_name = "and";
            break;
        case ReduceType::Or:
            op_name = "or";
            break;
        case ReduceType::Sum:
            op_name = "sum";
            break;
        case ReduceType::Prod:
            op_name = "prod";
            break;
        case ReduceType::Min:
            op_name = "min_";
            break;
        case ReduceType::Max:
            op_name = "max_";
            break;
    }

    auto init = Kernel::builder()
                      .entry("i" + op_name + type_to_name(dst.dtype()))
                      .build();
    size_t nthreads = dst.size();
    init.set_threads(nthreads);

    NS::UInteger thread_group_size = init.max_total_threads_per_threadgroup();
    if (thread_group_size > nthreads) {
        thread_group_size = nthreads;
    }
    init.set_threads_per_thread_group(thread_group_size);
    init({dst}, stream);
}

}// namespace vox
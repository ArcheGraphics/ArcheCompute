from arche_compute import *
import numpy as np

if __name__ == '__main__':
    src = arc.Array(val=np.array([1, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0], dtype=np.float32),
                    dtype=arc.float32)

    kernelSrc = '''
    #include <metal_stdlib>
        using namespace metal;

        struct alignas(16) Arguments {
            device float* buffer;
        };


        kernel void kernel_main(constant Arguments &args,
                                uint2 index [[thread_position_in_grid]],
                                uint2 gridSize [[threads_per_grid]])
        {
            args.buffer[0] = 10.0;
        }
    '''
    kernel = arc.Kernel.builder() \
        .entry("kernel_main") \
        .lib_name("custom_lib") \
        .source(kernelSrc) \
        .build()

    kernel.launch([1, 1, 1], [1, 1, 1], [src])
    arc.synchronize(True)

    print(src.dtype)
    print(src.tolist())

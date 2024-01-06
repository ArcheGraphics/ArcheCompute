from arche_compute import *

if __name__ == '__main__':
    device = create_device(DeviceType.GPU)
    print(device.name())

    stream = device.create_stream()
    buffer = device.create_buffer(4, 10)
    capture = device.debug_capture()
    capture_scope = capture.create_scope("arche-capture", DebugCaptureOption())

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
    kernel = device.create_kernel(kernelSrc, "kernel_main")

    capture_scope.start_debug_capture()
    capture_scope.mark_begin()
    stream.synchronize()
    capture_scope.mark_end()
    capture_scope.stop_debug_capture()

//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <memory>

namespace vox {
class Buffer;
class Stream;
class Kernel;
class Counter;
class DeviceExtension;

class Device {
public:
    Device() = default;
    virtual ~Device() = default;

    virtual std::string name() = 0;

    virtual std::string builtin_lib_name() = 0;

    virtual std::shared_ptr<Stream> create_stream() = 0;

    virtual std::shared_ptr<Buffer> create_buffer(size_t element_stride, size_t size) = 0;

    virtual std::shared_ptr<Kernel> create_kernel_with_source(const std::string &code, const std::string &entry) = 0;

    virtual std::shared_ptr<Kernel> create_kernel_from_library(const std::string &lib_name, const std::string &entry) = 0;

    virtual void register_library(const std::string &lib_name) = 0;

    virtual std::shared_ptr<Counter> create_counter(uint32_t count) = 0;
};

}// namespace vox
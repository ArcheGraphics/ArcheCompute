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

class Device {
public:
    Device() = default;
    virtual ~Device() = default;

    virtual std::string_view name() = 0;

    virtual std::shared_ptr<Stream> create_stream() = 0;

    virtual std::shared_ptr<Buffer> create_buffer(size_t size) = 0;

    virtual std::shared_ptr<Kernel> create_kernel(const std::string &code, const std::string &entry) = 0;
};

}// namespace vox
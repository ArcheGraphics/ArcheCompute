//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <variant>
#include <memory>

namespace vox {
class Buffer;

using BufferArgument = std::shared_ptr<Buffer>;

using UniformArgument = std::vector<uint8_t>;

using Argument = std::variant<BufferArgument, UniformArgument>;
}// namespace vox
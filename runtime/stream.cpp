//  Copyright (c) 2024 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scheduler.h"

namespace vox {
Stream default_stream() {
    return scheduler().get_default_stream();
}

void set_default_stream(Stream s) {
    return scheduler().set_default_stream(s);
}

Stream new_stream(Device d) {
    return scheduler().new_stream(d);
}
}// namespace vox
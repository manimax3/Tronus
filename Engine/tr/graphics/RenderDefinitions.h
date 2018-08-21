#pragma once
#include <tr.h>

#include "../util/enum.h"

namespace tr {
BETTER_ENUM(RenderStrategy,
            int,
            ForwardRendering,
            DefferedRendering,
            BatchRendering,
            InstancedRendering);
}

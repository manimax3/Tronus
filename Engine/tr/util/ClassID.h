#pragma once
#include <tr.h>

namespace tr {

struct ClassID_DefaultTag {
};

namespace detail {
    template<typename Tag>
    int64 type_counter = 0;
}

template<typename T, typename Tag = ClassID_DefaultTag>
inline const auto ClassID = ++(detail::type_counter<Tag>);
}

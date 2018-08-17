#pragma once
#include "spdlog/fmt/fmt.h"
#include <exception>

namespace tr {
struct NotImplementedError : public std::logic_error {
    explicit NotImplementedError(const char *name) noexcept
        : std::logic_error(fmt::format("Function {} is not implemented!", name))
    {
    }
};
}

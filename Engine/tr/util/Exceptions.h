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

struct ShaderInterfaceException : public std::runtime_error {
    explicit ShaderInterfaceException(const char *reason) noexcept
        : std::runtime_error(
              fmt::format("ShaderInterface incomaptible: {}", reason))
    {
    }
};
}

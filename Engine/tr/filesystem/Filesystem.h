#pragma once

#include "../core/Engine.h"
#include <tr.h>

namespace tr::fs {

class Filesystem : public Subsystem<Filesystem> {
public:
    bool Initialize(Engine *engine) override;
    bool Shutdown() override;

    template<typename Device, typename... Args>
    bool Mount(const std::string &path, Args &&... args);

    template<typename... Args>
    std::streambuff Get(const std::string &name, Args &&... args);

private:
    std::unordered_map<std::string, class Device *> mDevices;
};
}

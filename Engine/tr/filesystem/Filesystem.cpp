#include "Filesystem.h"
#include "../util/Log.h"
#include <array>
#include <dirent.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

using namespace tr;

bool fs::FileExists(const std::string &name)
{
    return access(name.c_str(), F_OK) != -1;
}

bool fs::DirExists(const std::string &name)
{
    DIR *dir = opendir(name.c_str());
    if (dir) {
        closedir(dir);
        return true;
    } else if (errno == ENOENT)
        return false;
    else
        Log().warn("Something went wrong checking for a directory: {}", name);

    return false;
}

std::string fs::CurrentWorkingDir()
{
    std::array<char, 512> data;
    std::memset(data.data(), 0, 512);
    getcwd(data.data(), 512);
    return data.data();
}

std::string fs::GetExecutablePath()
{
    std::array<char, 512> data;
    std::memset(data.data(), 0, 512);
    readlink("/proc/self/exe", data.data(), 512);
    std::string path(data.data());
    const auto  ep = path.substr(0, path.find_last_of('/') + 1);
    return ep;
}

std::vector<std::string> fs::FilesIn(const std::string &name) { return {}; }

std::vector<std::string> fs::DirsIn(const std::string &name) { return {}; }

std::string tr::fs::FileName(std::string_view path) {
    boost::filesystem::path p(path.begin(), path.end());
    auto fn = p.filename();
    return fn.generic_string<std::string>();
}

std::string tr::fs::Extension(std::string_view file) {
    boost::filesystem::path p(file.begin(), file.end());
    auto fn = p.extension();
    return fn.generic_string<std::string>();
}

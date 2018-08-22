#pragma once
#include <string>
#include <tr.h>
#include <vector>

namespace tr::fs {
bool FileExists(const std::string &name);
bool DirExists(const std::string &name);

std::string FileName(std::string_view path);
std::string Extension(std::string_view file);
std::string Stem(std::string_view file);

std::string CurrentWorkingDir();
std::string GetExecutablePath();

std::vector<std::string> FilesIn(const std::string &name);
std::vector<std::string> DirsIn(const std::string &name);
}

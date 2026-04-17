#pragma once
#include <cstddef>
#include <filesystem>
#include <vector>

namespace Ghost::Utils {
std::vector<std::byte> readFile(const std::filesystem::path &filepath);
}

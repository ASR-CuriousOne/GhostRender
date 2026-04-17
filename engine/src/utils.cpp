#include <Ghost/utils.hpp>
#include <fstream>

namespace Ghost::Utils {

std::vector<std::byte> readFile(const std::filesystem::path &filepath) {
    std::ifstream shaderFile(filepath, std::ios::ate | std::ios::binary);

    size_t fileSize = (size_t)shaderFile.tellg();
    std::vector<std::byte> buffer(fileSize);

    shaderFile.seekg(0);
    if (!shaderFile.read(reinterpret_cast<char *>(buffer.data()), fileSize)) {
        throw std::runtime_error("Failed to read file.");
    }
    shaderFile.close();

    return buffer;
}
} // namespace Ghost::Utils

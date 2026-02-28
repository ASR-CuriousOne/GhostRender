cmake_minimum_required(VERSION 3.20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(GhostRenderEngine STATIC ${ENGINE_SOURCES})
target_compile_definitions(GhostRenderEngine PUBLIC GLFW_INCLUDE_VULKAN)

target_include_directories(GhostRenderEngine PUBLIC
	"./include")

target_link_libraries(GhostRenderEngine
    PUBLIC
        Vulkan::Vulkan
        glfw
)

cmake_minimum_required(VERSION 3.20)

find_package(glfw3 REQUIRED)

target_compile_definitions(GhostRenderEngine PUBLIC GLFW_INCLUDE_VULKAN)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(GhostRenderApp ${APP_SOURCES})

target_link_libraries(GhostRenderApp PRIVATE GhostRenderEngine)
target_link_libraries(GhostRenderEngine
				PUBLIC
								glfw
)


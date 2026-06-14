cmake_minimum_required(VERSION 3.20)

file(GLOB IMGUI_SOURCES
				"../vendor/imgui/*.cpp"
				"../vendor/imgui/backends/imgui_impl_glfw.cpp"
				"../vendor/imgui/backends/imgui_impl_vulkan.cpp"
)

find_package(glfw3 REQUIRED)

target_compile_definitions(GhostRenderEngine PUBLIC GLFW_INCLUDE_VULKAN)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "*.cpp")
set(ALL_SOURCES ${APP_SOURCES} ${IMGUI_SOURCES})
add_executable(GhostRenderApp ${ALL_SOURCES})

target_include_directories(GhostRenderApp PRIVATE "../vendor/imgui" "../vendor/imgui/backends")

target_link_libraries(GhostRenderApp PRIVATE GhostRenderEngine)
target_link_libraries(GhostRenderEngine
				PUBLIC
								glfw
)


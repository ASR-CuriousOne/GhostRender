cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(GhostRenderApp ${APP_SOURCES})

target_link_libraries(GhostRenderApp PRIVATE GhostRenderEngine)


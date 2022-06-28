cmake_minimum_required(VERSION 3.22)
project(FlatEngine)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(src)
include_directories(src/core)
include_directories(vendor/)

target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/assimp-vc143-mt.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/freetype.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/glew32s.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/glfw3.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/irrKlang.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/SOIL.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/yaml-cpp.lib)
target_link_libraries(FlatEngine ${CMAKE_SOURCE_DIR}/vendor/lib/yaml-cppd.lib)


add_executable(FlatEngine
        src
        vendor
)

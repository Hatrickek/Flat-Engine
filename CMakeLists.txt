cmake_minimum_required(VERSION 3.22)

project(FlatEngine) 
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -DGLFW_INCLUDE_NONE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)
link_directories(${PROJECT_NAME}  ${CMAKE_SOURCE_DIR}/vendor/lib)

include_directories(${CMAKE_SOURCE_DIR}/vendor)
find_package(OpenGL REQUIRED)
if(WIN32)
  set(LIBS glfw3 opengl32 assimp freetype irrKlang yaml-cpp)
endif(WIN32)
#configure_file()
include_directories(${CMAKE_SOURCE_DIR}/vendor/yaml-cpp/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/ImGui/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/GLM/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/GLFW/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/GLAD/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/assimp/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/spdlog/)

include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB SOURCES 
        "src/*.cpp" 
        "src/core/*.cpp" 
        "src/render/*.cpp" 
        "src/ui/*.cpp" 
        "src/ui/panels/*.cpp"
        "src/render/*.cpp"
        "src/render/animation/*.cpp"
        "src/utils/*.cpp"
        "vendor/imgui/*.cpp"
		"vendor/imgui/backends/imgui_impl_opengl3.cpp"
		"vendor/imgui/backends/imgui_impl_opengl3.h"
		"vendor/imgui/backends/imgui_impl_glfw.cpp"
		"vendor/imgui/backends/imgui_impl_glfw.h"
		"vendor/imguizmo/*.cpp"
		"vendor/glad/glad.h"
		"vendor/glad.c"
		"vendor/glm/*.hpp"
		"vendor/GLFW/*.h"
		"vendor/stb_image.h"
		"vendor/stb_image.cpp"
		)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(FlatEngine ${OPENGL_LIBRARIES})
target_link_libraries(FlatEngine glfw3)
#target_link_libraries(FlatEngine yaml-cpp)
target_link_libraries(FlatEngine yaml-cppd)
target_link_libraries(FlatEngine assimp-vc143-mt)

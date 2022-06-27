cmake_minimum_required(VERSION 3.22)
project(FlatEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)
include_directories(src/core)
include_directories(vendor/)


add_executable(FlatEngine
        src/core
        src
        vendor
)

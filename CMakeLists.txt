cmake_minimum_required(VERSION 3.10)
project(BasicInterpreterOJ CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SRC_FILES src/*.cpp src/utils/*.cpp)
add_executable(code ${SRC_FILES})


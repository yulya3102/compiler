cmake_minimum_required(VERSION 3.1)

project(lc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra -Wno-unused-parameter")

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(utils)
add_subdirectory(parse)
add_subdirectory(sem)
add_subdirectory(gen)
add_subdirectory(optimise)
add_subdirectory(lcc)

enable_testing()
add_subdirectory(testing)

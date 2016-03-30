cmake_minimum_required(VERSION 2.8)

project(lc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(parse)

add_executable(parser ast/l.cpp main.cpp codegen.cpp)
target_link_libraries(parser libparser LLVMCore LLVMSupport)

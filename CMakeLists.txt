cmake_minimum_required(VERSION 2.8)

project(lc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra -Wno-unused-parameter")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(utils)
add_subdirectory(parse)
add_subdirectory(sem)
add_subdirectory(gen)

add_executable(lcc main.cpp sem/ctx.h sem/types.h sem/l.h sem/l.cpp utils/fmap.h utils/undefined.h)
target_link_libraries(lcc libparser libgen LLVMCore LLVMSupport)

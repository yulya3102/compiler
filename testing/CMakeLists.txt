add_subdirectory(bin2cpp)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/semantic_subscope.h ${CMAKE_CURRENT_SOURCE_DIR}/semantic_subscope.cpp
        testing::semantic_subscope ${CMAKE_CURRENT_SOURCE_DIR}/semantic_subscope.lc)

find_package(GTest REQUIRED)

add_executable(test_compiler main.cpp
    semantic_subscope.h semantic_subscope.cpp
)

llvm_map_components_to_libnames(LLVM_LIBS support core)
target_link_libraries(test_compiler lccomp ${GTEST_LIBRARY} ${LLVM_LIBS} pthread)

add_test(all_tests test_compiler)

find_package(GTest REQUIRED)

add_executable(test_compiler main.cpp)

llvm_map_components_to_libnames(LLVM_LIBS support core)
target_link_libraries(test_compiler lccomp ${GTEST_LIBRARY} ${LLVM_LIBS} pthread)

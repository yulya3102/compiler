find_package(GTest REQUIRED)

add_executable(test_compiler main.cpp)

target_link_libraries(test_compiler ${GTEST_LIBRARY} pthread)

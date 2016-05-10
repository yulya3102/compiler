add_subdirectory(bin2cpp)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/semantic_subscope.h ${CMAKE_CURRENT_SOURCE_DIR}/semantic_subscope.cpp
        testing::semantic_subscope ${CMAKE_CURRENT_SOURCE_DIR}/../examples/semantic_subscope.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_fact.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_fact.cpp
        testing::compiled_fact ${CMAKE_CURRENT_SOURCE_DIR}/../examples/fact.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_scope.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_scope.cpp
        testing::compiled_scope ${CMAKE_CURRENT_SOURCE_DIR}/../examples/scope.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_func_pointer.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_func_pointer.cpp
        testing::compiled_func_pointer ${CMAKE_CURRENT_SOURCE_DIR}/../examples/func_pointer.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_pointer.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_pointer.cpp
        testing::compiled_pointer ${CMAKE_CURRENT_SOURCE_DIR}/../examples/pointer.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_pointer_pointer.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_pointer_pointer.cpp
        testing::compiled_pointer_pointer ${CMAKE_CURRENT_SOURCE_DIR}/../examples/pointer_pointer.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_alloca_while.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_alloca_while.cpp
        testing::compiled_alloca_while ${CMAKE_CURRENT_SOURCE_DIR}/../examples/alloca_while.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_arg_var.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_arg_var.cpp
        testing::compiled_arg_var ${CMAKE_CURRENT_SOURCE_DIR}/../examples/arg_var.lc)

BIN2CPP(${CMAKE_CURRENT_SOURCE_DIR}/compiled_fib.h ${CMAKE_CURRENT_SOURCE_DIR}/compiled_fib.cpp
        testing::compiled_fib ${CMAKE_CURRENT_SOURCE_DIR}/../examples/fib.lc)

find_package(GTest REQUIRED)

add_executable(test_semantic semantic.cpp
    semantic_subscope.h semantic_subscope.cpp
)

add_executable(test_compiled main.cpp
    common.h common.cpp

    compiled_fact.h compiled_fact.cpp
    compiled_scope.h compiled_scope.cpp
    compiled_func_pointer.h compiled_func_pointer.cpp
    compiled_pointer.h compiled_pointer.cpp
    compiled_pointer_pointer.h compiled_pointer_pointer.cpp
    compiled_alloca_while.h compiled_alloca_while.cpp
    compiled_arg_var.h compiled_arg_var.cpp
    compiled_fib.h compiled_fib.cpp
)

llvm_map_components_to_libnames(LLVM_LIBS support core)
target_link_libraries(test_semantic lccomp ${GTEST_LIBRARY} ${LLVM_LIBS} pthread)
target_link_libraries(test_compiled lccomp ${GTEST_LIBRARY} ${LLVM_LIBS} pthread)

add_test(semantic test_semantic)
add_test(compiled test_compiled)

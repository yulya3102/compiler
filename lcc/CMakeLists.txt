add_executable(lcc main.cpp)
llvm_map_components_to_libnames(llvm_libs support core)
target_link_libraries(lcc parser gen utils sem ${llvm_libs})

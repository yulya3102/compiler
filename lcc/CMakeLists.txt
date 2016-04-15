add_library(lccomp lcc.h lcc.cpp)
llvm_map_components_to_libnames(llvm_libs support core)
target_link_libraries(lccomp parser gen utils sem ${llvm_libs})

add_executable(lcc main.cpp)
target_link_libraries(lcc lccomp)

add_library(gen
    gen.h gen.cpp
    ast/l.h
)

llvm_map_components_to_libnames(llvm_libs support core)
target_link_libraries(gen ${llvm_libs})

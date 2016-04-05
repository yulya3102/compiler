add_library(gen SHARED
    gen.h gen.cpp
)

target_link_libraries(gen libutils)

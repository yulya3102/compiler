add_library(libgen SHARED
    gen.h gen.cpp
)

target_link_libraries(libgen libutils)

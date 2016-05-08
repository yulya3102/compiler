add_library(optimise
    l.h tco.cpp accum.cpp
)

target_link_libraries(optimise utils ast)

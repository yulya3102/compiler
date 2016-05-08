add_library(optimise
    l.h tco.cpp accum.cpp common.cpp
)

target_link_libraries(optimise utils ast)

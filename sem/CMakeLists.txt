add_library(sem
    ctx.h func.h error.h
    types.h types.cpp
    l.h l.cpp
)

target_link_libraries(sem utils ast)

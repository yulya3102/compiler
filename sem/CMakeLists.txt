add_library(sem SHARED
    ctx.h types.h func.h error.h
    l.h l.cpp
)

target_link_libraries(sem utils)

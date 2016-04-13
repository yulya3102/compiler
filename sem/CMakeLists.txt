add_library(sem SHARED
    ctx.h types.h
    l.h l.cpp
)

target_link_libraries(sem utils)

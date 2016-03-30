add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
    COMMAND flex --outfile ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/l.lex
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/l.lex
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/l.cpp ${CMAKE_CURRENT_SOURCE_DIR}/gen_l.h
    COMMAND bison
        --language=C++
        --defines=${CMAKE_CURRENT_SOURCE_DIR}/gen_l.h
        --output=${CMAKE_CURRENT_SOURCE_DIR}/l.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/l.y++
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/l.y++
)

add_library(libparser SHARED
    parser.cpp lexer.cpp
    l.h l.cpp
)

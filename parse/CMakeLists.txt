function(add_lexer LEX_INPUT CPP_OUTPUT)
    add_custom_command(
        OUTPUT ${CPP_OUTPUT}
        COMMAND flex --outfile ${CPP_OUTPUT} ${LEX_INPUT}
        DEPENDS ${LEX_INPUT}
    )
endfunction(add_lexer)

function(add_parser GRAMMAR_INPUT DEFINES_OUTPUT CPP_OUTPUT)
add_custom_command(
    OUTPUT ${CPP_OUTPUT} ${DEFINES_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/location.hh
    COMMAND bison
        --language=C++
        --defines=${DEFINES_OUTPUT}
        --output=${CPP_OUTPUT}
        ${GRAMMAR_INPUT}
    DEPENDS ${GRAMMAR_INPUT}
)
endfunction(add_parser)

add_lexer(${CMAKE_CURRENT_SOURCE_DIR}/l.lex ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp)
add_parser(
    ${CMAKE_CURRENT_SOURCE_DIR}/l.y++
    ${CMAKE_CURRENT_SOURCE_DIR}/gen_l.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/l.cpp
)

add_library(ast
    ast/l.h ast/l.cpp ast/types.h ast/expr.h ast/decl.h ast/statement.h
    location.hh
)

add_library(parser
    parser.h parser.cpp
    lexer.h lexer.cpp
    l.h
    gen_l.h l.cpp
)
target_link_libraries(parser ast)

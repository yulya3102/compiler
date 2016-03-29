parser: parser.cpp lexer.cpp c.cpp main.cpp ast_parser.cpp
	g++ -std=c++1y -o $@ $^

lexer.cpp: c.lex
	flex --outfile $@ --c++ $<

parser.cpp: c.y++
	bison --defines=parser.h --output=$@ --language=C++ $<

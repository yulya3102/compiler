parser: parser.cpp lexer.cpp
	g++ -std=c++11 -o $@ $^

lexer.cpp: c.lex
	flex --outfile $@ --c++ $<

parser.cpp: c.y++
	bison --defines=parser.h --output=$@ --language=C++ $<

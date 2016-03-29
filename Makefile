parser: parser.cpp lexer.cpp ast/l.cpp main.cpp ast_parser.cpp codegen.cpp
	g++ -I. `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++1y -o $@ $^

lexer.cpp: c.lex
	flex --outfile $@ --c++ $<

parser.cpp: c.y++
	bison --defines=parser.h --output=$@ --language=C++ $<

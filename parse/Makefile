all: libparser.so

libparser.so: parser.o lexer.o l.o
	g++ -shared $^ -o $@

%.o: %.cpp
	g++ -fPIC -c -std=c++1y -I.. -o $@ $^

l.cpp: l.y++
	bison --language=C++ --defines=l.h --output=$@ $<

lexer.cpp: l.lex
	flex --outfile $@ $<

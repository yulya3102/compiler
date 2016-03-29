all: libparser.so

libparser.so: parser.o lexer.o l.o
	g++ -shared $^ -o $@

%.o: %.cpp
	g++ -fPIC -c -std=c++1y -I.. -o $@ $^

parser.cpp: l.h
l.h: gen_l.h
gen_l.h: l.cpp

l.cpp: l.y++
	bison --language=C++ --defines=gen_l.h --output=$@ $<

lexer.cpp: l.lex
	flex --outfile $@ $<

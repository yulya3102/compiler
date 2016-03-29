parser: ast/l.cpp main.cpp codegen.cpp
	g++ -I. `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++1y -o $@ $^ -L./parse -lparser

parse/parser.so: parse

.PHONY: parse

parse:
	make -C $@

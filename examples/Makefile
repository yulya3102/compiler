all: calls fact fun_defs globals locals while write

%: %.o
	gcc -o=$@ $<

%.o: %.ir
	llc -o=$@ --filetype=obj $<

%.s: %.ir
	llc -o=$@ --filetype=asm $<

LCC ?= ../build/lcc

%.ir: %.lc
	$(LCC) $< 2> $@

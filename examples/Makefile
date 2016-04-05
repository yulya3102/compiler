all: calls fact

%: %.o
	gcc -o $@ $<

%.o: %.ir
	llc -o=$@ --filetype=obj $<

%.s: %.ir
	llc -o=$@ --filetype=asm $<

LCC ?= ../build/lcc

%.ir: %.lc
	$(LCC) $< 2> $@

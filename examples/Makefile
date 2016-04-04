%: %.o
	gcc -o=$@ $<

%.o: %.ir
	llc -o=$@ --filetype=obj $<

LCC ?= ../build/lcc

%.ir: %.lc
	$(LCC) $< 2> $@

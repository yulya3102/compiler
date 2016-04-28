LLC ?= llc

%: %.o
	gcc -o $@ $<

%.o: %.ir
	$(LLC) -o=$@ --filetype=obj $<

%.s: %.ir
	$(LLC) -o=$@ --filetype=asm $<

LCC ?= ../lcc/lcc

%.ir: %.lc
	$(LCC) $< $@

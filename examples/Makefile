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

%.0.ir: %.lc
	$(LCC) -O0 $< $@

%.1.ir: %.lc
	$(LCC) -O1 $< $@

%.2.ir: %.lc
	$(LCC) -O2 $< $@

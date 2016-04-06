examples: scope fact

LLC ?= llc

%: %.o
	gcc -o $@ $<

%.o: %.ir
	$(LLC) -o=$@ --filetype=obj $<

%.s: %.ir
	$(LLC) -o=$@ --filetype=asm $<

LCC ?= ../lcc

%.ir: %.lc
	$(LCC) $< $@

.PHONY: test

test: examples
	./scope.test && echo "OK"
	./fact.test  && echo "OK"

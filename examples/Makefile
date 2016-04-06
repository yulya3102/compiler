examples: scope fact

%: %.o
	gcc -o $@ $<

%.o: %.ir
	llc -o=$@ --filetype=obj $<

%.s: %.ir
	llc -o=$@ --filetype=asm $<

LCC ?= ../lcc

%.ir: %.lc
	$(LCC) $< $@

.PHONY: test

test: examples
	./scope.test && echo "OK"
	./fact.test  && echo "OK"

EXAMPLES = pointer.lc func_pointer.lc pointer_pointer.lc
OBJECTS = $(patsubst %.lc,%.o,$(EXAMPLES))
TRANSLATED = $(patsubst %.lc,%.ir,$(EXAMPLES))
RESULTS = $(patsubst %.lc,%,$(EXAMPLES))

examples: $(RESULTS)

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

.PHONY: test

test: examples
	./pointer.test && echo "OK"
	./func_pointer.test && echo "OK"
	./pointer_pointer.test && echo "OK"

clean:
	rm -f $(OBJECTS) $(TRANSLATED) $(RESULTS)

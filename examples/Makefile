EXAMPLES = scope.lc fact.lc
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
	./scope.test && echo "OK"
	./fact.test  && echo "OK"

clean:
	rm -f $(OBJECTS) $(TRANSLATED) $(RESULTS)

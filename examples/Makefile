%: %.o
	gcc -o=$@ $<

%.o: %.ir
	llc -o=$@ --filetype=obj $<

%.ir: %.lc
	../build/lcc $< 2> $@

run: ints-and-floats 
	./ints-and-floats

ints-and-floats: lex.yy.c
	gcc -o $@ $<

lex.yy.c: ints-and-floats.l
	flex $<

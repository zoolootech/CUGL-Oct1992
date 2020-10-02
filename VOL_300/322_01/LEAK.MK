CFLAGS = -g

maltrace.a:	maltrace.o malloc.o btree.o
	ar rvu maltrace.a maltrace.o malloc.o btree.o
	ranlib maltrace.a

test:	test.o maltrace.a
	cc -g -o test test.o maltrace.a

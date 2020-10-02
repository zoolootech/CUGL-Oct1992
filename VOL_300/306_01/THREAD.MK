# make file for Turbo C++ or Microsoft C

.c.obj :
    cl -c -Gs -DNDEBUG $*.c
#    tcc -c -DNDEBUG $*.c

all : test.exe bench.exe

test.exe : test.obj thread.obj
    cl test.obj thread.obj /link /ST:4096
#    tcc test.obj thread.obj

bench.exe : bench.obj thread.obj
    cl bench.obj thread.obj /link /ST:4096
#    tcc bench.obj thread.obj

test.obj : test.c thread.h

bench.obj : bench.c thread.h

thread.obj : thread.c thread.h


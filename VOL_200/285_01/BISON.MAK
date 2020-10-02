#
#	file:		bison.mak
#	purpose:	Make for bison
#	author:		1989.12.30 - Roberto Artigas Jr
#	environment:	OS/2 1.2 EE, IBM C/2
#
bison.exe: allocate.obj closure.obj conflict.obj
bison.exe: derives.obj files.obj getargs.obj getopt.obj
bison.exe: gram.obj lalr.obj lex.obj lr0.obj
bison.exe: main.obj nullable.obj output.obj
bison.exe: print.obj reader.obj symtab.obj
bison.exe: warshall.obj
bison.exe: bison.lnk
 link @bison.lnk

allocate.obj: allocate.h
allocate.obj: allocate.c
 cl -c -AL -DLINT_ARGS -W3 -G2 allocate.c

closure.obj: machine.h new.h gram.h closure.h warshall.h
closure.obj: closure.c
 cl -c -AL -DLINT_ARGS -W3 -G2 closure.c

conflict.obj: machine.h new.h files.h gram.h state.h conflict.h
conflict.obj: conflict.c
 cl -c -AL -DLINT_ARGS -W3 -G2 conflict.c

derives.obj: new.h types.h gram.h derives.h
derives.obj: derives.c
 cl -c -AL -DLINT_ARGS -W3 -G2 derives.c

files.obj: files.h new.h gram.h
files.obj: files.c
 cl -c -AL -DLINT_ARGS -W3 -G2 files.c

getargs.obj: files.h bison.h getopt.h getargs.h
getargs.obj: getargs.c
 cl -c -AL -DLINT_ARGS -W3 -G2 getargs.c

getopt.obj: getopt.h
getopt.obj: getopt.c
 cl -c -AL -DLINT_ARGS -W3 -G2 getopt.c

gram.obj: gram.c
 cl -c -AL -DLINT_ARGS -W3 -G2 gram.c

lalr.obj: machine.h types.h state.h new.h gram.h lalr.h bison.h
lalr.obj: lalr.c
 cl -c -AL -DLINT_ARGS -W3 -G2 lalr.c

lex.obj: files.h symtab.h lex.h bison.h
lex.obj: lex.c
 cl -c -AL -DLINT_ARGS -W3 -G2 lex.c

lr0.obj: machine.h new.h gram.h state.h bison.h closure.h lr0.h
lr0.obj: lr0.c
 cl -c -AL -DLINT_ARGS -W3 -G2 lr0.c

main.obj: machine.h getargs.h files.h reader.h nullable.h lalr.h
main.obj: lr0.h print.h conflict.h derives.h output.h bison.h
main.obj: main.c
 cl -c -AL -DLINT_ARGS -W3 -G2 main.c

nullable.obj: types.h gram.h new.h nullable.h
nullable.obj: nullable.c
 cl -c -AL -DLINT_ARGS -W3 -G2 nullable.c

output.obj: machine.h new.h files.h gram.h
output.obj: state.h bison.h output.h bison.h
output.obj: output.c
 cl -c -AL -DLINT_ARGS -W3 -G2 output.c

print.obj: machine.h new.h files.h gram.h state.h
print.obj: conflict.h print.h
print.obj: print.c
 cl -c -AL -DLINT_ARGS -W3 -G2 print.c

reader.obj: files.h new.h symtab.h lex.h gram.h output.h
reader.obj: reader.h bison.h
reader.obj: reader.c
 cl -c -AL -DLINT_ARGS -W3 -G2 reader.c

symtab.obj: new.h gram.h symtab.h
symtab.obj: symtab.c
 cl -c -AL -DLINT_ARGS -W3 -G2 symtab.c

warshall.obj: machine.h warshall.h
warshall.obj: warshall.c
 cl -c -AL -DLINT_ARGS -W3 -G2 warshall.c

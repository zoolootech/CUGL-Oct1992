#
#	file:		flex.mak
#	purpose:	Make for flex
#	environment:	OS/2 1.2 EE, IBM C/2
#
flex.exe: parse.obj scan.obj flex.lnk
flex.exe: ccl.obj dfa.obj ecs.obj
flex.exe: main.obj misc.obj nfa.obj
flex.exe: sym.obj tblcmp.obj yylex.obj
 link @flex.lnk

ccl.obj: flexdef.h ccl.h misc.h
ccl.obj: ccl.c
 cl -c -AL -DLINT_ARGS -W3 -G2 ccl.c

dfa.obj: flexdef.h dfa.h misc.h ecs.h
dfa.obj: dfa.c
 cl -c -AL -DLINT_ARGS -W3 -G2 dfa.c

ecs.obj: flexdef.h ecs.h
ecs.obj: ecs.c
 cl -c -AL -DLINT_ARGS -W3 -G2 ecs.c

main.obj: flexdef.h main.h misc.h nfa.h tblcmp.h ecs.h
main.obj: main.c
 cl -c -AL -DLINT_ARGS -W3 -G2 main.c

misc.obj: flexdef.h main.h misc.h main.h parse.h
misc.obj: misc.c
 cl -c -AL -DLINT_ARGS -W3 -G2 misc.c

nfa.obj: flexdef.h nfa.h misc.h ecs.h
nfa.obj: nfa.c
 cl -c -AL -DLINT_ARGS -W3 -G2 nfa.c

parse.obj: flexdef.h ecs.h nfa.h sym.h ccl.h misc.h
parse.obj: parse.c
 cl -c -AL -DLINT_ARGS -W3 -G2 parse.c

scan.obj: skeleton.h misc.h main.h sym.h flexdef.h parse.h sym.h
scan.obj: scan.c
 cl -c -AL -DLINT_ARGS -W3 -G2 scan.c

sym.obj: flexdef.h sym.h parse.h misc.h nfa.h
sym.obj: sym.c
 cl -c -AL -DLINT_ARGS -W3 -G2 sym.c

tblcmp.obj: flexdef.h main.h misc.h tblcmp.h dfa.h nfa.h ecs.h
tblcmp.obj: tblcmp.c
 cl -c -AL -DLINT_ARGS -W3 -G2 tblcmp.c

yylex.obj: flexdef.h parse.h
yylex.obj: yylex.c
 cl -c -AL -DLINT_ARGS -W3 -G2 yylex.c

#
#	1989.12.31 - Roberto Artigas Jr
#	-------------------------------
#	P.O. Box 281415
#	Memphis, TN 38168-1415
#	home: 901 - 373 - 4738
#	work: 910 - 762 - 6092
#	-------------------------------
#	The fact that neither of 'parse.y' or 'scan.l' had
#	been run through their respective translations
#	for a while has been corrected. HOWEVER, If you are
#	tempted to fully regenerate all of FLEX, make sure
#	that you HAVE a fully functional set of backups
#	somewhere. If you do maintenance on either of these
#	files and YOU mess it up (WITHOUT a BACKUP) you
#	may put yourself in a bind. I know most of the
#	individuals out in CUG land are carefull. But even
#	GOD (According to GEORGE BURNS) has made mistakes.
#
#	FOR NOW - As a measure of SAFETY for all those
#	wonderfull budding compiler builders (Including
#	myself) these steps are COMMENTED out.
#
#	The ONLY reason I attempted to DO this in the first
#	PLACE is because I needed an OS/2 version of FLEX.
#

# 
#	BISON/YACC must generate 'parse.c'
#
#parse.c: parse.y
# bison -ld parse.y -o parse.c

#
#	FLEX/LEX must generate 'scan.c'
#
#scan.c: scan.l
# flex -sL scan.l
# rename lexyy.c scan.c

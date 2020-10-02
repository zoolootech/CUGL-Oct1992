: SAMPLE OUTLINE FOR COMPILATION "SCRIPT", "BATCH FILE", ETC.
: Makx pgm  --  compile file(s) to produce  pgm.x
:							run pgm.x <pgm.in >pgm.out
:							cmp pgm.out pgm.tst
:							rm pgm.o pgm.x
: Makxnt -- same compile, but no test file
: Maktry pgm file(s).c -- compile file(s) with #define TRYMAIN into pgm
:							(same run, cmp, rm)
: Mako file.c  --  compile-only, into object code

command /c Makx accnt
command /c Makx arith
command /c Makx asst
command /c Makx asst2
command /c Makx badpow
command /c Makx bdrill
command /c Makxnt bits
command /c Makx blast
command /c Makx blast2
command /c Makx cap
command /c Makx cap2
command /c Makx codes1
command /c Makx codes2
command /c Makx codes3
command /c Makx codes4
command /c Makxnt copy
command /c Makx copy2
command /c Makx dmpdem
command /c Makx fact
command /c Maktry factl factl.c
command /c Makx fast
command /c Makx getbn
command /c Makx getbn2
command /c Makx gettt
command /c Makx guess
command /c Makx guess2
command /c Makx hello
command /c Makx hello2
command /c Makx hello3
command /c Makx hello4
command /c Makx hello5
command /c Makx inits
command /c Makx loadtt
command /c Makx maxmin
command /c Makx mortg
command /c Makx mortg2
command /c Mako nfrom
command /c Makx powdem
command /c Makx pr2a
command /c Makx pr2b
command /c Makx prsam
command /c Mako rand
command /c Makx recpt1
command /c Makx recpt2
command /c Makxm recpt3 receip ++ recpt3 +receip
command /c Makx revers
command /c Makxm shuf52 nfrom ++ shuf52 +nfrom
command /c Makx sizes
command /c Makx slow
command /c Makxnt string
command /c Mako strlen
command /c Mako strscn
command /c Makxm tower pegs ++ tower +pegs
command /c Makx vacat

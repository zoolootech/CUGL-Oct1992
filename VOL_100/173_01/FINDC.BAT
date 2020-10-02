ECHO OFF
FOR %%g IN ( *.C ) DO fgrep %1 %%g
fgrep %1 base.c
fgrep %1 commen.c
fgrep %1 dfa.c
fgrep %1 eclosu.c
fgrep %1 gettok.c
fgrep %1 impure.c
fgrep %1 integ.c
fgrep %1 lex.c
fgrep %1 lexcha.c
fgrep %1 lexech.c
fgrep %1 lexerr.c
fgrep %1 lexget.c
fgrep %1 lexlen.c
fgrep %1 lexpee.c
fgrep %1 lexsrt.c
fgrep %1 lexswi.c
fgrep %1 llsave.c
fgrep %1 lmovb.c
fgrep %1 lmovi.c
fgrep %1 mapch.c
fgrep %1 min.c
fgrep %1 out1.c
fgrep %1 out2.c
fgrep %1 token.c
fgrep %1 ytab.c
fgrep %1 yylex.c

.c
fgrep %1 mapch.c
fgrep %1 min.c
fgrep %1 out1.c
fgrep %1 out2.c
fgrep %1 token.c
fgrep %1 ytab.c
fgrep %1 yyle
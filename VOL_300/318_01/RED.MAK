# Turbo C make file for red.exe
#
# SMALL memory model
#
# January 10, 1990.
#

red.exe: redglb.obj \
redmain.obj redcmnd.obj reded.obj redfmt.obj redout.obj redpmt.obj \
redsys.obj redutl.obj redbuf1.obj redbuf2.obj redbuf3.obj redbuf4.obj \
redoutan.obj
  tlink @red.lnk

redglb.obj: ..\redglb.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redglb.c

redmain.obj: ..\redmain.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redmain.c

redcmnd.obj: ..\redcmnd.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redcmnd.c

reded.obj: ..\reded.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\reded.c

redfmt.obj: ..\redfmt.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redfmt.c

redout.obj: ..\redout.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redout.c

redpmt.obj: ..\redpmt.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redpmt.c

redsys.obj: ..\redsys.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redsys.c

redutl.obj: ..\redutl.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redutl.c

redbuf1.obj: ..\redbuf1.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redbuf1.c

redbuf2.obj: ..\redbuf2.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redbuf2.c

redbuf3.obj: ..\redbuf3.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redbuf3.c

redbuf4.obj: ..\redbuf4.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC ..\redbuf4.c

redoutan.obj: ..\redouta.asm
  masm /MX /ML /DNEAR ..\redouta.asm,redoutan.obj,nul.lst,nul.cod;

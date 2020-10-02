# Turbo C make file for red.exe
#
# small memory model
#
# January 11, 1989
#

reddb.exe: redglbdb.obj \
redmaidb.obj redcmndb.obj rededdb.obj redfmtdb.obj redoutdb.obj redpmtdb.obj \
redoutan.obj \
redsysdb.obj redutldb.obj redb1db.obj redb2db.obj redb3db.obj redb4db.obj \
sls.obj
  tlink @reddb.lnk

sls.obj: ..\sherlock.c
  tcc -ms -c -C -osls.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DUSER_OUT -DTURBOC ..\sherlock.c

redglbdb.obj: ..\redglb.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredglbdb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redglb.c

redmaidb.obj: ..\redmain.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredmaidb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redmain.c

redcmndb.obj: ..\redcmnd.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredcmndb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redcmnd.c

rededdb.obj: ..\reded.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -orededdb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\reded.c

redfmtdb.obj: ..\redfmt.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredfmtdb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\redfmt.c

redoutdb.obj: ..\redout.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredoutdb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redout.c

redpmtdb.obj: ..\redpmt.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredpmtdb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\redpmt.c

redsysdb.obj: ..\redsys.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredsysdb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redsys.c

redutldb.obj: ..\redutl.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredutldb.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redutl.c

redb1db.obj: ..\redbuf1.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredb1db.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redbuf1.c

redb2db.obj: ..\redbuf2.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredb2db.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redbuf2.c

redb3db.obj: ..\redbuf3.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredb3db.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redbuf3.c

redb4db.obj: ..\redbuf4.c ..\red.h ..\redtmp.h ..\red1.h ..\redbuf.h
  tcc -ms -c -C -oredb4db.obj -Ic:\include\turboc -Id:..\ -Id:\sherlock -DTURBOC -DSHERLOCK ..\redbuf4.c

redoutan.obj: ..\redouta.asm
  masm /MX /ML /DNEAR ..\redouta.asm,redoutan.obj,nul.lst,nul.cod;

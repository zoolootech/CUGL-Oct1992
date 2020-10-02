# Make red.exe using Microsoft C version 5.00 or later
#
# reddb.mmk
# January 18, 1990
#
# /AS SMALL memory model
# /Gs no stack probes
# /Gt0 separate data areas
# /Od  no optimization
#

redmandb.obj: ..\redmain.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredmandb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redmain.c

redglbdb.obj: ..\redglb.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredglbdb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redglb.c

redcmndb.obj: ..\redcmnd.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredcmndb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redcmnd.c

rededdb.obj: ..\reded.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Forededdb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\reded.c

redfmtdb.obj: ..\redfmt.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Fofmtdb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redfmt.c

redoutdb.obj: ..\redout.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Fooutdb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redout.c

redpmtdb.obj: ..\redpmt.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Fopmtdb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redpmt.c

redsysdb.obj:  ..\redsys.c  ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Fosysdb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redsys.c

redutldb.obj: ..\redutl.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredutldb.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redutl.c

redb1db.obj: ..\redbuf1.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredb1db.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf1.c

redb2db.obj: ..\redbuf2.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredb2db.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf2.c

redb3db.obj: ..\redbuf3.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredb3db.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf3.c

redb4db.obj: ..\redbuf4.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /Foredb4db.obj /DSHERLOCK /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf4.c

sherlock.obj: ..\sherlock.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DUSER_OUT /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\sherlock.c

redoutan.obj: ..\redouta.asm
  masm /MX /ML /DNEAR ..\redouta.asm,redoutan.obj,nul.lst,nul.cod;

mreddb.exe: redmandb.obj redglbdb.obj redcmndb.obj rededdb.obj redfmtdb.obj \
redoutdb.obj redpmtdb.obj redsysdb.obj redutldb.obj \
redb1db.obj redb2db.obj redb3db.obj redb4db.obj \
redoutan.obj sherlock.obj
  link @mreddb.ml
  exemod mred.exe -stack 3000

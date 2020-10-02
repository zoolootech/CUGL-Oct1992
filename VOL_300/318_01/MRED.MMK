# Make red.exe using Microsoft C version 5.00 or later
#
# red.mmk
# January 18, 1990
#
# /AS SMALL memory model
# /Gs no stack probes
# /Gt0 separate data areas
# /Od  no optimization
#

redmain.obj: ..\redmain.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redmain.c

redglb.obj: ..\redglb.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redglb.c

redcmnd.obj: ..\redcmnd.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redcmnd.c

reded.obj: ..\reded.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\reded.c

redfmt.obj: ..\redfmt.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redfmt.c

redout.obj: ..\redout.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redout.c

redpmt.obj: ..\redpmt.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redpmt.c

redsys.obj:  ..\redsys.c  ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redsys.c

redutl.obj: ..\redutl.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redutl.c

redbuf1.obj: ..\redbuf1.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf1.c

redbuf2.obj: ..\redbuf2.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf2.c

redbuf3.obj: ..\redbuf3.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf3.c

redbuf4.obj: ..\redbuf4.c ..\red.h ..\red1.h ..\redtmp.h ..\redbuf.h
  cl /c /AS /DMICRO_SOFT /Ic:\include\msc /Id:\sherlock ..\redbuf4.c

redoutan.obj: ..\redouta.asm
  masm /MX /ML /DNEAR ..\redouta.asm,redoutan.obj,nul.lst,nul.cod;

mred.exe: redmain.obj redglb.obj redcmnd.obj reded.obj redfmt.obj \
redout.obj redpmt.obj redsys.obj redutl.obj \
redbuf1.obj redbuf2.obj redbuf3.obj redbuf4.obj \
redoutan.obj
  link @mred.ml
  exemod mred.exe -stack 3000

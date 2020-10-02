# Make cpp.exe using Microsoft C version 5.xx.
# February 14, 1989
#
# /Od disable optimization
#
# /AT TINY memory model.
# /AS SMALL memory model.
# /AM MEDIUM memory model.
# /AC COMPACT memory model.
# /AL LARGE memory model.
# /AH HUGE memory model.
#
# cpp.exe also depends on slxxx.obj and prfxxx.obj (see cpp.lnk)

cpp.obj: ..\cpp.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\cpp.c

glb.obj: ..\glb.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\glb.c

tok.obj: ..\tok.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\tok.c

dir.obj: ..\dir.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\dir.c

def.obj: ..\def.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\def.c

mst.obj: ..\mst.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\mst.c

mem.obj: ..\mem.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\mem.c

sys.obj: ..\sys.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\sys.c

str.obj: ..\str.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\str.c

utl.obj: ..\utl.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\utl.c

pr.obj: ..\pr.c  ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DNO_SHERLOCK /Ic:\include\msc /Id:\sherlock ..\pr.c

mcpp.exe: cpp.obj glb.obj tok.obj dir.obj def.obj \
mst.obj sys.obj mem.obj utl.obj str.obj pr.obj
  link @cpp.ml
  exemod cpp.exe /STACK 8000
  del mcpp.exe
  ren cpp.exe mcpp.exe
  copy mcpp.exe d:\sherlock\bin

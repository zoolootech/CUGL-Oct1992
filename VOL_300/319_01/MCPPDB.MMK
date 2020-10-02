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

cppdb.obj: ..\cpp.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Focppdb.obj ..\cpp.c

glbdb.obj: ..\glb.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Foglbdb.obj ..\glb.c

tokdb.obj: ..\tok.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fotokdb.obj ..\tok.c

dirdb.obj: ..\dir.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fodirdb.obj ..\dir.c

defdb.obj: ..\def.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fodefdb.obj ..\def.c

mstdb.obj: ..\mst.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fomstdb.obj ..\mst.c

memdb.obj: ..\mem.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fomemdb.obj ..\mem.c

sysdb.obj: ..\sys.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fosysdb.obj ..\sys.c

strdb.obj: ..\str.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Fostrdb.obj ..\str.c

utldb.obj: ..\utl.c ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Foutldb.obj ..\utl.c

prdb.obj: ..\pr.c  ..\cpp.h 
  cl /c /AL /Gt0 /DMICRO_SOFT /DSHERLOCK /Ic:\include\msc /Id:\sherlock /Foprdb.obj ..\pr.c

mcppdb.exe: cppdb.obj glbdb.obj tokdb.obj dirdb.obj defdb.obj \
mstdb.obj sysdb.obj memdb.obj utldb.obj strdb.obj prdb.obj
  link @cppdb.ml
  exemod cppdb.exe /STACK 8000
  del mcppdb.exe
  ren cppdb.exe mcppdb.exe
  copy mcppdb.exe d:\sherlock\bin

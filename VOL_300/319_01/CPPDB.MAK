# Turbo C make file for cpp.exe
#
# -mc: COMPACT memory model
# -c:  compile to .obj
# -C:  Allow nested comments
# -N:  Check for stack overflow
#
# March 14, 1988
#
# cpp.exe also depends on slxxx.obj and prfxxx.obj (see cpp.lnk)

cppdb.exe: cppdb.obj glbdb.obj tokdb.obj dirdb.obj defdb.obj \
mstdb.obj sysdb.obj memdb.obj utldb.obj strdb.obj prdb.obj
	tlink @cppdb.lnk
	tlink @cppdbx.lnk

cppdb.obj: ..\cpp.c ..\cpp.h ..\enum.h
  tcc -ocppdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\cpp.c

glbdb.obj: ..\glb.c ..\cpp.h ..\enum.h
  tcc -oglbdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\glb.c

tokdb.obj: ..\tok.c ..\cpp.h ..\enum.h
  tcc -otokdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\tok.c

dirdb.obj: ..\dir.c ..\cpp.h ..\enum.h
  tcc -odirdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\dir.c

defdb.obj: ..\def.c ..\cpp.h ..\enum.h
  tcc -odefdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\def.c

mstdb.obj: ..\mst.c ..\cpp.h ..\enum.h
  tcc -omstdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\mst.c

memdb.obj: ..\mem.c ..\cpp.h ..\enum.h
  tcc -omemdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\mem.c

sysdb.obj: ..\sys.c ..\cpp.h ..\enum.h
  tcc -osysdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\sys.c

strdb.obj: ..\str.c ..\cpp.h ..\enum.h
  tcc -ostrdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\str.c

utldb.obj: ..\utl.c ..\cpp.h ..\enum.h
  tcc -outldb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\utl.c

prdb.obj: ..\pr.c  ..\cpp.h ..\enum.h
  tcc -oprdb.obj -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DSHERLOCK ..\pr.c

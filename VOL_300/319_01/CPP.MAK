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

cpp.exe: cpp.obj glb.obj tok.obj dir.obj def.obj \
mst.obj sys.obj mem.obj utl.obj str.obj pr.obj
	tlink @cpp.lnk

cpp.obj: ..\cpp.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\cpp.c

glb.obj: ..\glb.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\glb.c

tok.obj: ..\tok.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\tok.c

dir.obj: ..\dir.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\dir.c

def.obj: ..\def.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\def.c

mst.obj: ..\mst.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\mst.c

mem.obj: ..\mem.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\mem.c

sys.obj: ..\sys.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\sys.c

str.obj: ..\str.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\str.c

utl.obj: ..\utl.c ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\utl.c

pr.obj: ..\pr.c  ..\cpp.h ..\enum.h
  tcc -mc -c -C -Ic:\include\turboc -Id:\cpp -Id:\sherlock -DTURBOC -DNO_SHERLOCK ..\pr.c

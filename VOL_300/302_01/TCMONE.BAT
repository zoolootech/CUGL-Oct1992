echo off
REM Batch file to compile 3D TRANSFORMS and add to library
REM for just one memory model.
REM
REM Usage is:
REM
REM         tcmone [t,s,c,m,l,h]
REM
REM where the single letter parameter specifies the memory model as
REM described below.
REM
REM Compile options are:
REM  -a-           byte alignment
REM  -C            nested comments on
REM  -c            compile to OBJ (no link)
REM  -f            floating point emulation
REM  -I..\include  include files path
REM  -K-           default char type unsigned
REM  -L..\lib      library files path
REM  -G            optimize for speed
REM  -m?           memory model   ? = t for TINY
REM                               ? = s for SMALL
REM                               ? = c for COMPACT
REM                               ? = m for MEDIUM
REM                               ? = l for LARGE
REM                               ? = h for HUGE
REM  -p-           C calling convention
REM  -r            user register variables
REM  -w            display warnings
REM
REM  Memory model model is defined as above by parameter %1.
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%1 -p- -r -w dumpo.c
tlib 3d%1 +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3d%1 +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3d%1 +-delf +-af +-no +-dumpv +-dumpf +-dumpo

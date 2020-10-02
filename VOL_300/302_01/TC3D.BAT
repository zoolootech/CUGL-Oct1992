echo off
REM Batch file to compile 3D TRANSFORMS and create libraries
REM for each memory model.
REM
REM Usage is:
REM
REM         tc3d
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
copy 3d.h ..\..\include
REM
REM  TINY model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w dumpo.c
tlib 3dt +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3dt +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3dt +-delf +-af +-no +-dumpv +-dumpf +-dumpo
copy 3dt.lib ..\..\lib
REM
REM  SMALL model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w dumpo.c
tlib 3ds +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3ds +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3ds +-delf +-af +-no +-dumpv +-dumpf +-dumpo
copy 3ds.lib ..\..\lib
REM
REM  MEDIUM model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w dumpo.c
tlib 3dm +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3dm +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3dm +-delf +-af +-no +-dumpv +-dumpf +-dumpo
copy 3dm.lib ..\..\lib
REM
REM  COMPACT model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w dumpo.c
tlib 3dc +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3dc +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3dc +-delf +-af +-no +-dumpv +-dumpf +-dumpo
copy 3dc.lib ..\..\lib
REM
REM  LARGE model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w dumpo.c
tlib 3dl +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3dl +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3dl +-delf +-af +-no +-dumpv +-dumpf +-dumpo
copy 3dl.lib ..\..\lib
REM
REM  HUGE model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w id.c dp.c mm.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w norm.c vm.c scale.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w trans.c xrot.c yrot.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w zrot.c dumpm.c persp.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w df.c do.c ac.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w nf.c xf.c delf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w mxz.c mnz.c af.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w no.c dumpv.c dumpf.c
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w dumpo.c
tlib 3dh +-id +-dp +-mm +-norm +-vm +-scale +-trans +-xrot +-yrot
tlib 3dh +-zrot +-dumpm +-df +-do +-ac +-nf +-xf +-mxz +-mnz
tlib 3dh +-delf +-af +-no +-dumpv +-dumpf +-dumpo
copy 3dh.lib ..\..\lib

echo off
REM Batch file to compile one source file and add to libraries
REM for each memory model.
REM
REM Usage is:
REM
REM         tcone <file>
REM
REM where <file> is the source file, no extension (.c assumed).
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
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mt -p- -r -w %1
tlib 3dt +-%1
REM
REM  SMALL model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ms -p- -r -w %1
tlib 3ds +-%1
REM
REM  MEDIUM model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mm -p- -r -w %1
tlib 3dm +-%1
REM
REM  COMPACT model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mc -p- -r -w %1
tlib 3dc +-%1
REM
REM  LARGE model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -ml -p- -r -w %1
tlib 3dl +-%1
REM
REM  HUGE model
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -mh -p- -r -w %1
tlib 3dh +-%1

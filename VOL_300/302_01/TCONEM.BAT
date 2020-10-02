echo off
REM Batch file to compile one source file and add to libraries
REM for just one memory model.
REM
REM Usage is:
REM
REM         tconem <file> [t,s,c,m,l,h]
REM
REM where <file> is the source file, no extension (.c assumed), and
REM the single letter parameter specifies the memory model as described below.
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
REM  The memory model is defined by parameter %2, as defined above.
REM  The file to be compiled is %1, with no extension.
REM
tcc -a- -C -c -f -I..\..\include -K -L..\..\lib -G -m%2 -p- -r -w %1
tlib 3d%2 +-%1

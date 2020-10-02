@echo off
rem blkio installation batch file-----------------------------------------------
rem #ident	"install.bat	1.3 - 89/10/31"

rem ----------------------------------------------------------------------------
rem This batch file is provided for installation on MS-DOS systems.
rem It is written for use with Borland Turbo C.  To convert it for use
rem with another compiler, make the following modifications:
rem      1. Replace \tc\include with the include directory used by the
rem         new compiler.
rem      2. Replace \tc\lib with the library directory used by the new
rem         compiler.
rem      3. Replace tcc with the command to invoke the compiler being
rem         used, replacing the switches also, if necessary.  -O is
rem         the optimize option and -c means compile but don't link.
rem         -C- disables nested comments.  -m_ specifies the desired
rem         memory model.
rem      4. The command to build the library archive from the object
rem         modules may vary more drastically.  Turbo C used the tlib
rem         command with what Borland refers to as a response file.
rem         This response file, blkio.rsp, contains a list of all the
rem         object modules in the library.
rem ----------------------------------------------------------------------------

rem install boolean header file-------------------------------------------------
rem
if not exist \tc\include\bool.h goto bool
echo \tc\include\bool.h exists.  ^C to exit, any other key to continue.
pause
:bool
echo on
copy bool.h \tc\include\bool.h
@echo off

rem extract the reference manual------------------------------------------------
if not exist blkio.man goto man
echo blkio.man exists.  ^C to exit, any other key to continue.
pause
:man
if not exist tmp goto tmp
echo tmp exists.  ^C to exit, any other key to continue.
pause
:tmp
echo on
copy blkio.h/a+bclose.c+bexit.c+bflpop.c+bflpush.c+bflush.c+bgetb.c tmp
type tmp | manx -c > blkio.man
copy bgetbf.c/a+bgeth.c+bgethf.c+bopen.c+bputb.c+bputbf.c tmp
type tmp | manx -c >> blkio.man
copy bputh.c/a+bputhf.c+bsetbuf.c+bsetvbuf.c+bsync.c+lockb.c tmp
type tmp | manx -c >> blkio.man
del tmp
@echo off

rem compile all blkio source files----------------------------------------------
echo on
tcc -O -c -C- -mh bclose.c   bexit.c    bflpop.c   bflpush.c  bflush.c   bgetb.c
tcc -O -c -C- -mh bgetbf.c   bgeth.c    bgethf.c   bopen.c    bputb.c    bputbf.c
tcc -O -c -C- -mh bputh.c    bputhf.c   bsetbuf.c  bsetvbuf.c bsync.c    lockb.c
tcc -O -c -C- -mh bops.c     buops.c
@echo off

rem build the blkio library archive---------------------------------------------
echo on
tlib blkio @blkio.rsp
@echo off

rem install the blkio library---------------------------------------------------
if not exist \tc\include\blkio.h goto h
echo \tc\include\blkio.h exists.  ^C to exit, any other key to continue.
pause
:h
echo on
copy blkio.h \tc\include\blkio.h
@echo off

if not exist \tc\lib\blkio.lib goto lib
echo \tc\lib\blkio.lib exists.  ^C to exit, any other key to continue.
pause
:lib
echo on
copy blkio.lib \tc\lib\blkio.lib
@echo off

rem end of blkio installation batch file----------------------------------------
:end

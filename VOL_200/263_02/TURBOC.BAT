REM
REM  This batch file issues the shareware TurboC libraries from disk #1.
REM  It makes certain assumptions about diskette drives and the directories
REM  used for your TurboC installation.  If these are wrong, you will need
REM  to edit this file before installation can begin.
REM
REM  The assumptions are:
REM  a) The diskette is ....................  A:
REM  b) The TurboC main directory is .......  \TC
REM  c) The libraries go into ..............  \TC\LIB
REM  b) The header files go into ...........  \TC\INCLUDE
REM

IF NOT EXIST \TC\INCLUDE\stdio.h GOTO ERREND

COPY  A:*.h         \TC\INCLUDE /v
COPY  A:mc_wndw.tbc \TC\LIB\mc_wndw.lib /v
COPY  A:mc_ndx.tbc  \TC\LIB\mc_ndx.lib /v
COPY  A:compile.tbc \TC\compile.bat /v
MKDIR \TC\marietta

GOTO END

:ERREND
REM  Your directories are not setup the way we assumed
REM  Edit the TURBOC.BAT file.

:END

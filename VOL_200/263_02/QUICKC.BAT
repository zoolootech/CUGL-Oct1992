REM
REM  This batch file issues the shareware QuickC libraries from disk #2.
REM  It makes certain assumptions about diskette drives and the directories
REM  used for your QuickC or Microsoft C installation.  If these are wrong, 
REM  you will need to edit this file before installation can begin.
REM
REM  The assumptions are:
REM  a) The diskette is .........................  A:
REM  b) The QuickC main directory is ............  \QC2
REM  c) The libraries go into ...................  \QC2\LIB
REM  b) The header files go into ................  \QC2\INCLUDE
REM

IF NOT EXIST \QC2\INCLUDE\stdio.h GOTO ERREND

COPY  A:*.h          \QC2\INCLUDE /v
COPY  A:mc_wndw.msc  \QC2\LIB\mc_wndw.lib /v
COPY  A:mc_ndx.msc   \QC2\LIB\mc_ndx.lib /v
COPY  A:compile.msc  \QC2\compile.bat /v
MKDIR \QC2\marietta

GOTO END

:ERREND
REM  Your directories are not setup the way we assumed
REM  Edit the QUICKC.BAT file.
:END

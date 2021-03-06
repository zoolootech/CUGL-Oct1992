echo off
goto start
rem
rem                             INSTALL.BAT
rem    Batch file to copy relevant files to working directory and possibly
rem                            run an example
rem
rem                     Author: M. A. Pollatschek
rem
rem                         IMPORTANT NOTE
rem
rem THIS PROCEDURE,  ITS USE,  OPERATION  AND SUPPORT  IS PROVIDED  "AS  IS"
rem WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,BUT
rem NOT LIMITED TO,  THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND  FITNESS
rem FOR A  PARTICULAR  PURPOSE.   THE  ENTIRE RISK  AS  TO THE  QUALITY  AND
rem PERFORMANCE OF  THIS  PROGRAM IS  WITH  THE USER.    IN NO  EVENT  SHALL
rem THE AUTHOR AND/OR THE PUBLISHER  BE  LIABLE  FOR ANY  DAMAGES INCLUDING,
rem WITHOUT LIMITATION,  ANY  LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL
rem OR CONSEQUENTIAL  DAMAGES  ARISING  THE  USE  OR  INABILITY  TO USE SUCH
rem PROGRAM,  EVEN  IF  THE AUTHOR AND/OR THE PUBLISHER  BEEN ADVISED OF THE
rem POSSIBILITY  OF SUCH  DAMAGES  OR  FOR  ANY  CLAIM  BY  ANY OTHER PARTY.
:usage
echo Usage is: A:INSTALL language
echo �
echo while current directory is the target directory which should hold the files
echo       (typically the directory of the compiler) and
echo �
echo where language is one of BC, C, F, QB, QC, QCL, QP, TC, TP whose meaning:
echo �
echo        ������������������������������������������������������������ͻ
echo        �For running under:�   From Microsoft:  �   From Borland:    �
echo        ������������������������������������������������������������͹
echo        �                  � QuickBasic  [ QB  ]�                    �
echo        �   integrated     � QuickC      [ QC  ]� TurboC      [ TC  ]�
echo        �   environment    � QuickPascal [ QP  ]� TurboPascal [ TP  ]�
echo        ������������������������������������������������������������͹
echo        �                  � QuickBasic  [ BC  ]�                    �
echo        � DOS (as .EXE)    � C           [ C   ]�                    �
echo        �                  � QuickC      [ QCL ]� TurboC      [ TC  ]�
echo        �                  � Fortran     [ F   ]�                    �
echo        ������������������������������������������������������������ͼ
goto end
:start
if "%1" == "" goto usage
if %1 == TP goto turbop
if %1 == QP goto quickp
if %1 == TC goto turboc
if %1 == BC goto msb
if %1 == QCL goto msqc
if %1 == F goto msf
if %1 == QB goto msqb
if %1 == QC goto msqqcc
if %1 == C goto c1
echo INSTALL: argument is wrong or not uppercase (must be UPPERCASE!)
echo �
goto usage
:turbop
A:PKXARC -e A:SSS *.SSS EX_*.PAS SSSQTP.H
A:PKXARC -e A:LIB SSS.TPU
echo Now you may run some EX_*.PAS simulation programs.
echo �
echo Try to load and run from TurboPascal!
echo �
echo I am going to load now TurboPascal and run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
TURBO EX_0505.PAS
goto end
:quickp
A:PKXARC -e A:SSS *.SSS EX_*.PAS SSSQTP.H
A:PKXARC -e A:LIB SSS.QPU
echo Now you may run some EX_*.PAS simulation programs.
echo �
echo Try to load and run from QuickPascal!
echo �
echo I am going to load now QuickPascal and run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
QP EX_0505.PAS
goto end
:turboc
A:PKXARC -e A:SSS *.SSS EX_*.C SSSC.H SSSKRC.H DOT_IN.EXE SSSPRJ.BAT
A:PKXARC -e A:LIB SSST.LIB
echo Now you may run some EX_*.C simulation programs.
echo �
echo Try to load and run from TurboC, having prepared a .PRJ file.
echo �
echo I am going to prepare now a .PRJ file.
echo You can stop here by pressing Control-C.
pause
echo off
SSSPRJ EX_0505
goto end
:msb
A:PKXARC -e A:SSS *.SSS EX_*.BAS SSSB.H
A:PKXARC -e A:LIB SSSM.LIB
A:PKXARC -e A:SSS SSSGO.BAT CHECKING.EXE DOT_IN.EXE
echo Now you may run some EX_*.BAS simulation programs.
echo �
echo I am going to run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
SSSGO B EX_0505
goto end
:msf
A:PKXARC -e A:SSS *.SSS EX_*.FOR SSSF?.H
A:PKXARC -e A:LIB SSSM.LIB
A:PKXARC -e A:SSS SSSGO.BAT CHECKING.EXE DOT_IN.EXE
echo Now you may run some EX_*.FOR simulation programs.
echo �
echo I am going to run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
SSSGO F EX_0505
goto end
:msqc
A:PKXARC -e A:SSS *.SSS EX_*.C SSSC.H SSSKRC.H
A:PKXARC -e A:LIB SSSM.LIB
A:PKXARC -e A:SSS SSSGO.BAT CHECKING.EXE DOT_IN.EXE
echo Now you may run some EX_*.C simulation programs.
echo �
echo I am going to run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
SSSGO QC EX_0505
goto end
:msqb
A:PKXARC -e A:SSS *.SSS EX_*.BAS SSSB.H
A:PKXARC -e A:LIB SSSB.QLB
echo Now you may run some EX_*.BAS simulation programs.
echo �
echo To simulate without leaving the programming environment load QB so:
echo QB /lSSSB.QLB
echo �
echo I am going to run an example program.
echo You can stop here by pressing Control-C.
pause
echo on
QB /lSSSB.QLB EX_0505.BAS
goto end
:msqqcc
A:PKXARC -e A:SSS *.SSS EX_*.C SSSC.H
A:PKXARC -e A:LIB SSSC.QLB
echo Now you may run some EX_*.C simulation programs.
echo �
echo To simulate from inside the programming environment load QC so:
echo QC /lSSSC.QLB
echo �
echo I am going to run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
QC /lSSSC.QLB EX_0505.C
goto end
:c1
A:PKXARC -e A:SSS *.SSS EX_*.C SSSC.H SSSKRC.H
A:PKXARC -e A:LIB SSSM.LIB
A:PKXARC -e A:SSS SSSGO.BAT CHECKING.EXE DOT_IN.EXE
echo Now you may run some EX_*.C simulation programs.
echo �
echo I am going to run an example program.
echo You can stop here by pressing Control-C.
pause
echo off
SSSGO C EX_0505
:end
 
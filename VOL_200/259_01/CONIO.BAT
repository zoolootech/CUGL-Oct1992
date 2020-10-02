ECHO OFF
IF %1.==. GOTO PARMERR
IF EXIST %1\?LIBC.LIB GOTO CLIBC
   ECHO CAN'T FIND C LIBRARY FILES IN DIRECTORY %1
   GOTO PARMERR
:CLIBC
IF NOT EXIST %1\CLIBC.LIB GOTO SLIBC
   ECHO INSTALLING CONIO ROUTINES INTO %1\CLIBC.LIB
   ECHO FOR COMPACT MEMORY MODEL...
   lib %1\clibc -putch -getch -getche -ungetch -kbhit -+conio.ner; > NUL
:SLIBC
IF NOT EXIST %1\SLIBC.LIB GOTO MLIBC
   ECHO INSTALLING CONIO ROUTINES INTO %1\SLIBC.LIB
   ECHO FOR SMALL MEMORY MODEL...
   lib %1\slibc -putch -getch -getche -ungetch -kbhit -+conio.ner; > NUL
:MLIBC
IF NOT EXIST %1\MLIBC.LIB GOTO LLIBC
   ECHO INSTALLING CONIO ROUTINES INTO %1\MLIBC.LIB
   ECHO FOR MEDIUM MEMORY MODEL...
   lib %1\mlibc -putch -getch -getche -ungetch -kbhit -+conio.far; > NUL
:LLIBC
IF NOT EXIST %1\LLIBC.LIB GOTO END
   ECHO INSTALLING CONIO ROUTINES INTO %1\LLIBC.LIB
   ECHO FOR LARGE & HUGE MEMORY MODELS...
   lib %1\llibc -putch -getch -getche -ungetch -kbhit -+conio.far; > NUL
GOTO END
:PARMERR
ECHO  You must specify the directory where your C libraries are.
ECHO  Example:  CONIO C:\MSC\LIB
:END

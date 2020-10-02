@ECHO OFF
CLS
ECHO This .BAT file compiles DBLIB under C++
REM
	REM validate model. 
	set mdl=%1
	for %%a	in ( s m l t h c y ) do  if (%mdl%)==(%%a) goto :alright
	ECHO parameter 1 must be memory model, or y for overaly, large model.
	goto :quit
:ALRIGHT
	set ovrly=
	if  NOT (mdl) == (y)  goto :NOLAY
		REM overalys specified by 'y' as model.
		set mdl=l
		set ovly=-Yo
	:NOLAY  



	if exist *.obj    erase *.obj
	if (%2) == () goto  :ALLFILE
		REM specific files were given to compile.
		@ECHO ON	
tcc -c -m%mdl% %olay% %2 %3 %4 %5 %6 %7 %8 %9 >errors.lst
		@ECHO OFF
		goto :HAVEOBJ
	:ALLFILE
		REM no files specified, so do all files.
		@ECHO ON	
tcc -c -m%mdl% %olay% *.c* > errors.lst
		@ECHO OFF 
	:HAVEOBJ
	if NOT EXIST *.obj goto :DONE
	dir *.obj  | dir2bat -+@ & > obj.lst
	echo ,liblst               >>obj.lst
	@ECHO ON
	tlib dblib%1 @obj.lst >> errors.lst
	@ECHO OFF
	if exist *.obj          erase *.obj
	if exist obj.lst        erase obj.lst
	if exist dblib?.bak     erase dblib?.bak

:DONE
ECHO library made. Compiler and Librarian messages are in errors.lst
:QUIT

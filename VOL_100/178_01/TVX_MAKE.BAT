REM This is the batch file needed to compile and line TVX using
REM Computer Innovations C-86 compiler.  If you have a different
REM compiler, you will have to make your own batch file.
REM	This batch file assumes stdio.h is in a directory called \c\.
REM
p1 tvx_1 -hc:\c86\
if errorlevel 1 goto error
p2 tvx_1
if errorlevel 1 goto error
p3 tvx_1
if errorlevel 1 goto error
p4 tvx_1
if errorlevel 1 goto error
p1 tvx_2 -hc:\c86\
if errorlevel 1 goto error
p2 tvx_2
if errorlevel 1 goto error
p3 tvx_2
if errorlevel 1 goto error
p4 tvx_2
if errorlevel 1 goto error
p1 tvx_lex -hc:\c86\
if errorlevel 1 goto error
p2 tvx_lex
if errorlevel 1 goto error
p3 tvx_lex
if errorlevel 1 goto error
p4 tvx_lex
if errorlevel 1 goto error
p1 tvx_io -hc:\c86\
if errorlevel 1 goto error
p2 tvx_io
if errorlevel 1 goto error
p3 tvx_io
if errorlevel 1 goto error
p4 tvx_io
if errorlevel 1 goto error
p1 tvx_lib -hc:\c86\
if errorlevel 1 goto error
p2 tvx_lib
if errorlevel 1 goto error
p3 tvx_lib
if errorlevel 1 goto error
p4 tvx_lib
if errorlevel 1 goto error
p1 tvx_ibm -hc:\c86\
if errorlevel 1 goto error
p2 tvx_ibm
if errorlevel 1 goto error
p3 tvx_ibm
if errorlevel 1 goto error
p4 tvx_ibm
if errorlevel 1 goto error
REM
REM	Now, link the thing into a file called TVX.EXE
REM	It uses the CII small, DOS 2 library, which is called /c/cslib2
REM	on my system.  You may need to replace the library with whatever
REM	you call the corresponding library (like c86s2s.lib).
REM
link tvx_1+tvx_2+tvx_lex+tvx_io+tvx_lib+tvx_ibm,tvx,TVX_MAP,\c86\c86s2s
if errorlevel 1 goto error
REM
REM	Build the patch / configuration utilities
REM
p1 tvx_cfg -hc:\c86\
if errorlevel 1 goto error
p2 tvx_cfg
if errorlevel 1 goto error
p3 tvx_cfg
if errorlevel 1 goto error
p4 tvx_cfg
if errorlevel 1 goto error
link tvx_cfg,tvx_cfg,TVX_MAP1,\c86\c86s2s
if errorlevel 1 goto error
p1 tvx_ptch -hc:\c86\
if errorlevel 1 goto error
p2 tvx_ptch
if errorlevel 1 goto error
p3 tvx_ptch
if errorlevel 1 goto error
p4 tvx_ptch
if errorlevel 1 goto error
link tvx_ptch,tvx_ptch,TVX_MAP2,\c86\c86s2s
if errorlevel 1 goto error
REM
REM	Build of tvx finished
REM
:error
echo 

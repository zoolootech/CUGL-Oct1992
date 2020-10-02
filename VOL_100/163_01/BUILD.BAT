echo off
rem         This batch file is used to build
rem         the Small-C compiler.
echo on
cc cc1 ;
cc cc2 ;
cc cc3 ;
cc cc4 ;
cc cc5 ;
masm cc1 ;
masm cc2 ;
masm cc3 ;
masm cc4 ;
masm cc5 ;
link cc1 cc2 cc3 cc4 cc5,cc,cc,c

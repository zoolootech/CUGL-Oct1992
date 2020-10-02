echo off
rem
rem  This batch file is used to build test.exe;
rem  a demonstration of the mult-dimensional array
rem  capability of Version 1.01 of Small-C
rem
echo on
cc test ;
cc ext ;
masm test ;
masm ext ;
link test ext,test,test,c

@echo off
rem ctocpp1.bat processes a single file
echo %1
cl /E %1 > foo.tmp
ctocxx < foo.tmp
rm foo.tmp


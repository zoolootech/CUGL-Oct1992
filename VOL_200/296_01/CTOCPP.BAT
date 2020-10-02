@echo off
rem .bat command file to convert .c to .cpp files
for %%p in (%1 %2 %3 %4 %5 %6 %7 %8 %9) do  call ctocpp1.bat %%p


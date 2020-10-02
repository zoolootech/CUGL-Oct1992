rem             Batch file to make explod.exe using Turbo C
rem    You may have to change the paths in the tcc command to suit your setup
masm /mx expa ;
tcc -I\tc\inc -L\tc\lib explod.c expa.obj

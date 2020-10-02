REM  Compiling and linking CSRDEMO for Turbo C.
TCC -c CSRDEMO
TLINK C0S CSRDEMO,CSRDEMO,,LIB\CS LIB\TSCSR
Erase CSRDEMO.MAP
Erase CSRDEMO.OBJ

REM  Compiling and linking CSRDEMO for Microsoft C.
MSC CSRDEMO /DMSC;
LINK CSRDEMO,,,CSRMSC;
Erase CSRDEMO.MAP
Erase CSRDEMO.OBJ

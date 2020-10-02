REM This file is an $include Quick Basic file when using
REM the medium memory model of the CESMouse Tools Library
REM Copyright 1989, 1991, Computer Engineering Services

DECLARE FUNCTION showmouse CDECL
DECLARE FUNCTION hidemouse CDECL
DECLARE FUNCTION posmouse CDECL (X AS INTEGER, Y AS INTEGER)

DECLARE FUNCTION resetmouse% CDECL
DECLARE FUNCTION mousedrvr% CDECL
DECLARE FUNCTION buttons% CDECL
DECLARE FUNCTION lftbutton% CDECL
DECLARE FUNCTION ctrbutton% CDECL
DECLARE FUNCTION rgtbutton% CDECL
DECLARE FUNCTION mousecursX% CDECL
DECLARE FUNCTION mousecursY% CDECL

DECLARE lastMX AS INTEGER
DECLARE lastMY AS INTEGER
DECLARE mbtns AS INTEGER

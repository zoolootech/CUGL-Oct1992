   EXAMPLE USAGE OF PUSH & POP IN BATCH FILE

WS.BAT

ECHO OFF
PUSH
CD \BATCH
KEY WS2 /ML
CD \WS2000
WS2 %1 %2
CD \BATCH
KEY START /ML
POP
^Z

PUSH.EXE and POP.EXE are also stored in C:\BATCH (on the PATH).
The references to "KEY" are invocations of Borland's Superkey.
The reference to "WS2" is the invocation of MicroPro's WordStar 2000.
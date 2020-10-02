echo on

Rem Get rid of old files, just in case ..

@If exist PCBFAIL.TXT Del PCBFAIL.TXT
@If exist PCBPASS.TXT Del PCBPASS.TXT
@If exist ZDCS-DEL.LST Del ZDCS-DEL.LST

Rem Remember to change the drive & path info for your configuration.

ExZTest %1 0 %2 /!r:\wrk\comment /s /w

ZDCSFC16 %1 %2 %3

IF NOT EXIST ZDCS-DEL.LST GOTO END
pkzip -d %1 @ZDCS-DEL.LST
:END

;yam configuration file.  Frequently changed parameters are included here.
;All null lines and those with ';' at begining of line of line are ingnored.

;sign on
FLAVOR=Hayes mode YAM\n
REVISION=MS-DOS Version rev 9 NOV 85\n

;initial configuration as in phone list
PORTINIT=b1200   S1PND8  I

;ignore these characters on rx
RXNONO=\0x7f

;xon-xoff character
XON=\0x11
XOFF=\0x13

: Build SSORT.COM - Select-a-SORT
:
: cc is BDS C vers 1.5
: ll is L2 which comes with BDS C or via BDS C users group
:
: MicroShell users: Don't actually use this shell
: file since ASM gets confused when activated from
: within a shell file. Just treat it as build instructions.
:		Harvey Moran 2/26/84
:
cc ssort -o -e 3800
casm lexlate
asm lexlate.ddz
ddt
f100,1000,0
iLEXLATE.HEX
r
g0
save 4 lexlate.crl
ll ssort lexlate -WA
ou have MicroShell, I probably don't have to tell you notä	  
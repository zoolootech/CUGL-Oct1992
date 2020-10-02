#  makefile for PC-Gammon IV produces BACKGMMN.EXE
#
backgmmn.exe: backgmmn.h   #alter backgmmn.h to force a total compile
     cl -Fe$* bg0.c bg1.c bg2.c bg3.c -link ciao /EX
     beep

bg0.obj: bg0.c
     cl -c bg0.c

bg1.obj: bg1.c
     cl -c bg1.c

bg2.obj: bg2.c
     cl -c bg2.c

bg3.obj: bg3.c
     cl -c bg3.c

backgmmn.exe: bg0.obj bg1.obj bg2.obj bg3.obj ciao.lib
     cl -Fe$* bg0 bg1 bg2 bg3 -link ciao /EX
     beep


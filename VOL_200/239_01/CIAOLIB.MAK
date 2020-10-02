#
#  CIAOLIB -- makefile for CIAO.LIB
# "Never MAKE At Midnight!"
#
ciao.obj: ciao.c        # ibm video RAM primitives
   cl -c ciao.c

clairol.obj: clairol.c  # popout to set screen attributes
   cl -c clairol.c

sound.obj: sound.c      # sound effects primitives
   cl -c sound.c

wprintf.obj: wprintf.c  # formatted window output, with attributes
   cl -c wprintf.c

keyin.obj: keyin.c      # character input
   cl -c keyin.c

getms.obj: getms.c      # string input
   cl -c getms.c

noop.obj: noop.c        # minor but useful utils
   cl -c noop.c

xeq.obj: xeq.c          # run a file in a directory
   cl -c xeq.c

typeafil.obj: typeafil.c  # patented typeafile in a windowbox with word wrap
   cl -c typeafil.c

money.obj: money.c
   cl -c money.c

keydefs.obj: keydefs.c  # patented Fn key popout menu
   cl -c keydefs.c

ciao.lib: ciao.obj clairol.obj sound.obj wprintf.obj \
          keyin.obj getms.obj noop.obj xeq.obj typeafil.obj money.obj
 if exist ciao.lib erase ciao.lib
 lib ciao+ciao+wprintf+keyin+getms+noop+sound+clairol+xeq+typeafil+money+keydefs,ciao.xrf;


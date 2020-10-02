#
#  Create TAVL library.  Unlike the tavl*.c & tavl*.h source code,
#                        which is "Standard C" and does not depend
#                        on any particular compiler, this MAKEFILE
#                        is geared to Turbo C v2.0, Borland's MAKE
#                        and Borland's library utility "TLIB", and
#                        is included purely as a convenience and
#                        for illustrative purposes.
#
#    Naming convention: tavl_t.lib    -  TINY    memory model library
#                       tavl_s.lib    -  SMALL   memory model library
#                       tavl_m.lib    -  MEDIUM  memory model library
#                       tavl_c.lib    -  COMPACT memory model library
#                       tavl_l.lib    -  LARGE   memory model library
#                       tavl_h.lib    -  HUGE    memory model library
#
#
#    Example command line to create compact memory model library:
#        MAKE -DMDL=c -ftavl_lib.mak
#
!if !$d(MDL)
MDL=s
!endif
#
#
tavl_$(MDL).lib: tavlinit.obj tavlpred.obj tavlsucc.obj tavl_sdt.obj \
                 tavl_rst.obj tavlfree.obj tavlfind.obj tavlrebl.obj \
                 tavl_ins.obj tavl_del.obj tavl_gdt.obj tavldall.obj
  tlib tavl_$(MDL) +tavlinit +tavlpred +tavlsucc +tavl_sdt +tavl_rst +tavldall
  tlib tavl_$(MDL) +tavlfree +tavlfind +tavlrebl +tavl_ins +tavl_del +tavl_gdt
#
#  Don't leave the object files lying around; they will interfere
#  with creating other memory model libraries.
#
  del tavlinit.obj
  del tavlpred.obj
  del tavlsucc.obj
  del tavl_sdt.obj
  del tavl_rst.obj
  del tavlfree.obj
  del tavlfind.obj
  del tavlrebl.obj
  del tavl_ins.obj
  del tavl_del.obj
  del tavl_gdt.obj
  del tavldall.obj
#
#
#
.c.obj:
  tcc -c -m$(MDL) -A -G -a -Z -O -DNDEBUG $*
#
#  - compiler options:
#
#  -c              compile only (REQUIRED)
#  -m$(MDL)        memory model (REQUIRED)
#  -DNDEBUG        NDEBUG defined - removes assertions (RECOMMENDED)
#  -A              ANSI keywords only
#  -G              optimize for speed
#  -a              word alignment
#  -Z              register optimization ON
#  -O              optimize jumps
#
#  Change the options to suit your needs - these are mine.
#

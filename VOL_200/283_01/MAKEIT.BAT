rem MAKEIT.BAT, 12/18/88, d.c.oshel
rem to make large model and small model Fafnir4x.Lib libraries
rem place all files in the currently active subdirectory
rem requires *INSTALLED* Microsoft C 5.0 (and utils) and MASM 5.0!
tm start
make /X - >s.err MDL=/AS ASM=svideo CIO=ciao FAF=sfaf LIB=4s fafnir
make /X - >l.err MDL=/AL ASM=lvideo CIO=liao FAF=lfaf LIB=4l fafnir
tm stop
rem compile histories are in s.err and l.err text files

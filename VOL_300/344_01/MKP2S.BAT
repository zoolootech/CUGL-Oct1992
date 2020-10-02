flex -L p2s.l
copy lexyy.c p2s.c
tcc -I\flex -DMSDOS=1 p2s.c



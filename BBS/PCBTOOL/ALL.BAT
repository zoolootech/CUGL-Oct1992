set cdrom=k:
set output=..\pcb

set in_type=cug

call doit zipped\listings listings
call doit zipped\vol_100 vol_100
call doit zipped\vol_200 vol_200
call doit zipped\vol_300 vol_300

set cdrom=
set output=
set in_type=

echo off
echo now i'm going to concatonate the dir.lst i just built with each
echo of the directories onto your dir.lst file.  the backup will be
echo 'dir_lst.old' in case it doesn't work.
pause

del dir_lst.old
copy dir.lst dir_lst.old
type dir_lst >> dir.lst
del dir_lst

echo that should be it....
  

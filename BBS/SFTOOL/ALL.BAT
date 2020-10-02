set cdrom=k:
set output=..\sf

rem call doit DEMO  DEMO

set in_type=cug

call doit zipped\listings SFFILES.30
call doit zipped\vol_100 SFFILES.31
call doit zipped\vol_200 SFFILES.32
call doit zipped\vol_300 SFFILES.33

set cdrom=
set output=
set in_type=
  

rem this is where the actual files live
set cdrom=k:
rem this is where you want your index files
set output=..\rbbs

set in_type=cug

call 2dir zipped\listings listings
call 2dir zipped\vol_100 vol_100
call 2dir zipped\vol_200 vol_200
call 2dir zipped\vol_300 vol_300

set cdrom=
set output=
set in_type=

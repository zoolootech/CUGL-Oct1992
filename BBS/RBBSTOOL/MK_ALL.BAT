rem  this batch file concatonates the individual rbbs .dir files together
rem  into one large index file.
rem  the simtel to rbbs conversion must already be done.

set cdrom=k:
set output=..\rbbs

del %output%\all.dir

type header > %output%\all.dir

set in_type=cug

call doit zipped\listings all.dir
call doit zipped\vol_100 all.dir
call doit zipped\vol_200 all.dir
call doit zipped\vol_300 all.dir

set cdrom=
set output=
set in_type=

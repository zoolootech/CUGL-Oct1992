rem  this batch file concatonates the individual rbbs .dir files together
rem  so there are only 4 file areas.
rem  you can edit this substituting your own last parameter to get a
rem  different arrangement of directories.

set cdrom=k:
set output=..\rbbs

type header > %output%\1.dir
type header > %output%\2.dir
type header > %output%\3.dir
type header > %output%\4.dir

set in_type=cug

call doit zipped\listings 1.dir
call doit zipped\vol_100 2.dir
call doit zipped\vol_200 3.dir
call doit zipped\vol_300 4.dir

set output=
set in_type=
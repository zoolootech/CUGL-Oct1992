rem %1 == disc directory
type header > %output%\%2.dir
2rbbs %in_type% \%1 < %cdrom%\%1\index.txt >> %output%\%2.dir


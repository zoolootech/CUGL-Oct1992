rem %1 == disc directory
rem %2 == file name
type header > %output%\%2
2sf %in_type% %cdrom%\%1 < %cdrom%\%1\index.txt >> %output%\%2


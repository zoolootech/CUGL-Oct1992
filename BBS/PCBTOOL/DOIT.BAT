rem %1 == disc directory
rem %2 == file name
type header > %output%\%2.dir
2pcb %in_type% %cdrom%\%1 < %cdrom%\%1\index.txt >> %output%\%2.dir
dirlst %output%\%2.dir %cdrom%\%1\ %2 >> dir_lst

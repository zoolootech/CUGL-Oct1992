:
#	name:		cclrepl.ksh
#	purpose:	Compile large model
#
if [ ! -f $1.c ]
then
	echo "Usage: $0 csource"
	echo "The csource must exist in this directory"
	echo "You must not define a file extension (.c)"
	exit 1
fi
rm -f $1.exe
rm -f $1.lst
rm -f $1.map
export INCLUDE="D:\INCLUDE;D:\INCLUDE\SYS;"
export LIB="D:\LIB"
export TMP="D:\TMP"
cl -c -AL -Fs -W2 $1.c
link /st:10000 $1+mdict+file+matches, /noi, $1, llibce
rm -f $1.obj
exit 0

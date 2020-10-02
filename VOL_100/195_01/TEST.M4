[TEST.M4 of JUGPDS Vol.18]

define(proc,[integer function $1$2 define(procname,$1)])
define(return,[{procname = $1; [return]}])

define(str,[ifelse($2,",,data $1(incr($3))/[LET]substr($2,1,1)/
[str($1,substr($2,2),incr($3))])])

define(string,[integer $1(len(substr($2,2)))
str($1,substr($2,2),0)data $1(len(substr($2,2)))/EOS/])


proc(equal,(str1,str2))

len(abc)
len("abc")
substr(abcdefg,2)
string(name,"text")
str(a,abc",0)
return(YES)
define(N,7)
arith(N,+,3)
arith(N,-,3)
arith(N,*,3)
arith(N,/,3)
arith(N,%,3)
arith(N,<,3)
arith(N,>,3)
arith(N,<=,3)
arith(N,>=,3)
arith(N,=<,3)
arith(N,=>,3)
arith(N,<<,3)
arith(N,>>,3)
arith(N,><,3)
arith(N,<>,3)

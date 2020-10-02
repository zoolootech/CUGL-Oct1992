command /c lcc %1
%1 <%1.in >%1.out
comp %1.out %1.tst
del %1.obj %1.exe

if NOT EXIST %1.c goto :end
cb %1.c | retabf -o3 -n1 | clv > %1.cb
xc2 %1.cb -w 104 -o %1.prn
del %1.cb
:end


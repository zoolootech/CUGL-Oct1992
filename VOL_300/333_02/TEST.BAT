awk -f P001.AWK emp.dat
awk -f P007.AWK emp.dat
awk -f P007A.AWK emp.dat
awk -f P008.AWK emp.dat
awk -f P009.AWK emp.dat
awk -f P009A.AWK emp.dat
awk -f P010.AWK emp.dat
awk -f P011.AWK emp.dat
awk -f P012.AWK emp.dat
awk -f P012A.AWK emp.dat
awk -f P012B.AWK emp.dat
awk -f P013.AWK emp.dat
awk -f P013A.AWK emp.dat
awk -f P014.AWK emp.dat
awk -f P014A.AWK emp.dat
awk -f P014B.AWK emp.dat
awk -f P015.AWK interest.dat
awk -f P016.AWK interest.dat
awk -f P016A.AWK emp.dat
awk -f P017.AWK emp.dat
awk -f P024.AWK country.dat 
awk -f P033.AWK country.dat 
awk -f P035.AWK country.dat 
awk -f P038.AWK country.dat 
awk -f P038A.AWK country.dat 
awk -f P043.AWK country.dat 
awk -f P043A.AWK country.dat 
awk -f P044.AWK country.dat 
awk -f P051.AWK country.dat 
awk -f P051A.AWK country.dat 
awk -f P054.AWK country.dat 
awk -f P063.AWK PARAM1 PARAM2 PARAM3 PARAM4 PARAM5
awk -f P090.AWK country.dat | awk -f P092.AWK
awk -f cntlines.awk *.dat
awk -f fs.awk fs.dat
awk "-F[\t ]*(SSN|NAME|TEL):[\t ]*" -f fs1.awk fs.dat

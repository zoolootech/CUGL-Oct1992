echo off
echo batch file to run test assembly
echo all:
asm -l all
pause
echo arith:  
asm -l arith
pause
echo bcd:
asm -l bcd 
pause
echo bitfield: 
asm -l bitfield
echo should be: 8 errors
pause
echo branch:
asm -l branch
echo should be: 14 errors, 10 warnings
pause
echo bit:
asm -l bit
pause
echo chk2:
asm -l chk2
pause
echo moveq:
asm -l moveq
echo should be: 3 warnings
pause
echo data:   
asm -l data    
pause
echo eaddress:
asm -l eaddress
pause
echo eaddres1:
asm -l eaddres1
pause
echo except:
asm -l except
pause
echo expressn:
asm -l expressn
pause
echo logic:
asm -l logic   
pause
echo miscel:
asm -l miscel  
echo should be: 38 errors, 13 warnings
pause
echo move:
asm -l move 
echo should be: 2 errors
pause
echo proc:   
asm -l proc
pause
echo absolute:    
asm -l absolute
echo should be: 3 errors
pause
echo range:
asm -l range
pause
echo shift:
asm -l shift
pause
echo strings:   
asm -l strings
echo should be: 1 error
pause
echo trapcc:    
asm -l trapcc  
echo should be: 10 errors, 4 warnings


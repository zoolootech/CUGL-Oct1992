echo off
echo  .   
echo      USAGE: install source_drive target_drive
echo  .    
echo      Your source_drive is %1
echo      Your target_drive is %2
echo  .   
echo      Hit the Ctrl-Break key and then enter the letter Y to terminate
echo      this install program if the source and target drives are incorrect.
echo  .   
pause
echo  .   
echo      Files being copied from %1\commands and %1\help  to  %2
copy %1\commands\*.*  %2
copy %1\help\*.*  %2
%2
help utilix
echo  .  
echo      To print the help files, set your printer to 12 cpi pitch, 
echo      then use the command:
echo on
rem           pr -s -w105 *.hlp >lpt1:

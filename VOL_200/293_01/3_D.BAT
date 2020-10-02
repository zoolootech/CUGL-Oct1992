REM       *************************************************
REM       *       3-D RECONSTRUCTION OF CT IMAGES         *
REM       *************************************************
REM
REM       Daniel Geist
REM       Michael W. Vannier
REM
REM       Mallinckrodt Institute of Radiology
REM       Washington University School of Medicine
REM       510 South Kingshighway Blvd.
REM       St. Louis, Mo. 63110
REM

REM                       PHASE 1
REM      ===============================================
REM      Running Shading Extarction Program & main axis views.
grad

REM                       PHASE 2
REM      ===============================================
REM      Running angle view program.
REM      (To stop angle view creation loop enter invalid angle)
:ANGLE
ang
IF NOT ERRORLEVEL 1 GOTO ANGLE 

REM                       PHASE 3
REM      ===============================================
REM      Flipping files
flipd gll.out /P
fliph gfr.out /P
flipv gre.out /P
flipv gto.out /D
flipd dll.out /P
fliph dfr.out /P
flipv dre.out /P
flipv dto.out /D
REM    ++++++++         Finished          +++++++++
REM
REM    You may want to erase the temporary files now 

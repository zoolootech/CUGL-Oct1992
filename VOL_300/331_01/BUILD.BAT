erase obj\*.obj
masm pcio,obj\;
msc se,obj\;
if errorlevel 1 goto stop
msc ged1,obj\;
if errorlevel 1 goto stop
msc edit,obj\;
if errorlevel 1 goto stop
msc hist,obj\;
if errorlevel 1 goto stop
msc paint,obj\;
if errorlevel 1 goto stop
msc search,obj\;
if errorlevel 1 goto stop
msc ged5,obj\;
if errorlevel 2 goto stop
msc block,obj\;
if errorlevel 1 goto stop
msc ged7,obj\;
if errorlevel 1 goto stop
msc store,obj\;
if errorlevel 1 goto stop
msc swap,obj\;
if errorlevel 1 goto stop
msc ged10,obj\;
if errorlevel 1 goto stop
msc roff,obj\;
if errorlevel 1 goto stop
msc term,obj\;
if errorlevel 1 goto stop
rem     msc tprintf,obj\;
l
:stop

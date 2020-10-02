rem Zortech C++ compile batch file
rem change include directories in the command line
rem only large model allowed
rem other switches are optional

ztc -a -b -C -c -e -g -Id:\zortech\include;c:\zc\dp -l -mL -v -cpp %1


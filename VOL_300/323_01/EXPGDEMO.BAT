rem   The parameters for these demos were chosen for the best 
rem   effect on a 8Mhz XT with HGC or EGA graphics.  You should 
rem   put in or increase the -d parameter for faster machines.   
rem   For CGA, reduce the size of the -x and -y parameters.
rem
rem   You can press ESC during each demo to skip it.
rem

expgen -sr -p300 -x250 -y150 -t1 >temp
explod -s8 -d1 -n30 temp
expgen -sr -p300 -x250 -y150 -t12 >temp
explod -s2 -d10 -n9 temp
expgen -sr -p75 -t10 >temp
explod -s20 -n80 temp
expgen -sr -p200 -x150 -y100 -t8 >temp
explod -s8 -w150 -n40 temp
expgen -sr -p300 -x300 -y180 -t8 >temp
explod -s4 -g40 -n20 temp

expgen -st -p75 -x300 -y100 -t3 >temp
explod -s30 -d1 -n150 temp
expgen -sx -p75 -x250 -y150 -t10 >temp
explod -s20 -n100 temp
expgen -s+ -p300 -x300 -y180 -t8 >temp
explod -s8 -g40 -w-300 -n40 temp

del temp

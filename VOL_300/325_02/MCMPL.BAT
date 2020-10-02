rem batch file to compile in Microsoft C
rem must be compiled using /AH, huge model
rem other switches are optional

cl /Id:\msc\include /Ic:\mc\dp /G2 /AH /W3 /c /Zi /Fs /Gs /Oait %1.c

#  dem_menu.c make file

dem_menu.obj	:  dem_menu.c
  cl /c dem_menu.c

dem_menu.exe : dem_menu.obj
  link dem_menu,,, uclib.lib

			c:
			cd\tc\include
			copy a:jjbkbd.h
			copy a:jjbset.h
			cd\tc
			copy a:jjb*.*
			del jjbqc*.*
			tlib  lib\cs - jjbtc - jjbtcinp - jjbtcdol
			tlib  lib\cs + jjbtc + jjbtcinp + jjbtcdol
			tc jjbshow1.c

			c:
			cd\tc\include
			copy a:jjbkbd.h
			copy a:jjbset.h
			cd\tc
			copy a:jjbtc.obj
			copy a:jjbtcinp.obj
			copy a:jjbtcdol.obj
			tlib  lib\cs + jjbtc + jjbtcinp + jjbtcdol

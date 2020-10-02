			c:
			cd\bin
			copy a:jjb*.*
			del jjbtc*.*
			copy jjbkbd.h    c:\include
			copy jjbset.h    c:\include
			del jjb*.h
			qlib jjbqc.obj jjbqcinp.obj jjbqcdol.obj
			ren  jjbqc.qlb  jjb.qlb
			jjbqcs1


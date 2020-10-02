			c:
			cd\include
			copy a:jjbkbd.h
			copy a:jjbset.h
			cd\bin
			copy a:jjbqc.obj      c:\bin
			copy a:jjbqcinp.obj   c:\bin
			copy a:jjbqcdol.obj   c:\bin
			del jjb.qlb
			qlib jjbqc.obj jjbqcinp.obj jjbqcdol.obj
			ren jjbqc.qlb  jjb.qlb

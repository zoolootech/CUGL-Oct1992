#include "timer1.h"
	double j, k;

	DO_SKIP("s.m")			  			OD
	DO_SKIP("p->m")					  OD
	DO_SKIP("a[k]")			  				OD
	j = k = 255;
	DO_FEXPR("(double)k")	(double)k  		OD
	DO_IEXPR("(long)k")		(long)k  		OD
	DO_IEXPR("(int)k")		(int)k  		OD
	DO_IEXPR("(char)k")		(char)k  		OD
	DO_IEXPR("&k")			(int)&k			OD
	DO_SKIP("~k")			  				OD
	DO_FEXPR("++k")			++k  			OD
	DO_SKIP("k++")			 	 			OD
	DO_FEXPR("!k")			!k  			OD
	DO_FEXPR("-k")			-k  			OD
	DO_FEXPR("j * k")		j * k  			OD
	DO_FEXPR("j / k")		j / k  			OD
	DO_SKIP("j % k")		 	 			OD
	DO_FEXPR("j + k")		j + k  			OD
	DO_FEXPR("j - k")		j - k  			OD
	k = 15;
	DO_SKIP("j << k")		 		 		OD
	DO_SKIP("j >> k")		 		 		OD
	k = 255;
	DO_FEXPR("j <= k")		j <= k  		OD
	DO_FEXPR("j != k")		j != k  		OD
	DO_FEXPR("j && k")		j && k  		OD
	DO_FEXPR("j || k")		j || k  		OD
	/* average the times for && and || */
	t_ctime[t_try-2] = t_ctime[t_try-1] = 
		(t_ctime[t_try-2] + t_ctime[t_try-1]) / 2.;
	DO_SKIP("j & k")		 	 			OD
	DO_SKIP("j | k")		 	 			OD
	DO_SKIP("j ^ k")		 	 			OD
	DO_FEXPR("i ? j : k")	j ? j : k  		OD
	DO_STMT("j = k")		j = k  			OD
}

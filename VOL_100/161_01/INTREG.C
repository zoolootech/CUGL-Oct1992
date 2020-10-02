#include "timer1.h"
	register int j, k;
	int a[10];
	struct st 
	{
		int junk;
		int m;
	} s;

	DO_SKIP("s.m")			  				OD
	k = (int)&s;
	DO_IEXPR("p->m")	((struct st *)k)->m  OD
	k = 2;
	DO_IEXPR("a[k]")		a[k]  			OD
	j = k = 255;
	DO_FEXPR("(double)k")	(double)k  		OD
	DO_IEXPR("(long)k")		(long)k  		OD
	DO_IEXPR("(int)k")		(int)k  		OD
	DO_IEXPR("(char)k")		(char)k  		OD
	DO_SKIP("&k")			  				OD
	DO_IEXPR("~k")			~k  			OD
	DO_IEXPR("++k")			++k  			OD
	DO_IEXPR("k++")			k++  			OD
	DO_IEXPR("!k")			!k  			OD
	DO_IEXPR("-k")			-k  			OD
	DO_IEXPR("j * k")		j * k  			OD
	DO_IEXPR("j / k")		j / k  			OD
	DO_IEXPR("j % k")		j % k  			OD
	DO_IEXPR("j + k")		j + k  			OD
	DO_IEXPR("j - k")		j - k  			OD
	k = 15;
	DO_IEXPR("j << k")		j << k  		OD
	DO_IEXPR("j >> k")		j >> k  		OD
	k = 255;
	DO_IEXPR("j <= k")		j <= k  		OD
	DO_IEXPR("j != k")		j != k  		OD
	DO_IEXPR("j && k")		j && k  		OD
	DO_IEXPR("j || k")		j || k  		OD
	/* average the times for && and || */
	t_ctime[t_try-2] = t_ctime[t_try-1] = 
		(t_ctime[t_try-2] + t_ctime[t_try-1]) / 2.;
	DO_IEXPR("j & k")		j & k  			OD
	DO_IEXPR("j | k")		j | k  			OD
	DO_IEXPR("j ^ k")		j ^ k  			OD
	DO_IEXPR("i ? j : k")	j ? j : k  		OD
	DO_STMT("j = k")		j = k  			OD
}

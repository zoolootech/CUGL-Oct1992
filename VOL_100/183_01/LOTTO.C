/*
	lotto.c
		print batches of six numbers for lottery betting.

8/29/84	rjf	created.

*/
main(argc,argv)
int argc;
char argv[];
{

	double drand48();
	int z,x,ay[44];
	
	for(;;) {
		for (z=0;z<45;++z)
			ay[z] = 0;
			for (z=1;z<7;++z) {
	 		do {
				x = (int)(drand48()*44)+1;
			   }
			while (ay[x] == 1) ;
			ay[x] = 1;			
			printf("%2d\t",x);
		}
		printf("\n");
	}
}

                                                                                                          ntf("\n");
	}
}

                                                                                                          
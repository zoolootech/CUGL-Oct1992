
/*  EADVENT.C   no modifications for V 1.43 */
#include "advent.h"

main()
{
	init();
	if (yes(65,1,0))
		limit = 1000;
	else
		limit = 330;
	while(!saveflg)
		turn();
	if (saveflg)
		exec("saveadv");
}

/*
	Initialization
*/
init()
{
	fd[1]=fopen("advent1.dat",dbuff);
	fd[2]=fopen("advent2.dat",dbuff);
	fd[3]=fopen("advent3.dat",dbuff);
	fd[4]=fopen("advent4.dat",dbuff);
	fd[5]=fopen("advent5.dat",dbuff);
	fd[6]=fopen("advent6.dat",dbuff);
	srand(511);
}

/* MOVER.C v0.1 */

#include <undelete.h>


main(argc,argv)
int argc; char *argv[];
{
	int x; 
	char foo[40], current, extent, diag, temp, orig, Unum, Debug;
	char Moving;
	if(argc < 2)
	{
		printf(
		 "\nUsage:undelete <filename> <filename>[-d][-U<num>][-M].....");
		exit();
	}

	Debug = Dump = Unum = 0;
	for(x=1;x<argc;x++)
		if(argv[x][0] == '-')
		{
			switch(toupper(argv[x][1])){
			case 'D': Dump = 1;
				argv[x][0] = '\0'; /* remove entry */
				break;
			case 'U': _mvx(argv[x]); 
				if(Debug) printf("\nString became '%s'",argv[x]);
				Unum = (atoi(argv[x]) & 0x1f);
				argv[x][0] = '\0';
				printf("\nRestoring to user #%d",Unum);
				break;
			case 'M':Moving = 1;
				argv[x][0] = '\0';
				printf("\nUndeleted entries will be moved.");
				break;

			case 'X':argv[x][0] = '\0';
				Debug = 1;
			}
		}



	orig = bdos(0x19);
	current = orig;
	fillarray(current);
	for(x=1;x<argc;x++)
	{
		if(argv[x][0] == '\0') continue; /* Skip null ones */
		if(argv[x][1] == ':')
		{
			temp = toupper(argv[x][0]) - 'A';
			if(temp != current)
				fillarray(current=temp);
			_mvx(argv[x]);
		}
		else if(orig != current) fillarray(current=orig);


		extent = 0;
		do 
		{
			setfcb(foo,argv[x]);
			foo[12] = extent++;
			if((!(diag = findfcb(foo))) ||
			 ((marker.ptr->user != 0xe5) && !Moving))
			{
				if(!diag)
					printf("\n%s not found in directory.",argv[x]);
				else
					printf("\n%s was not a deleted entry.",argv[x]);
				break;
			}
			sector[marker.sec].dir[marker.entry].user = Unum;
			bios(SETDMA,sector[marker.sec].dir);
			bios(SETTRK,DTRACK+(marker.sec / NSECS));
			bios(SETSEC,translate(marker.sec % NSECS));
			bios(DWRITE);
		} while(marker.ptr->ext == 128);
	}
}


 ent
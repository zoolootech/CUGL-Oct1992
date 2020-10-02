



#include "bdscio.h"
#include "makedef.h"
#define MINCPM 0x1680




main( argc, argv )
int argc ;
char **argv ;
{
	char boot[16], cpmfile[16], bios[16] ;
	unsigned parm[MAXVECTOR] ;
	char test, *p, n ;
	char *biosadress();
	unsigned cpmbytes, bootbytes, biosbytes, cpmsize ;
	unsigned bootsiz ;
	char *area, *sbrk(), linebuf[256] ;

	putchar('\n');
	printf("      + + +  CP/M 2.2 System Generator  + + + \n");
	printf("             (c) 1981  Occam Mjukvaror\n");
	area = sbrk( MINCPM );
	if(area == ERROR){
		printf("Sorry, but you've got too little memory space.\n");
		exit();
		}

	cat( argc, argv, linebuf );
	if(parm[TEST] == TRUE)
	puts( linebuf );puts("\n");
	cpmsize = 0x1600 ;
	argument( linebuf, boot, cpmfile, bios, parm );
	if(parm[TEST] == TRUE){
	printf("Cpmbase in core = %4xH\n",area);
	if(parm[ISBOOT] == TRUE)
		printf("Bootfile = %s \n",boot);
	printf("Cpmfile = %s \n",cpmfile );
	printf("Biosfile = %s \n", bios );
	for( argc=0;argc < MAXVECTOR;argc++)
		printf("parm %u = %u \n",argc,parm[argc]);
	}
	rcpm( cpmfile, area );
	if(parm[ISBOOT] == TRUE)bootsiz = rhex( boot, area, 128, 0,0);
	if(parm[TEST] == TRUE)printf("Biosbase in core = %4xH\n",area + 0x1680);
	if(parm[NOCHECK] == TRUE)biosbytes = rhex( bios, area + 0x1680, 9999, 0,0);
	else biosbytes = rhex( bios, area + 0x1680, 9999, biosadress( area ),0);
	cpmsize += biosbytes ;
	if(parm[TEST] == TRUE)printf("Biosadress = %4x \n",biosadress(area));
	if(parm[TEST] == TRUE)printf("Bios was %u bytes.\n",biosbytes);
	if(parm[PATCH] == TRUE){
		p = (parm[PATCHADRESS] - 0x900) + area ;
		if(parm[TEST] == TRUE)printf("Patch adress in core = %4xH\n",p);
		for(n = 0; parm[PATCHBYTE + n] != ERROR ; n++){
			if(parm[PATCHBYTE] == ERROR)break ;
			*p++ = parm[PATCHBYTE + n ];
			}
		}
	if(parm[AUTOPATCH] == TRUE){
		p = (0x18de - 0x900) + area ;
		*p++ = 0xc3 ;
		bootbytes = biosadress(area);
		*p++ = ((bootbytes + 0x33) & 0x00ff );
		*p++ = ((bootbytes + 0x33) >> 8 ) &0x00ff ;
		}
	bootbytes = cpmsize / 128 + 1 ;
	if(parm[TEST] == TRUE)printf("Sectors to write = %u\n",bootbytes);
	if(parm[IMMEDIATE] == TRUE)sysgen(parm[DRIVE],area);
	else wcpm( cpmfile, area, bootbytes );
	p = biosadress( area );
	bootbytes = p ;
	printf("  CCP  adress = %4xH       CCP  size = %u bytes.\n",p - 0x1600,
		    0x800 );
	printf("  BDOS adress = %4xH       BDOS size = %u bytes.\n",(p - 0x1600) + 0x800,
		    (0x1600 - 0x800));
	printf("  BIOS adress = %4xH       BIOS size = %u bytes.\n",p,biosbytes);
	printf("  CP/M system = %u kbyte    CP/M size = %u bytes.\n",(bootbytes + 0x600)/1024,
		     cpmsize );
	if(parm[ISBOOT] == TRUE)
	printf("  BOOT hex file was %s, it contained %u bytes.\n",boot,bootsiz);
	printf("  BIOS hex file was %s.\n",bios);
	printf("  CP/M system read from %s.\n",cpmfile);
	if(parm[PATCH] == TRUE){
		printf("  Patch installed at adress %4xH, bytes: ",parm[PATCHADRESS]);;
		for(n=0;n<MAXPATCH;n++){
			if(parm[PATCHBYTE + n] == ERROR)break;
			printf("%2xH ",parm[PATCHBYTE + n]);
			}
		printf("\n");
		}
	if(parm[AUTOPATCH] == TRUE)
	printf("  Auto patch installed as preprogrammed.\n");
	if(parm[IMMEDIATE] == TRUE)
		printf("  Immediate sysgen on drive %c.\n",parm[DRIVE] + 'A');
	else printf("  The number of 128 byte sectors written to file %s was : %3u\n",
		cpmfile,cpmsize / 128 + 17 );

	parm[BOOTADRESS] = 0xf427 ;
	if(parm[DIRECTBOOT] == TRUE)call(parm[BOOTADRESS],0,0,0,0);
}






cat( argc, argv, linebuf )
int argc ;
char **argv, *linebuf ;
{
	char n ;

	*linebuf = 0 ;
	for(n = 1 ; n < argc ; n++ ){
		strcat( linebuf, argv[n] );
		strcat( linebuf, "     " );
		}
}


char *biosadress( base )
char *base ;
{
	char *o, c ;

	o = base + 0x82 ;
	c = *o ;
	return((c << 8) + 0x1300);
}




 /* C Program to read in an EPROM (2716-58) and display
in hex, the object dump. Displays checksum at end. Also
states whether the EPROM has been erased correctly (all FFH
data).  I would be glad to make available
 the schematic for the hardware adaptor
that uses 2 1/2 paralell IO ports.  The total cost in parts
is from $5-10.00.  The same hardware will allow programming
of 2716-58 style EPROMS. I have a Z-80 assembly language
prom burner but am in the process of converting it into C.  
Larry Langrehr
2069 N. Humboldt Blvd.
Chicago, Il. 60647  Eve phone (312) 278-5826
*/

#define CNTRL 4
#define DATA 2
#define RESET 17
#define IDLE 1
#define CLKHI 9
#define ROMSIZ 2048
char string[2];
int v[ROMSIZ], lincnt;
main ()
{
               rstcntr ();
               if (read() == 1)  {   /*    Check if erased  */
                    printf("\nProm Erased.\n");
                    exit ();
               }
               else
		    getsize ();
                    display ();
}
rstcntr ()   /*  reset address counter */
{
               outp(CNTRL,RESET);
               outp(CNTRL,IDLE);
}
getsize ()   /*  find out how many lines to dump  */
int k;
{
               printf("\nEnter # of pages to dump\n");
               k = atoi(gets(string));
               lincnt = k*16;
}
read ()     /*  read the entire prom into ram  */
int i, erasflg;
{
	erasflg = 1;
	for(i=0; i<ROMSIZ; ++i)   {
		v[i] = inp(DATA);
		if (v[i] != 255)
			erasflg = 0;
		clockpulse ();
	}
	return erasflg;
}
display ()
int linadrs, chksum;
int bytadrs, bytcnt;
int i,j;
{
               chksum = bytadrs = linadrs = 0;
               bytcnt = 16;
               for(i=0; i<lincnt; ++i)   {
                  printf("\n%4.0x ", linadrs);
                  linadrs += bytcnt;
                  for(j=0; j<bytcnt; ++j)  {
                      chksum += v[bytadrs];
                      printf("%2.0x ", v[bytadrs++]);
                  }
              }
              printf("\n\nChecksum = %x", chksum);
}
clockpulse ()
{
              outp(CNTRL,CLKHI);
              outp(CNTRL,IDLE);
}

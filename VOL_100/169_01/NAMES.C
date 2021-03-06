/*****************************************************************
 *  The following program is not intended on how to write a good *
 *  c program and there are many people out there who are   many *
 *  many times better c programmers than I will ever be. Most of *
 *  the questions I get on my BBS from new c programmers is "how *
 *  do I get output to disk.  There seems to be a lot of confus- *
 *  ion in this area.  Therefor I wrote this little program with *
 *  the hope that it might clear up some of the  questions  some *
 *  of you may have in the area.  It is the common name and addr *
 *  data entry program.  It was compiled with the DeSemet c com- *
 *  piler.  The output is a disk file on drive B that has  null  *
 *  characters between the fields and a cr/lf at the end of the  *
 *  record.     Hope it helps and have fun.  Lynn Long           *
 *****************************************************************/













static char *notes[6] = {
	"BUILD NAME AND ADDRESS FILE",
	"By Lynn Long",
	"Tulsa IBM-IBBS"
	"918-749-0718",
    "Press Any Key To Continue",
    "All Names Entered? "
	};

#include "stdio.h"
FILE *fd;
char buff[50];
char *input;
#define COMMA	44
#define ESCAPE  0x1b
main()

{
    int t;
    char c;
    hedrnote();
    c=getchar();
    if((fd=fopen("b:names.dat","wb")) < 0) {
    	printf("Error in creating n&a file");
    	exit(1);
    }
    t=TRUE;
    do {
    	t = bldfil();
       }while(t);
    fclose(fd);
    scr_clr();
}
hedrnote()
{
	frame(8,20,8,40);
    scr_rowcol(10,26);
    printf(notes[0]);
    scr_rowcol(11,34);
    printf(notes[1]);
    scr_rowcol(12,33);
    printf(notes[2]);
    scr_rowcol(13,34);
    printf(notes[3]);
    scr_rowcol(22,27);
    printf(notes[4]);
 


}
bldfil(t)
int t;
{

    char d;	

	frame(6,20,15,40);
    scr_rowcol(8,26);
    printf("FIRST NAME:");
    scr_rowcol(10,26);
    printf("LAST NAME:");
    scr_rowcol(12,26);
    printf("ADDRESS:");
    scr_rowcol(14,26);
    printf("CITY:");
    scr_rowcol(16,26);
    printf("STATE:");
    scr_rowcol(18,26);
    printf("ZIP CODE:");
    scr_rowcol(8,40);
    gets(buff);
    putout();
    scr_rowcol(10,40);
    gets(buff);
    putout();
    scr_rowcol(12,40);
    gets(buff);
    putout();
    scr_rowcol(14,40);
    gets(buff);
    putout();
    scr_rowcol(16,40);
    gets(buff);
    putout();
    scr_rowcol(18,40);
    gets(buff);
    putout();
    scr_rowcol(22,30);
    printf(notes[5]);
    putc('\n', fd);
    putc('\r', fd);
    d=getchar();
    if(d == 'y')
    	t=FALSE;
    else
        t=TRUE;
    return(t);
}
putout()
{
    input = buff;
    do {
            putc(*input, fd);
        }while(*input++);

}
/**********************************************************
 *   This function builds a nice looking frame for a      *
 *   menu.  The function is passed the parameters for     *
 *   the upper left corner row, upper left corner column  *
 *   the height of the frame and the width.  Depending    *
 *   on your particular compiler characteristics you      *
 *   will have to substitute its functions for clear-     *
 *   ing the screen and locating at the correct row       *
 *   and column on the screen. the functions you will     *
 *   need to replace with your own are scr_clr() and      *
 *   scr_rowcol(x, y).                                    *
 *                                                        *
 *          Program By                                    *
 *          Lynn Long                                     *
 *          Tulsa, OK RBBS "C" Bulletin Board             *
 *          918-749-0718                                  *
 *********************************************************/









#define			ULCOR			201
#define			URCOR			187
#define			LLCOR			200
#define			LRCOR			188
#define			VBAR			186
#define			HBAR			205

frame(row, col, hgt, wdth)
int row, col, hgt, wdth;

{
	int x, y;
    

	scr_clr();                                /*  insert your code here to clear screen  */
	scr_rowcol(row, col);                     /*  insert your function to locate row and col */
	putchar(ULCOR);
    for(x = col + 1; x <=(col + wdth -1); x++)
     	putchar(HBAR);
        putchar(URCOR);
        for(x = row + 1; x <=(row + hgt - 1); x++){
        scr_rowcol(x, col);    
        putchar(VBAR);
        scr_rowcol(x, col+wdth);
        putchar(VBAR);
   }
    scr_rowcol(x, col);  
    putchar(LLCOR);
    for(x= col + 1; x <=(col + wdth -1); x++)
    	putchar(HBAR);
    	putchar(LRCOR);	
} 
                                           col + 1; x <=(col + wdth -1); x++)
    	putchar(HBAR);
    	putchar(LRCOR);	
} 
                                                                                                                                                                                                                                                                                                           
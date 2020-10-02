/****************************************************************************/
/* plot.c                                                                   */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/* this file contains the functions necessary                               */
/* to display the bp.c error graph                                          */ 
/****************************************************************************/

#include <stdio.h>
#include <fg.h>
#include <stdlib.h>
#include <dos.h>

#include"plot.h"

#define U(x) (unsigned int)(x)       /* type conversion  */

void build_grid(void);
void prt_scr(void);
void menu(int);
void title(int);
void printer_msg(int);
int printer_status(void);
void peek(unsigned,unsigned,void *,int);

int x_base = 59;
int y_base = 23;
int x_div = 60;
int y_div = 30;
static fg_line_t graph_line;   /* holds the coordinates for graph line */

/****************************************************************************/
/* init_graph                                                               */
/* this function initialises graphics and draws grid                        */
/****************************************************************************/
void init_graph()
{
	fg_pbox_t graph_box;  /* an array of 4 integers */
	fg_line_t grid_line; /* array to hold line coordinates */

	int x_coord;
	int y_coord;

   int i,         /* X-axis counter */
       j;         /* Y-axis counter */

	if(fg_init_herc()==FG_NULL){
		fputs("UNABLE TO OPEN GRAPHICS DEVICE\n",stderr);
		exit (1);
	}

	title(FG_WHITE);

	graph_box[FG_X1] = x_base;
	graph_box[FG_Y1] = y_base;
	graph_box[FG_X2] = x_base + 10 * x_div;
	graph_box[FG_Y2] = y_base + 10 * y_div;

	fg_drawbox(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,
		graph_box,fg_displaybox);
	
   /* put in intersections */
   for(i=0;i<11;i++){                       /* counts along x axis */
      for(j=0;j<11;j++){                    /* counts along y axis */
			x_coord = x_base + x_div * i;
			y_coord = y_base + y_div * j;
			grid_line[FG_X1] = x_coord - 2;
			grid_line[FG_X2] = x_coord + 2;
			grid_line[FG_Y1] = y_coord ;
			grid_line[FG_Y2] = y_coord ;
			fg_drawline(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,grid_line);
			grid_line[FG_X1] = x_coord ;
			grid_line[FG_X2] = x_coord ;
			grid_line[FG_Y1] = y_coord - 2;
			grid_line[FG_Y2] = y_coord + 2;
			fg_drawline(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,grid_line);
      }
   }
	fg_puts(FG_WHITE,FG_MODE_SET,~0,FG_ROT90,20,150,
		"error",fg_displaybox);
	fg_puts(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,5,5,
		"iter",fg_displaybox);
}

/****************************************************************************/
/* title                                                                    */
/* this function prints title of graph                                      */
/****************************************************************************/
void title(
	int color
	)
{
	fg_puts(color,FG_MODE_SET,~0,FG_ROT0,x_base+10,y_base+10*y_div+10,
		"Back Propagation Generalised Delta Rule Learning Program",
		fg_displaybox);
}


/****************************************************************************/
/* close_graph                                                              */
/* this function clears the screen and resets to text                       */
/****************************************************************************/

void close_graph()
{
   int choice;           /* program control choice */

   printf("%c",7);       /* sound bell */

   title(FG_BLACK);

   for(;;){
      menu(FG_WHITE);
      choice = getch();

      switch(choice){
         case 'p':
         case 'P':
            /* print screen */
            /* replace title */
            menu(FG_BLACK);
            printer_msg(FG_WHITE);
            getch();
            printer_msg(FG_BLACK);

            title(FG_WHITE);
            prt_scr();      
            title(FG_BLACK);

            break;
         case 'v':
         case 'V':
             /* replace title */
             menu(FG_BLACK);
             title(FG_WHITE);
             getch();
             title(FG_BLACK);
             break;
         case 'q':
         case 'Q':
				fg_term();
            return;
         default:
            break;
      }
   }
}


/****************************************************************************/
/* set_scales                                                               */
/* this function takes the maximum values for X and Y and scales the axes   */
/****************************************************************************/

void set_scales(
   double error,
   int iter,
   double *x_scale,
   double *y_scale
)
{


   double y_max, y_step, y_val;
   double x_max, x_step, x_val;

   char buff[5];     /* holds the string conversion of scale values  */

   int i,         /* X-axis counter */
       j;         /* Y-axis counter */

   /* find the smallest integer value greater than error */
   for(y_max=0.0;y_max<error;y_max++);
   *y_scale = y_max;

   y_step = y_max/10.0;

   for(j=0;j<11;j++){
      y_val = ((double)j)*y_step;
      sprintf(buff,"%03.1f",y_val);
   	fg_puts(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,25,y_base+y_div*j-5,
			buff,fg_displaybox);
   }

   /* find the smallest round hundreds value greater than iter */
   for(x_max=0.0;x_max<((double)iter);x_max+=100.0);
   *x_scale = x_max;
   x_step = x_max/10.0;

   for(i=0;i<11;i++){
      x_val = ((double)i)*x_step;
      sprintf(buff,"%3.0f",x_val);
   	fg_puts(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,x_base+x_div*i-15,5,
			buff,fg_displaybox);
   }
	graph_line[FG_X1] = x_base;
	graph_line[FG_Y1] = (int)((error/y_max)*(double)(10*y_div))+y_base;
}

/****************************************************************************/
/* point                                                                    */
/* this function plots a line from previous value to new value              */
/****************************************************************************/

void point(
   double error,
   int iter,
   double x_max,
   double y_max
)
{
	graph_line[FG_X2] = (int)(((double)iter/x_max)*(double)(10*x_div))+x_base;
	graph_line[FG_Y2] = (int)((error/y_max)*(double)(10*y_div))+y_base;

	fg_drawline(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,graph_line);

	graph_line[FG_X1] = graph_line[FG_X2];
	graph_line[FG_Y1] = graph_line[FG_Y2];
}

/****************************************************************************/
/* menu                                                                     */
/* this function displays menu                                              */
/****************************************************************************/
void menu(
   int color                      /* color of letters */
)
{
	fg_puts(color,FG_MODE_SET,~0,FG_ROT0,x_base+10,y_base+10*y_div+10,
		"View    Print    Quit",
		fg_displaybox);
}
/****************************************************************************/
/* printer_msg                                                              */
/* this function prints a printer warning                                   */
/****************************************************************************/
void printer_msg(
   int color
)
{
	fg_puts(color,FG_MODE_SET,~0,FG_ROT0,x_base+10,y_base+10*y_div+10,
		"CHECK PRINTER     press any key when ready",
		fg_displaybox);

}
/****************************************************************************/
/* prt_scr                                                                  */
/* this function prints out video memory to a Star printer                  */
/* if you want to print 8 or 9 pin graphics you will need to replace fputc  */
/* with a function that does not convert \n to \r\n                         */
/****************************************************************************/
void prt_scr()
{
   int sweep;             /* a row made up of 6 lines across */
   int disp;              /* distance along a line (90 bytes across screen) */
   int line_no;           /* numbers the lines in a sweep */
   int b;                 /* binary digit within word[] */
   int printbyte;         /* fires pins */
   unsigned int word[6];  /* holds values of 6 lines in a sweep */
   unsigned int base;     /* beginning of video memory */
   unsigned int mask;     /* looks for a specific pixel */
	char buff;             /* buffer to hold peek return */
	
   base = 0xb000;         /* beginning of video memory for Hercules */

   fprintf(stdprn,"%c",7);         /* sound bell  */
   fprintf(stdprn,"%c%c%c",27,51,18); /* set printer linefeed to 1/12" */

   for(sweep=0;sweep<58;sweep++){
      fprintf(stdprn,"%c%c%c%c%c",27,42,6,208,2); /* set printer to graphics */
      for(disp=0;disp<90;disp++){
         for(line_no=0;line_no<6;line_no++){
            peek(base,
            	(0x2000*((sweep*6+line_no)%4)+90*(((sweep*6)+line_no)/4)+(disp)),
            	&buff,1);
            word[line_no] = (unsigned)buff;
         }
         mask = U(0x80);
         for(b=0;b<8;b++){  /* cycle through the binary digits */
            printbyte = 0;

            printbyte = ((word[0]&mask)&&mask)*128 + printbyte;
            printbyte = ((word[1]&mask)&&mask)* 64 + printbyte;
            printbyte = ((word[2]&mask)&&mask)* 32 + printbyte;
            printbyte = ((word[3]&mask)&&mask)* 16 + printbyte;
            printbyte = ((word[4]&mask)&&mask)*  8 + printbyte;
            printbyte = ((word[5]&mask)&&mask)*  4 + printbyte;

				fputc(printbyte,stdprn);	
            mask = mask/0x2u;
         }
      }
	   fprintf(stdprn,"%c",10);        /* line feed */ 
   }
   fprintf(stdprn,"%c%c%c",27,65,12); /* define printer line space to 1/6" */
   fprintf(stdprn,"%c%c",27,50);      /* activate line space definition */
   fprintf(stdprn,"%c",7);            /* sound bell  */
}



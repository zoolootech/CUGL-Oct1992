/*
HEADER:         CUGXXX;
TITLE:          Multiple character print program;
DATE:           3-20-86;
DESCRIPTION:    Prints character in multiples up, down, right or left. 
KEYWORDS:       Multiple character printing;
FILENAME:       DRAW.C;
WARNINGS:       None;
AUTHORS:        Jim Nech;
COMPILER:       DeSmet C;
REFERENCES:     US-DISK 1308;
ENDREF
*/
/*****************************************************************************
* These functions allow you to print a character multiple times up,down,right*
* or left. I have made these functions available in the hope that some of you*
* may find them of use.These functions work properly with the DeSmet C	     *
* compiler.Please send any comments or suggestions to:                       *
*                       Jim Nech                                             *
*                       10114 Torrington                                     *
*                       Houston Tx, 77075                                    *
*			(713)941-3100                                        *
*                       HAL-PC Member                                        *
*****************************************************************************/
draw_down(r,c,t,v)
{
int i;
for(i=0; i<t; i++)
scr_rowcol(r,c),printf("%c",v),r++;
}
draw_up(r,c,t,v)
{
while(t>0)
{
t--;
scr_rowcol(r,c);
printf("%c",v);
--r;
}
}


draw_right(r,c,t,v)
{
int i;
scr_rowcol(r,c);
for(i=0; i<t; i++)
printf("%c",v);
}


draw_left(r,c,t,v)
{
while(t>0)
{
t--;
scr_rowcol(r,c);
c--;
printf("%c",v);
}
}

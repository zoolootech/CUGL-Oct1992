
/*
 *      HEADER:         ;
 *      TITLE:          PROTOTYPES for memrite.obj and litebar.obj;
 *      DATE:           10/29/1989;
 *      VERSION:        1.0;
 *      FILENAME:       CRT.H;
 *      SEE-ALSO:       LITEBAR.ASM, MEMRITE.ASM;
 *      AUTHORS:        Michael Kelly;
 */

#define HORIZ 0
#define VERT 1

/*
 *  sets attribute on IBM screens in text mode without changing text.
 *  works on any video page either vertically or horizontally.
 *  handy for light bar menus and putting shadows on windows.
 /
extern void far pascal litebar(
    int atr,int row,int col,int page,int len,int dir
    );

/*
 *  writes text and attribute to IBM text screens on any video page
 *  either vertically or horizontally.
 */
extern void far pascal memrite(
    int atr,int row,int col,int page,int dir,char far *str
    );

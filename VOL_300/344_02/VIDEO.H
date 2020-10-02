

/*
 *      HEADER:         ;
 *      TITLE:          PROTOTYPES for screen functions in video.c;
 *      DATE:           10/29/1989;
 *      VERSION:        1.0;
 *      FILENAME:       VIDEO.H;
 *      SEE-ALSO:       VIDEO.C, CRT.H, LITEBAR.ASM, MEMRITE.ASM;
 *      AUTHORS:        Michael Kelly;
 */


/*
 *  gets current video mode
 */
unsigned char getmode(void);

/*
 *  based on Jeff Duntemans PASCAL detection routines in DDJ
 *
 *  detects the active video card on IBM PC & PS/2 machines
 */
enum VIDEO_CARD {NONE,MDA,CGA,EGAMONO,EGACOLOR,VGAMONO,VGACOLOR,MCGAMONO,MCGACOLOR};
enum VIDEO_CARD get_adaptor(void);

/*
 *  these use my assembler functions to draw boxes and borders for
 *  IBM PC and clone text screens
 */
int border(int left, int top, int right, int bot,int vpage, int style, int atr);
int box(int left, int top, int right, int bot,int vpage, int style, int atr);

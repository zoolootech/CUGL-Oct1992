/* VIDEO.H -- after Microsoft Systems Journal, Nov. 1988, p. 10 */

/*---------------------------------------------------------------*/
/* Direct Screen I/O, compatible with Monochrome or CGA adapters */
/* including Microsoft's own "snow elimination" algorithm.       */
/*---------------------------------------------------------------*/

/* FAFNIR library provides direct screen i/o support for Small and
   Large memory models.  Medium (/AM), Compact (/AC) and Huge (/AH) 
   memory models are not currently supported; these need their own
   assembler modules and some vetting in the C library code to work
   properly.

   There are 14 direct-video functions:

        in CVIDEO.C:

        extern void MSJ_GetVideoParms();
        extern void MSJ_ClrScr();
        extern void MSJ_ClrRegion();
        extern void MSJ_TextBox();
        extern void MSJ_SaveRegion();
        extern void MSJ_RestRegion();

        in xVIDEO.ASM (where x is S,M,CM,L):

        extern void far * cdecl MSJ_FarPtr( void near * );
        extern int        cdecl MSJ_GetCharAttr();
        extern void       cdecl MSJ_DispCharAttr();
        extern void       cdecl MSJ_DispString();
        extern void       cdecl MSJ_DispMsgLen();
        extern void       cdecl MSJ_SetFldAttr();
        extern void       cdecl MSJ_MovScrBuf();
        extern void       cdecl MSJ_MovBufScr();


   12/16/88, d.c.oshel, ames, ia
   */

/* Considerations:  Prosise's original CVIDEO.C included calls to int86()
                    indiscriminately in places other than GetVideoParms().
                    This destroys the generality of the algorithm, by
                    making it impossible to write to a general buffer
                    other than the physical screen buffer, by forcing
                    some CVIDEO routines to call ROM BIOS.

                    Note that an alternate screen buffer must assume
                    a starting offset of 0, and a segment address aligned
                    to make the first condition true.  I haven't included
                    any real general support for hidden text screens, yet. 

                    What I've done is to add a couple of service routines
                    to ASMVIDEO.ASM, and remove all int86()'s from this
                    and my own libraries, with the exception of routines
                    which read or update the physical cursor size or location, 
                    or which initialize the i/o module.

                    The only remaining hardware-dependent code is in the
                    assembler functions which use Microsoft's snow
                    elimination algorithm.  These poll the CRTC video
                    port (reads only) on the CGA card.

                    In general, these routines "bleed" onto the primary
                    screen in "multi-tasking" emulations, such as DoubleDos
                    or CodeView.  However, it is more important to keep
                    low-level screen i/o as object-oriented as possible,
                    than it is to adjust in all cases to programs written
                    with a specific hardware orientation.  My goal here is
                    PORTABILITY, not COMPATIBILITY.  This is, of course,
                    a copout.  The problem has no good solution, so I
                    have opted to pursue my own goals (in future development)
                    whilst forcing the compromise.  Tough nuggies.

                    12/10/88, d.c.oshel, ames, iowa
                    */

/*-------------------------------------------------------------------*/
/*     CAUTION:  These routines do little or no error trapping!      */
/*-------------------------------------------------------------------*/

/* Data */

struct MSJ_VideoInfo {
	unsigned char mode;
	unsigned char rows;
	unsigned char columns;
	unsigned char ColorFlag;
	unsigned char SnowFlag;
	unsigned int  BufferStart;
	unsigned int  SegAddr;
};

extern struct MSJ_VideoInfo video;  /* defined in CVIDEO.C */


/* C function prototypes */

extern void MSJ_GetVideoParms( struct MSJ_VideoInfo * sptr );

extern void MSJ_ClrScr(     char VideoAttr, 
                            struct MSJ_VideoInfo * sptr );

extern void MSJ_ClrRegion(  char row1, char col1, 
                            char row2, char col2, 
                            char VideoAttr );

extern void MSJ_TextBox(    char row1, char col1, 
                            char row2, char col2, 
                            char VideoAttr, 
                            struct MSJ_VideoInfo * sptr );

extern void MSJ_SaveRegion( char row1, char col1, 
                            char row2, char col2, 
                            int * ScreenBuffer, 
                            struct MSJ_VideoInfo * sptr );

extern void MSJ_RestRegion( char row1, char col1, 
                            char row2, char col2, 
                            int * ScreenBuffer, 
                            struct MSJ_VideoInfo * sptr );



/* macro assembler functions */


/* MSJ, Nov. 1988
   */
extern int cdecl MSJ_GetCharAttr( char row, char col, 
                                  struct MSJ_VideoInfo * sptr );

/* MSJ, Nov. 1988
   */
extern void cdecl MSJ_DispCharAttr( char ch, 
                                    char row, char col, 
                                    char VideoAttr, 
                                    struct MSJ_VideoInfo * sptr );


/* MSJ, Nov. 1988
   */
extern void cdecl MSJ_DispString( char * msg, 
                                  char row, char col, 
                                  char VideoAttr, 
                                  struct MSJ_VideoInfo * sptr ); 


/* MSJ_DispMsgLen is like MSJ_DispString, but takes a length argument,
   and does not disturb screen attributes in the receiving field
   12/10/88, d.c.oshel
   */
extern void cdecl MSJ_DispMsgLen( char * msg, 
                                  char row, char col, 
                                  int len,
                                  struct MSJ_VideoInfo * sptr ); 


/* MSJ_SetFldAttr takes a length argument and clears a field of the
   screen to the given char having the given attribute
   12/10/88, d.c.oshel
   */
extern void cdecl MSJ_SetFldAttr( char ch, 
                                  char row, 
                                  char col, 
                                  char VideoAttr,
                                  int len,
                                  struct MSJ_VideoInfo * sptr );


/* move count WORDS from screen to far buffer, 12/10/88, d.c.oshel 
   count is number of char/attr pairs to save or restore; be sure to 
   ALLOCATE count * sizeof(int) ... !
   */
extern void cdecl MSJ_MovScrBuf(  char far * buffer,
                                  char row,
                                  char col,
                                  int count,
                                  struct MSJ_VideoInfo * sptr );


/* move count WORDS from far buffer to screen, 12/10/88, d.c.oshel 
   count is number of char/attr pairs to save or restore; be sure to 
   ALLOCATE count * sizeof(int) ... !
   */
extern void cdecl MSJ_MovBufScr(  char far * buffer,
                                  char row,
                                  char col,
                                  int count,
                                  struct MSJ_VideoInfo * sptr );

/* eof */

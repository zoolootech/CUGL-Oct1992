/*  wn_defs.h
 *
 *  This header contains the definitions for the Unicorn Window System
 *  Version 2.0.  Currently menus and data forms are not implemented
 *  although they are in the works.  As with shareware in general the
 *  author connot promise a completion date as this is a spare time
 *  time project while the author moonlights about 40 hours a week
 *  with a Bellevue, WA software house attempting to feed his family
 *  and dogs.
 *
 */

#define TABS	     4
#define UP	   200

typedef (*PFI)();



typedef struct fld
{
   char       *fld_mask;     /* Data entry mask for this field	      */
   char       *fld_prot;     /* protection for this field	      */
   char       *fld_prompt;   /* Field prompt field		      */
   char       *fld_name;     /* Name label for field		      */
   int	       fld_number;   /* Number of this field in the form      */
   char       *fld_hlptag;   /* Help tag for this field.	      */
   char       *fld_message;  /* Message to be displayed when field is */
			     /* the currently active field.	      */
   int	       fld_prow;     /* Field prompt row		      */
   int	       fld_pcol;     /* Field prompt column		      */
   int	       fld_patt;     /* Field prompt attribute		      */
   char       *fld_data;     /* pointer to the field data, always kept*/
			     /* as a char *, the field type handles   */
			     /* any required data conversions.	      */
   int	      fld_type;      /* field type			      */
   int	      fld_active;    /* Field active attribute. 	      */
   int	      fld_inactive;  /* Field inactive attribute.	      */
   int	      fld_row;	     /* field row within the window	      */
   int	      fld_col;	     /* field column within the window	      */
   PFI	      fld_bfore;     /* function done before field data entry */
   PFI	      fld_after;     /* function done after field data entry  */
   PFI	      fld_val;	     /* field validation function	      */
   PFI	      usr_val;	     /* User validation function, called just */
			     /* before any defined fld_val().	      */
   PFI	      on_key;	     /* Called from the field editor when a   */
			     /* key is pressed by the user.  If this  */
			     /* function pointer is NULL ( default )  */
			     /* then no function call is performed.   */
   char       *user1;	     /* user pointer			      */
   char       *user2;	     /* user pointer			      */
   struct fld *next;	     /* pointer to next data field	      */
   struct fld *prev;	     /* pointer to previous data field	      */
} FIELD, *FIELDPTR;



typedef struct
{
   int	      nw;	     /* Northwest corner character	      */
   int	      ne;	     /* Northeast corner character	      */
   int	      sw;	     /* Southwest corner character	      */
   int	      se;	     /* Southeast corner character	      */
   int	      line;	     /* Horizontal line character	      */
   int	      side;	     /* Vertical side character 	      */
   char       *user1;	     /* user pointer			      */
   char       *user2;	     /* user pointer			      */
}BORDER, *BORDERPTR;


typedef struct clr
{
   int	      bdr;	     /* window border color		      */
   int	      title;	     /* Default window title color	      */
   int	      hilite;	     /* window highlighting color	      */
   int	      normal;	     /* window normal color		      */
   int	      prev;	     /* previous normal color		      */
   char       *user1;	     /* User available pointer		      */
   char       *user2;	     /* User available pointer		      */
} COLOR, *COLORPTR;

/*
typedef struct
{
   int	      row;
   int	      col;
} CURSOR, *CURSORPTR;
*/

typedef struct title
{
   int	      side;	     /* TOP or BOTTOM			      */
   int	      color;	     /* Attribute for the title.	      */
   int	      position;      /* Border column or RIGHT, LEFT or CENTER*/
   char       *title;	     /* pointer to the title text	      */
   struct title *next;	     /* pointer to the next title structure   */
   char       *user1;	     /* user pointer			      */
   char       *user2;	     /* user pointer			      */
} TITLE, *TITLEPTR;



typedef struct wn
{
   char       *wn_name;      /* Name for the window.		      */
   int	      visible;	     /* true if window is currently visible   */
   int	      hidden;	     /* true if window has been hidden	      */
   char       *sav_buff;     /* pointer to background save buffer     */
   TITLEPTR   title;	     /* pointer to the window title structure */
   int	      row;	     /* upper left row of window	      */
   int	      col;	     /* upper left column of window	      */
   int	      margins;	     /* width of right & left margins.	      */
   int	      width;	     /* window width			      */
   int	      height;	     /* window height			      */
   int	      wn_cur_row;    /* window current row pointer	      */
   int	      wn_select;     /* window selection		      */
   CURSORPTR  csr;	     /* pointer to cursor structure	      */
   BORDERPTR  bdr;	     /* window border type		      */
   COLORPTR   clr;	     /* Window color structure		      */
   struct wn  *next;	     /* pointer to next window in list	      */
   struct wn  *prev;	     /* pointer to previous window in list    */
   FIELDPTR   fld_first;     /* pointer to the first field structure  */
   FIELDPTR   fld_last;      /* pointer to the last field structure   */
   int	      wn_numflds;    /* Number of fields in the data form     */
   char       *wn_help;      /* Help tag for the window.	      */
   char       *user1;	     /* user pointer			      */
   char       *user2;	     /* user pointer			      */
   } WINDOW, *WINDOWPTR;


typedef struct wn_menu
{
   char       *mn_name;      /* pointer to the menu name	      */
   char       **mn_select;   /* pointer to the menu text selections   */
   void       (**func)();    /* 				      */

   void       (*mn_hlp)();   /* menu help function		      */
   char       *user1;	     /* user pointer			      */
   char       *user2;	     /* user pointer			      */
} MENU, *MENUPTR;


/*--------------------------------------------------------------------*/
/*  Definitions for border types and title positioning.  Positioning  */
/*  of titles and the ability to handle multiple titles for a single  */
/*  window is not yet implemented.  Currently you muct specify a      */
/*  column for the title to begin in rather than use the RIGHT, LEFT  */
/*  or CENTER justification parameters. 			      */
/*--------------------------------------------------------------------*/


#define BDR_LNP    &bdr_ln
#define BDR_DLNP   &bdr_dln
#define BDR_SLNP   &bdr_sln
#define BDR_12LNP  &bdr_12n
#define BDR_21LNP  &bdr_21n

#define RIGHT	   -1
#define LEFT	   -2
#define CENTER	   -3

#define TOP	   -1
#define BOTTOM	   -2


/*--------------------------------------------------------------------*/
/*  Data entry definitions					      */
/*--------------------------------------------------------------------*/

#define FIELDCHAR    '_'

#define FLD_ALPHA_NUMERIC     1
#define FLD_INTEGER	      2
#define FLD_LONG	      3
#define FLD_DATE	      4
#define FLD_MONEY	      5
#define FLD_ZERO_FILL	      6
#define FLD_SPACE_FILL	      7
#define FLD_INTEGER_SPACE     8
#define FLD_INTEGER_ZERO      9
#define FLD_LONG_SPACE	     10
#define FLD_LONG_ZERO	     11

/*  Field read error codes */

#define BAD_DATE      -1000

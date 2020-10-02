/***********************************************************************/
/*                                                                     */
/*    MOUSTRAP.H                                                       */
/*                                                                     */
/*   Author: James M. Curran                                           */
/*   Copyright (c) 1988, James M. Curran                               */
/*                                                                     */
/***********************************************************************/
/* Created : Friday, September  9, 1988 at 10:18 AM                    */
/*                                                                     */
/* Revision: 1.0   Last Revised: Sunday, September 11, 1988 @ 5:39 PM  */
/***********************************************************************/


#define M_Undefined		0

/* Paging Method Types		*/
#define M_Overlaid_Pages	16
#define M_Single_Pages		65535


/*  Page Coordinate Types	*/
#define M_Text_Coord		3
#define M_Graphic_Coord		0

/*  Buttons			*/
#define M_Left			0
#define M_Right			1
#define	M_Center		2
#define M_Middle		2

/*  Cursor direction (for limits)*/
#define	M_HORIZ			1
#define	M_VERT			2


#define MERROR		       -1	/* An error occured (return value) */
#define MNOERROR		0	/* NO error occured (return value) */

/*	Errors (in M_ERROR if return value was MERROR)		  	   */
#define	MNOINIT			1	/* M_Page_Method not initialized   */
#define MNOSPACE		2	/* Insuficent memory to add item   */
#define MTOOMANY		3	/* Attemp to create more than 16 pages */
#define MNOREINIT		4	/* Attempt to execute DEFINE...SYSTEM twice */
#define MNOTPAGE		5	/* Page referenced hasn't been defined */
#define MNOTBUTTON		6	/* Button referenced-- not defined */
#define MNOMOUSE		7	/* There's no mouse attached	   */
#define MBUTTONRET		8	/* Button define with return value */
#define MNOHOTSPOT              9       /* Hot Spot referenced-- not defined*/
#define MNOACTIVE	       10	/* There is no page active	   */
 	

#define	TC(char,fore,back) char+fore*256+back*4096


typedef unsigned	mouse_t;

	struct	M_P_D {
			mouse_t	Page_ID;
			mouse_t	Top;
			mouse_t	Left;
			mouse_t	Right;
			mouse_t	Bottom;
			mouse_t	Coord_Type;
			struct	M_P_D		*Next;	       
			};

#define M_Page_Definition struct M_P_D

	struct	M_B_D {
			mouse_t	Page_ID;
			mouse_t	Button;
			mouse_t	Return_Value;
			struct  M_B_D		*Next;
			};

#define	M_Button_Definition struct M_B_D


	struct	M_HS_D {
			mouse_t	HS_ID;
			mouse_t	Page_ID;
			mouse_t Button;
			mouse_t	Top;
			mouse_t	Left;
			mouse_t	Right;
			mouse_t	Bottom;
			mouse_t	Return_Value;
			struct	M_HS_D		*Next;
			};

#define	M_Hot_Spot_Definition struct M_HS_D




/*	Mouse Control Primatives			*/

extern	mouse_t cdecl Check_Mouse(void);

extern	void 	cdecl Show_Mouse(void);

extern	void 	cdecl Hide_Mouse(void);

extern	mouse_t cdecl Get_Mouse_Position(mouse_t  *, mouse_t  *);

extern	void 	cdecl Set_Mouse_Position(mouse_t X, mouse_t Y);

extern	mouse_t cdecl Get_Mouse_Press(mouse_t Button, mouse_t  *count, \
			 mouse_t  *X, mouse_t  *Y);

extern	mouse_t cdecl Get_Mouse_Release(mouse_t Button, mouse_t  *count, \
			 mouse_t  *X, mouse_t  *Y);

extern	void 	cdecl Set_Mouse_Region(mouse_t Top, mouse_t Left, \
			 mouse_t Bottom, mouse_t Right);

extern	void 	cdecl Set_Mouse_Limits(mouse_t Direction, mouse_t Minimum,\
			 mouse_t Maximum);

extern	void 	cdecl Set_Mouse_Limits_Horiz(mouse_t Left, mouse_t Right);

extern	void    cdecl Set_Mouse_Limits_Vert(mouse_t Top, mouse_t Bottom);

extern	void    cdecl Set_Mouse_Graphic_Cursor(mouse_t Hot_X, mouse_t Hot_Y,\
			 mouse_t Mask[]);
extern	void    cdecl Set_Mouse_Text_Cursor(mouse_t typ, mouse_t p1, \
			 mouse_t p2);

extern	void    cdecl Get_Mickeys(mouse_t *X, mouse_t *Y);



/*	Advanced Mouse Control Routines			*/


extern  mouse_t cdecl Define_Mouse_System(mouse_t Page_Type);

extern  void    cdecl Clear_All_Mouse_Definitions(void );

extern  mouse_t cdecl Add_Mouse_Page(mouse_t Page_Type,mouse_t Top,
			mouse_t Left,mouse_t Bottom,mouse_t Right);

extern  mouse_t cdecl Delete_Mouse_Page(mouse_t Page_ID);

extern  mouse_t cdecl Activate_Mouse_Page(mouse_t Page_ID);

extern  mouse_t cdecl DeActivate_Mouse_Page(mouse_t Page_ID);

extern  mouse_t cdecl Clear_Mouse_Pages(void );

extern  mouse_t cdecl Add_Mouse_Button(mouse_t Page_ID,mouse_t Button,
			mouse_t Return_Value);

extern  mouse_t cdecl Delete_Mouse_Button(mouse_t Page_ID,mouse_t Button);

extern  mouse_t cdecl Add_Mouse_Hot_Spot(mouse_t Page_ID,mouse_t Button,
			mouse_t top, mouse_t left,mouse_t bottom,
			mouse_t right,mouse_t ret_value);

extern  mouse_t cdecl Delete_Mouse_Hot_Spot(mouse_t HS_ID);

extern  mouse_t cdecl Read_Mouse(void );


/*	Global variables available for use in application programs	*/

	extern	mouse_t	M_Paging_Method; /* Overlaid, Single, or Undefined */
	extern	mouse_t	M_Error;	 /* Error Condition		   */
	extern	mouse_t	_mouse_there;	 /* 0=No Mouse, 2 or 3= # of buttons*/



/*	Pre-Defined Graphic Cursor					*/
	extern mouse_t Lightening[];	/* Use Hot-Spot (4,15)		*/
	extern mouse_t Peace[];		/* Use Hot-Spot	(8,8)		*/
	extern mouse_t CrossHair[];	/* Use Hot-spot (8,8)		*/


/*	"Force" automatic use of ?MOUSE.LIB during link			*/

#if M_I86SM
#pragma comment(lib,"SMouse") 
#endif

#if M_I86CM
#pragma comment(lib,"CMouse") 
#endif

#if M_I86MM
#pragma comment(lib,"MMouse") 
#endif

#if M_I86LM
#pragma comment(lib,"LMouse") 
#endif	


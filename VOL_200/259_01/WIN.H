#ifndef WIN_H
#define WIN_H

#define ANSI_PROTO	      1

#ifndef TRUE
#define TRUE		      1
#define FALSE		      0
#endif

#define BLACK		      0x0
#define BLUE		      0x1
#define GREEN		      0x2
#define CYAN		      0x3
#define RED		      0x4
#define MAGENTA 	      0x5
#define YELLOW		      0x6
#define WHITE		      0x7
#define REV_BLACK	      0x0
#define REV_BLUE	      0x10
#define REV_GREEN	      0x20
#define REV_CYAN	      0x30
#define REV_RED 	      0x40
#define REV_MAGENTA	      0x50
#define REV_YELLOW	      0x60
#define REV_WHITE	      0x70
#define HI_INTENSITY	      0x8
#define BLINK		      0x80

#define NO_BOX		      0
#define DBL_LINE_TOP_BOTTOM   1
#define DBL_LINE_SIDES	      2
#define DBL_LINE_ALL_SIDES    3
#define SNGL_LINE_ALL_SIDES   4
#define GRAPHIC_BOX	      5
#define NO_WIND_BORDER	      99

#define VIDEO_MONO	      0
#define VIDEO_CGA	      1
#define VIDEO_EGA	      2
#define VIDEO_VGA	      3

typedef short		      HWND;
typedef unsigned char	      BYTE;
typedef short		      BOOL;


				 /*  WIN.C  */
#if ANSI_PROTO
void	 pascal    WinDrawWindow(short, short, short, short,
				 short, short, short, short);
HWND	 pascal    WinCreateWindow(short, short, short, short,
				   short, short, short, short);
BOOL	 pascal    WinDestroyWindow(HWND);
void	 pascal    WinScrollWindowUp(HWND);
void	 pascal    WinScrollWindowDown(HWND);
void	 pascal    WinSetCursorPos(HWND, short, short);
void	 pascal    WinClearScreen(HWND, short);
void	 pascal    WinTextOut(HWND, char *, short);
void	 pascal    WinCenterText(HWND, short, char *, short);
void	 pascal    WinMoveWindow(HWND, short, short);
short	 pascal    WinGetWindowRow(HWND);
short	 pascal    WinGetWindowCol(HWND);
short	 pascal    WinGetWindowWidth(HWND);
short	 pascal    WinGetWindowHeight(HWND);
short	 pascal    WinGetWindowClr(HWND);
short	 pascal    WinGetWindowBdrClr(HWND);
short	 pascal    WinGetBorderType(HWND);
BOOL	 pascal    WinHideWindow(HWND);
BOOL	 pascal    WinShowWindow(HWND);
void	 pascal    WinInitialize(void);
void	 pascal    WinTerminate(void);
#else
void	 pascal    WinDrawWindow();
HWND	 pascal    WinCreateWindow();
BOOL	 pascal    WinDestroyWindow();
void	 pascal    WinScrollWindowUp();
void	 pascal    WinScrollWindowDown();
void	 pascal    WinSetCursorPos();
void	 pascal    WinClearScreen();
void	 pascal    WinTextOut();
void	 pascal    WinCenterText();
void	 pascal    WinMoveWindow();
short	 pascal    WinGetWindowRow();
short	 pascal    WinGetWindowCol();
short	 pascal    WinGetWindowWidth();
short	 pascal    WinGetWindowHeight();
short	 pascal    WinGetWindowClr();
short	 pascal    WinGetWindowBdrClr();
short	 pascal    WinGetBorderType();
BOOL	 pascal    WinHideWindow();
BOOL	 pascal    WinShowWindow();
void	 pascal    WinInitialize();
void	 pascal    WinTerminate();
#endif

				/*  SCRIO.C  */
#if ANSI_PROTO
short	 pascal    ScrGetRectSize(short, short);
void	 pascal    ScrClearRect(short, short, short, short, short);
void	 pascal    ScrSaveRect(short, short, short, short, char *);
void	 pascal    ScrRestoreRect(short, short, short, short, char *);
void	 pascal    ScrScrollRectUp(short, short, short, short,
				   short, short);
void	 pascal    ScrScrollRectDown(short, short, short, short,
				     short, short);
void	 pascal    ScrSetCursorPos(short, short);
void	 pascal    ScrGetCursorPos(short *, short *);
void	 pascal    ScrCursorOn(void);
void	 pascal    ScrCursorOff(void);
void	 pascal    ScrTextOut(char *, short, short);
void	 pascal    ScrDrawRect(short, short, short, short, short, short);
void	 pascal    ScrInitialize(void);
#else
short	 pascal    ScrGetRectSize();
void	 pascal    ScrClearRect();
void	 pascal    ScrSaveRect();
void	 pascal    ScrRestoreRect();
void	 pascal    ScrScrollRectUp();
void	 pascal    ScrScrollRectDown();
void	 pascal    ScrSetCursorPos();
void	 pascal    ScrGetCursorPos();
void	 pascal    ScrCursorOn();
void	 pascal    ScrCursorOff();
void	 pascal    ScrTextOut();
void	 pascal    ScrDrawRect();
void	 pascal    ScrInitialize();
#endif

#endif


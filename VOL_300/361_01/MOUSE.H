
/* mouse.h  */

#ifndef MOUSE

#define MOUSE
#include <graphics.h>			/* MUST Link w/ Graphic.Lib ON */

/* Define Keys.h Mouse Button Handles for MouseClicked(). */
#define BTN_ANY		 0
#define BTN_L		157
#define BTN_R		170
#define BTN_M		182
#define BTN_LR		184
#define BTN_LM		185
#define BTN_MR		186
#define BTN_LMR		197

/* Define Macros */
#define HideMouse()		(MouseSwitch(0))
#define ShowMouse()		(MouseSwitch(1))

/* Declare ProtoTypes for Any Mouse */
void Mouse(int *A, int *B, int *C, int *D);
int  MouseAtxy(int *X, int *Y);
int  MouseClicked(int Button);
int  MouseClickedxy(int Button, int *X, int *Y);
void MouseDeltaxy(int *X, int *Y);
int  MouseOK(void);
void MousePageSet(int Which1);
int  MouseReSet(void);
void MouseSpeed(int Dx, int Dy, int Zoom);
int  MouseSwitch(int Flag);
void MouseToxy(int X, int Y);
int  MouseTrap(int Left, int Top, int Right, int Bottom);
int  MouseWait4User(void);

#endif


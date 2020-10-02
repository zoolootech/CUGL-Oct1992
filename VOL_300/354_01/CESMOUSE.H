/*  Mouse functions header file  */
/*  Copyright 1989, 1991, Computer Engineering Services */

void showmouse(void);
void hidemouse(void);
void posmouse(int mouseX, int mouseY);
void setmousetrack(int Xtrack, int Ytrack);
void setmousespeed(int speed);
void mousegraphcurs(int hspot, int vspot, int cursor);
void mousetextcurs(int curstype, int scrnmask, int cursmask);
void penemulON(void);
void penemulOFF(void);
void cagemouse(int ulX, int ulY, int lrX, int lrY);
void cagehoriz(int lft, int rgt);
void cagevert(int top, int bot);
void mousewindoff(int ulX, int ulY, int lrX, int lrY);
void savemousedrvr(int *mdrvrbuff);
void restoremousedrvr(int *mdrvrbuff);
void setcrtpage(int page);
void assignmbtn(int btn, int key);
void setlang(int lang);
void enablemdrvr(void);
void setallscales(int Xtrack, int Ytrack, int speed);

int resetmouse(void);
int mousedrvr(void);
int buttons(void);
int lftbutton(void);
int ctrbutton(void);
int rgtbutton(void);
int mousecursX(void);
int mousecursY(void);
int sensemouse(int ulX, int ulY, int lrX, int lrY);
int sensetop(int top);
int sensebottom(int bottom);
int senseleft(int left);
int senseright(int right);
int mousetrackX(void);
int mousetrackY(void);
int getmousespeed(void);
int getmouseBPXinfo(int button);
int getmouseBPYinfo(int button);
int getmouseBRXinfo(int button);
int getmouseBRYinfo(int button);
int mouseXcnt(void);
int mouseYcnt(void);
int getmdrvrstorage(void);
int getcrtpage(void);
int disablemdrvr(void);
int getmversion(void);
int getmtype(void);
int getmirq(void);
int mdrvrreset(void);
int getlang(void);


int lastMX, lastMY, Xcount, Ycount, btnrel, btnpress, mbtns;


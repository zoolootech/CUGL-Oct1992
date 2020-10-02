/*
   HEADER:       ;
   TITLE:        Text Window Support Functions;
   DATE:         07/17/1990;
   DESCRIPTION: "Emulates most text windowing functions of Turbo C 2.0
		 with a few additions. With exception of make_window()
		 functions can all be used in TSR's. Make_window() could
		 be modified not to call malloc() and thus also be used
		 in a TSR.";
   SYSTEM:       MS-DOS;
   FILENAME:     WGCONIO.C;
   WARNINGS:    "Uses int86(), if not supported replace with equivalent.";
   SEE-ALSO:     wgconio.h, wgconio.lib, sample.c, sample.exe;
   AUTHORS:      Bill Giel;
   COMPILERS:    TC;
*/

#include<stdlib.h>
#include<dos.h>
#include<stdarg.h>
#include<process.h>

#define WINLEFT 1
#define WINTOP 1
#define WINBOTTOM 25
#define WINRIGHT 80
#define SCREENHEIGHT 25
#define SCREENWIDTH 80
#define MAX_TEXTBUF 4000
#define TXTLINE_BYTES 160
#define TXTCHAR_BYTES 2


/* Macros for scroll_window and wg_scroll functions*/

#define UP 6
#define DOWN 7


/* Macro to create a far pointer from a segment,offset*/

#define MFP(seg,off) ((void far *)\
		(((unsigned long)(seg) << 16) + (unsigned)(off)))


/*Structure to hold textscreen info*/

struct wgtext_info{
       unsigned char winleft, wintop, winright, winbottom, attribute,
		currmode, screenheight, screenwidth, curx,
		cury, foreground, background;
}WGW;


/*Structure to hold video mode parameters used by the system*/

struct wgvid_parameters{
	unsigned char textline,textchar;
	char far *vid_mem;
}WGV;


/*Union to hold 16-bit keycode*/

union keycode{
	char ch[2];
	int i;
}WGC;


/*Structure for Current Widow Parameters*/

struct wgwind{
	unsigned char foreborder,backborder,foretitle,backtitle,
		foreground,background,leftlim,toplim,rightlim,bottomlim;
};



/*Prototypes*/

void wgwindow		(int left,int top,int right,int bottom);
void wgtextcolor	(int color);
void wgtextbackground	(int color);
void wgtextattr		(int attribute);
void goto_xy		(int x,int y);
void wggotoxy		(int x,int y);
void wgclrscr		(void);
void wgclreol		(void);
int  wggettext		(int left,int top,int right,int bottom,char *buf);
int  wgputtext		(int left,int top,int right,int bottom,char *buf);
int  wgcputs		(char *string);
int  wgputch		(int c);
int  wgwherex		(void);
int  wgwherey		(void);
void wggettextinfo	(struct wgtext_info *inforec);
void initiallize_WGW	(void);
int  get_video_mode	(void);
void get_cursor_pos	(unsigned char *x,unsigned char *y);
int  get_keycode	(void);
int  wggetch		(void);
int  wggetche		(void);
void get_cursor_size	(unsigned char *ch,unsigned char *cl);
int  wgmovetext		(int left,int top,int right,int bottom,
				int newleft,int newtop);
void scroll_window	(unsigned char direct,unsigned char lines,
				unsigned char top,unsigned char left,
				unsigned char bottom,unsigned char right,
				unsigned char attribute);
void wg_scroll		(unsigned char direction);
void box		(int width,int height);
void box2		(int width,int height);
char *make_window	(char *title,struct wgwind w,
				struct wgtext_info *t,int shadow_flag);
void restore_screen	(struct wgwind w, char *text_buf,
				struct wgtext_info t, int shadow_flag);
void cursor_off		(void);
void size_cursor	(unsigned char start,unsigned char end);
int  cdecl wgcprintf	(char *format, ...);
void get_attrib		(unsigned char *attribute,unsigned char *foreground,
						unsigned char *background);
void wgdelline		(void);
void wginsline		(void);
int  make_shadow	(int left,int top,int right,int bottom);
char *make_tsr_window	(char *title,struct wgwind w,
					struct wgtext_info *t,char *buf);
void restore_tsr_screen(struct wgwind w, char *text_buf,
					struct wgtext_info t);

int get_video_mode(void)
{
	union REGS r;

	r.x.ax=0x0F00;
	int86(0x10,&r,&r);
	return(r.x.ax & 0xFF);
}

void get_cursor_pos(unsigned char *x,unsigned char *y)
{
	union REGS r;

	r.h.bh=0;
	r.h.ah=3;
	int86(0x10,&r,&r);
	*y=r.h.dh+1;
	*x=r.h.dl+1;
}

void get_attrib(unsigned char *attribute,unsigned char *foreground,
		unsigned char *background)
{
	union REGS r;
	unsigned char dummy;

	r.h.ah=8;
	r.h.bh=0;
	int86(0x10,&r,&r);

	dummy=r.h.ah;
	*attribute=r.h.ah;
	*background=r.h.ah>>4;
	*foreground=dummy<<4>>4;
}

void initiallize_WGW(void)
{

	WGW.winleft=WINLEFT;
	WGW.wintop=WINTOP;
	WGW.winright=WINRIGHT;
	WGW.winbottom=WINBOTTOM;
	WGW.screenheight=SCREENHEIGHT;
	WGW.screenwidth=SCREENWIDTH;
	WGW.currmode=get_video_mode();
	get_cursor_pos(&WGW.curx,&WGW.cury);
	get_attrib(&WGW.attribute,&WGW.foreground,&WGW.background);

	WGV.textline=TXTLINE_BYTES;
	WGV.textchar=TXTCHAR_BYTES;


	if(WGW.currmode==7)WGV.vid_mem=(char far *)MFP(0xB000,0000);
	else WGV.vid_mem=(char far *)MFP(0xB800,0000);

	if(WGW.currmode<=1){
		WGW.screenwidth=40;
		WGW.winright=40;
		WGV.textchar=2;
		WGV.textline=80;

	}

	WGC.ch[0]=1;
}

void goto_xy(int x,int y)
{
	union REGS r;

	r.h.dh=y-1;
	r.h.dl=x-1;
	r.h.ah=2;
	r.h.bh=0;
	int86(0x10,&r,&r);
}


void wggotoxy(int x,int y)
{
	union REGS r;

	if(y>WGW.winbottom-WGW.wintop+1||y<1)return;
	if(x>WGW.winright-WGW.winleft+1||x<1)return;

	WGW.curx=x;WGW.cury=y;

	r.h.dh=WGW.wintop+WGW.cury-2;
	r.h.dl=WGW.winleft+WGW.curx-2;
	r.h.ah=2;
	r.h.bh=0;
	int86(0x10,&r,&r);
}

void wgwindow(int left,int top,int right,int bottom)
{

	if(left<1||right>WGW.screenwidth||left>right)return;
	if(top<1||bottom>WGW.screenheight||top>bottom)return;

	WGW.winleft=left;
	WGW.wintop=top;
	WGW.winright=right;
	WGW.winbottom=bottom;
	wggotoxy(1,1);
}

void wgclrscr(void)
{
	register i,j;
	char far *v;
	union REGS r;
	extern wg_directvideo;

	if(wg_directvideo==1)
	    for(i=WGW.wintop-1;i<WGW.winbottom;i++)
		for(j=WGW.winleft-1;j<WGW.winright;j++){
			v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
			*v++=' ';
			*v=WGW.attribute;
		}
	else{
			r.h.ah=6;
			r.h.al=0;
			r.h.ch=WGW.wintop-1;
			r.h.cl=WGW.winleft-1;
			r.h.dh=WGW.winbottom-1;
			r.h.dl=WGW.winright-1;
			r.h.bh=WGW.attribute;
			int86(0x10,&r,&r);
	}

	wggotoxy(1,1);
}

void wgtextcolor(int color)
{
	WGW.attribute=(color+(WGW.background<<4));
	WGW.foreground=color;
}

void wgtextbackground(int color)
{
	if(color>7)color-=8;
	WGW.attribute=(WGW.foreground+(color<<4));
	WGW.background=color;
}

void wgtextattr(int attribute)
{
	int dummy;

	dummy=attribute;
	WGW.attribute=attribute;
	WGW.background=dummy>>4;
	WGW.foreground=attribute<<4>>4;
}

void wgclreol(void)
{
	register j;
	int y;
	char far *v;
	union REGS r;
	extern wg_directvideo;

	y=WGW.wintop+WGW.cury-2;

	if(wg_directvideo==1)
		for(j=WGW.winleft+WGW.curx-2;j<WGW.winright;j++){
			v=WGV.vid_mem+y*WGV.textline+j*WGV.textchar;
			*v++=' ';
			*v=WGW.attribute;
		}
	else for(j=WGW.winleft;j<=WGW.winright;j++){
			goto_xy(j,WGW.cury);
			r.h.ah=9;
			r.h.bh=0;
			r.x.cx=1;
			r.h.al=' ';
			r.h.bl=WGW.attribute;
			int86(0x10,&r,&r);
	}
}

int wggettext(int left,int top,int right,int bottom,char *buf)
{
	register i,j;
	char far *v;
	char *buf_ptr;
	union REGS r;
	extern wg_directvideo;

	if(left<1||right>WGW.screenwidth||left>right)return 0;
	if(top<1||bottom>WGW.screenheight||top>bottom)return 0;

	buf_ptr=buf;

	if(wg_directvideo==1)
		for(i=top-1;i<bottom;i++)
			for(j=left-1;j<right;j++){
				v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
				*buf_ptr++=*v++;
				*buf_ptr++=*v;
			}
	else for(i=top;i<=bottom;i++)
			for(j=left;j<=right;j++){
				goto_xy(j,i);
				r.h.ah=8;
				r.h.bh=0;
				*buf_ptr++=int86(0x10,&r,&r);
				*buf_ptr++=r.h.ah;
	}
	return 1;
}

int wgputtext(int left,int top,int right,int bottom,char *buf)
{
	register i,j;
	char far *v;
	char *buf_ptr;
	union REGS r;
	extern wg_directvideo;

	if(left<1||right>WGW.screenwidth||left>right)return 0;
	if(top<1||bottom>WGW.screenheight||top>bottom)return 0;

	buf_ptr=buf;

	if(wg_directvideo==1)
		for(i=top-1;i<bottom;i++)
			for(j=left-1;j<right;j++){
				v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
				*v++=*buf_ptr++;
				*v=*buf_ptr++;
			}
	else for(i=top;i<=bottom;i++)
			for(j=left;j<=right;j++){
				goto_xy(j,i);
				r.h.ah=9;
				r.h.bh=0;
				r.x.cx=1;
				r.h.al=*buf_ptr++;
				r.h.bl=*buf_ptr++;
				int86(0x10,&r,&r);
	}
	return 1;
}

int wgmovetext(int left,int top,int right,int bottom,int newleft,int newtop)
{
	char far *v,*p;
	char buf[MAX_TEXTBUF];
	register i,j;
	int max_i,max_j;
	union REGS r;
	unsigned char attribute,foreground,background;
	extern wg_directvideo;

	if(left<1||right>WGW.screenwidth||left>right)return 0;
	if(top<1||bottom>WGW.screenheight||top>bottom)return 0;
	if(newleft<1||newleft+right-left>WGW.screenwidth)return 0;
	if(newtop<1||newtop+bottom-top>WGW.screenheight)return 0;

	p=buf;

	if(wg_directvideo==1)
		for(i=top-1;i<bottom;i++)
		   for(j=left-1;j<right;j++){
			v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
			*p++=*v++;
			*p++=*v;
		   }
	else for(i=top;i<=bottom;i++)
			for(j=left;j<=right;j++){
				goto_xy(j,i);
				get_attrib(&attribute,&foreground,&background);
				r.h.ah=8;
				r.h.bh=0;
				*p++=int86(0x10,&r,&r);
				*p++=attribute;
	}

	p=buf;
	max_i=newtop+bottom-top;
	max_j=newleft+right-left;

	if(wg_directvideo==1)
		for(i=newtop-1;i<max_i;i++)
		   for(j=newleft-1;j<max_j;j++){
			v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
			*v++=*p++;
			*v=*p++;
		   }
	else for(i=newtop;i<=max_i;i++)
			for(j=newleft;j<=max_j;j++){
				goto_xy(j,i);
				r.h.ah=9;
				r.h.bh=0;
				r.x.cx=1;
				r.h.al=*p++;
				r.h.bl=*p++;
				int86(0x10,&r,&r);
	}
	return 1;
}


void scroll_window(unsigned char direct,unsigned char lines,
	unsigned char top,unsigned char left,unsigned char bottom,
	unsigned char right,unsigned char attribute)
{
	union REGS r;

	r.h.ah=direct;   /* Direction of Scroll     */
	r.h.al=lines;    /* No. of Lines To Scroll  */
	r.h.ch=top-1;    /* Absolute Window top     */
	r.h.cl=left-1;   /*    "   Window left      */
	r.h.dh=bottom-1; /*    "   Window bottom    */
	r.h.dl=right-1;  /*    "   Window right     */
	r.h.bh=attribute;/* Color for blank line(s) */
	int86(0x10,&r,&r);
}

void wg_scroll(unsigned char direct)
{
	scroll_window(direct,1,WGW.wintop,WGW.winleft,
		WGW.winbottom,WGW.winright,WGW.attribute);
}

int wgcputs(char *string)
{
	int startx,endx,y;
	char far *v;
	char *p;
	union REGS r;
	extern wg_directvideo;

	p=string;
	y=WGW.wintop+WGW.cury-2;
	startx=WGW.winleft+WGW.curx-2;
	endx=WGW.winright;
	while(*p){
		if(*p!=10&&*p!=13){
			if(wg_directvideo==1){
			     v=WGV.vid_mem+y*WGV.textline+(startx++)*WGV.textchar;
			     *v++=*p++;
			     *v=WGW.attribute;
			}
			else{
			     goto_xy((startx++)+1,y+1);
			     r.h.ah=9;
			     r.h.bh=0;
			     r.x.cx=1;
			     r.h.al=*p++;
			     r.h.bl=WGW.attribute;
			     int86(0x10,&r,&r);
			}
		}
		if(startx==endx||*p==10||*p==13){
			if(*p==10||startx==endx)y++;
			if(y==WGW.winbottom){
				wg_scroll(UP);
				y--;
			}
			if(*p==13||startx==endx)startx=WGW.winleft-1;
			if(*p==10||*p==13)p++;
		}
	}
	WGW.cury=y-WGW.wintop+2;WGW.curx=startx-WGW.winleft+2;
	wggotoxy(WGW.curx,WGW.cury);
	--p;
	return(*p);
}

int wgputch(int c)
{
	int startx,endx,y;
	char far *v;
	union REGS r;
	extern wg_directvideo;

	y=WGW.wintop+WGW.cury-2;
	startx=WGW.winleft+WGW.curx-2;
	endx=WGW.winright;

	if(c!=10&&c!=13){
		if(wg_directvideo==1){
		     v=WGV.vid_mem+y*WGV.textline+(startx++)*WGV.textchar;
		     *v++=c;
		     *v=WGW.attribute;
		}
		else{
		     goto_xy((startx++)+1,y+1);
		     r.h.ah=9;
		     r.h.bh=0;
		     r.x.cx=1;
		     r.h.al=c;
		     r.h.bl=WGW.attribute;
		     int86(0x10,&r,&r);
		}
	}
	if(startx==endx||c==10||c==13){
		if(c==10||startx==endx)y++;
		if(y==WGW.winbottom){
			wg_scroll(UP);
			y--;
		}
		if(c==13||startx==endx)startx=WGW.winleft-1;
	}
	WGW.cury=y-WGW.wintop+2;WGW.curx=startx-WGW.winleft+2;
	wggotoxy(WGW.curx,WGW.cury);
	return c;
}

int wgwherex(void)
{
	return(WGW.curx);
}

int wgwherey(void)
{
	return(WGW.cury);
}

void wggettextinfo(struct wgtext_info *textinfo)
{
	*textinfo=WGW;
}

int get_keycode(void)
{
	union REGS r;

	r.h.ah=0;
	int86(0x16,&r,&r);
	return r.x.ax;

}

int wggetch(void)
{
	if(WGC.ch[0]==0){
		WGC.ch[0]=1;
		return(WGC.ch[1]);
	}
	WGC.i=get_keycode();
	return(WGC.i & 255);
}

int wggetche(void)
{
	char ch;
	if((ch=wggetch())!=0)wgputch(ch);
	return ch;
}

void get_cursor_size(unsigned char *ch,unsigned char *cl)
{
	union REGS r;

	r.h.ah=3;
	r.h.bh=0;
	int86(0x10,&r,&r);

	*ch=r.h.ch;
	*cl=r.h.cl;
}

void box(int width,int height)
{
		register i;
                int upperleftx=wgwherex();
                int upperlefty=wgwherey();

		wgputch(201);
		for(i=1;i<width-1;++i)wgputch(205);
		wgputch(187);
                for(i=upperlefty+1;i<upperlefty+height-1;++i){
                        wggotoxy(upperleftx,i);
			wgputch(186);
                        wggotoxy(upperleftx+width-1,i);
			wgputch(186);
		}
                wggotoxy(upperleftx,i);
		wgputch(200);
		for(i=1;i<width-1;++i)wgputch(205);
		wgputch(188);
}

void box2(int width,int height)
{
		register i;
                int upperleftx=wgwherex();
                int upperlefty=wgwherey();

		wgputch(218);
		for(i=1;i<width-1;++i)wgputch(196);
		wgputch(191);
                for(i=upperlefty+1;i<upperlefty+height-1;++i){
                        wggotoxy(upperleftx,i);
			wgputch(179);
                        wggotoxy(upperleftx+width-1,i);
			wgputch(179);
		}
                wggotoxy(upperleftx,i);
		wgputch(192);
		for(i=1;i<width-1;++i)wgputch(196);
		wgputch(217);
}

char *make_window(char *title,struct wgwind w,
			struct wgtext_info *t,int shadow_flag)
{
	char *buf_ptr;
	int v_offset=0,h_offset=0,v2=0,h2=0;

	if(w.leftlim<1||w.rightlim>WGW.screenwidth||w.leftlim>w.rightlim)
								return 0;
	if(w.toplim<1||w.bottomlim>WGW.screenheight||w.toplim>w.bottomlim)
								return 0;

	if(shadow_flag){
		v_offset=v2=1;
		h_offset=h2=2;
		while(w.rightlim+h_offset>WGW.screenwidth)h_offset--;
		while(w.bottomlim+v_offset>WGW.screenheight)v_offset--;
	}
	wggettextinfo(t);
	buf_ptr=(char *)malloc((w.rightlim-w.leftlim+1+h_offset)
				*(w.bottomlim-w.toplim+1+v_offset)*2);
	if(!buf_ptr)return(0);
	wggettext(w.leftlim,w.toplim,
		w.rightlim+h_offset,w.bottomlim+v_offset,buf_ptr);
	if(shadow_flag)make_shadow(w.leftlim+h2,w.toplim+v2,
			w.rightlim+h_offset,w.bottomlim+v_offset);
	wgwindow(w.leftlim,w.toplim,w.rightlim,w.bottomlim);
	wgtextbackground(w.backborder);wgtextcolor(w.foreborder);
	wgclrscr();wggotoxy(2,1);
	box(w.rightlim-w.leftlim-1,w.bottomlim-w.toplim+1);
	if(*title){
		wggotoxy((w.rightlim-w.leftlim-strlen(title)-2)/2+3,1);
		wgtextcolor(w.foretitle);wgtextbackground(w.backtitle);
		wgcputs(title);
	}
	wgwindow(w.leftlim+2,w.toplim+1,w.rightlim-2,w.bottomlim-1);
	wgtextcolor(w.foreground);wgtextbackground(w.background);wgclrscr();
	return(buf_ptr);
}


void restore_screen(struct wgwind w, char *text_buf,struct wgtext_info t,
			int shadow_flag)
{
	int v_offset=0,h_offset=0;

	if(shadow_flag){
		v_offset=1;
		h_offset=2;
		while(w.rightlim+h_offset>WGW.screenwidth)h_offset--;
		while(w.bottomlim+v_offset>WGW.screenheight)v_offset--;
	}

	if(text_buf==NULL)return;
	wgputtext(w.leftlim,w.toplim,w.rightlim+h_offset,
					w.bottomlim+v_offset,text_buf);
	free(text_buf);
	wgwindow(t.winleft,t.wintop,t.winright,t.winbottom);
	wgtextcolor(t.foreground);wgtextbackground(t.background);
	wggotoxy(t.curx,t.cury);
}

void cursor_off(void)
{
	union REGS r;
	r.h.ah=1;
	r.h.ch=0x20;
	int86(0x10,&r,&r);
}


void size_cursor(unsigned char start,unsigned char end)
{
	union REGS r;
	r.h.ah=1;
	r.h.ch=start;
	r.h.cl=end;
	int86(0x10,&r,&r);
}

int cdecl wgcprintf(char *format, ...)
{
	char dummy_str[255];
	int rtn_val;
	va_list aptr;

	va_start(aptr,format);
	rtn_val=vsprintf(dummy_str,format,aptr);
	va_end(aptr);
	wgcputs(dummy_str);
	return(rtn_val);
}

void wgdelline(void)
{
	scroll_window(UP,1,wgwherey()+WGW.wintop-1,WGW.winleft,
		WGW.winbottom,WGW.winright,WGW.attribute);
	wggotoxy(1,wgwherey());
}

void wginsline(void)
{
	scroll_window(DOWN,1,wgwherey()+WGW.wintop-1,WGW.winleft,
		WGW.winbottom,WGW.winright,WGW.attribute);
	wggotoxy(1,wgwherey());
}

int make_shadow(int left,int top,int right,int bottom)
{
	register i,j;
	char far *v;
	union REGS r;
	extern wg_directvideo;

	if(left<1||right>WGW.screenwidth||left>right)return 0;
	if(top<1||bottom>WGW.screenheight||top>bottom)return 0;


	if(wg_directvideo==1){
		for(i=top-1;i<bottom-1;i++)
			for(j=right-2;j<right;j++){
				v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
				v++;*v=8;
			}

			for(j=left-1;j<right;j++){
				v=WGV.vid_mem+i*WGV.textline+j*WGV.textchar;
				v++;*v=8;
			}
	}
	else{
		 for(i=top;i<=bottom-1;i++)
			for(j=right-1;j<=right;j++){
				goto_xy(j,i);
				r.h.ah=8;
				r.h.bh=0;
				r.h.al=int86(0x10,&r,&r);
				r.h.ah=9;
				r.h.bh=0;
				r.x.cx=1;
				r.h.bl=8;
				int86(0x10,&r,&r);
			}
			for(j=left;j<=right;j++){
				goto_xy(j,i);
				r.h.ah=8;
				r.h.bh=0;
				r.h.al=int86(0x10,&r,&r);
				r.h.ah=9;
				r.h.bh=0;
				r.x.cx=1;
				r.h.bl=8;
				int86(0x10,&r,&r);
			}
	}
	return 1;
}

char *make_tsr_window(char *title,struct wgwind w,
			struct wgtext_info *t,char *buf)
{

	if(w.leftlim<1||w.rightlim>WGW.screenwidth||w.leftlim>w.rightlim)
								return 0;
	if(w.toplim<1||w.bottomlim>WGW.screenheight||w.toplim>w.bottomlim)
								return 0;

	wggettextinfo(t);
	wggettext(w.leftlim,w.toplim,w.rightlim,w.bottomlim,buf);
	wgwindow(w.leftlim,w.toplim,w.rightlim,w.bottomlim);
	wgtextbackground(w.backborder);wgtextcolor(w.foreborder);
	wgclrscr();wggotoxy(2,1);
	box(w.rightlim-w.leftlim-1,w.bottomlim-w.toplim+1);
	if(*title){
		wggotoxy((w.rightlim-w.leftlim-strlen(title)-2)/2+2,1);
		wgtextcolor(w.foretitle);wgtextbackground(w.backtitle);
		wgcputs(title);
	}
	wgwindow(w.leftlim+2,w.toplim+1,w.rightlim-2,w.bottomlim-1);
	wgtextcolor(w.foreground);wgtextbackground(w.background);wgclrscr();
	return(buf);
}

void restore_tsr_screen(struct wgwind w, char *text_buf,struct wgtext_info t)
{

	if(text_buf==NULL)return;
	wgputtext(w.leftlim,w.toplim,w.rightlim,w.bottomlim,text_buf);
	wgwindow(t.winleft,t.wintop,t.winright,t.winbottom);
	wgtextcolor(t.foreground);wgtextbackground(t.background);
	wggotoxy(t.curx,t.cury);
}
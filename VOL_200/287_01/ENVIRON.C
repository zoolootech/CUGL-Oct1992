#include <stdio.h>
#include <gds.h>
#include <GRADENV.h>

#define MAX_ENVSLOT 10 

struct grad_env {
    int status;
    int winx1, winy1, winx2, winy2;
    int orgx, orgy;
    int style, plottype, frame_nu, font_nu;
};

static struct grad_env ENV_STACK[MAX_ENVSLOT]= {
  { PERMANENT, 0, 0, MAX_XSCN, MAX_YSCN, 0, 0, 0xffff, 0, 0, 0 }
};

EnvSave(slot_nu)
int slot_nu;
{
    register struct grad_env *ptr;

    if ((slot_nu<0) || (slot_nu>=MAX_ENVSLOT)) {
        graderror(2,1,slot_nu);
        slot_nu=0;
    }
    if (slot_nu==0) {
        if ((slot_nu=find_envs())<=0) return(0); 
    }
    ptr=&ENV_STACK[slot_nu];
    ptr->status=USED;
    ptr->winx1=WINX1;
    ptr->winy1=WINY1;
    ptr->winx2=WINX2;
    ptr->winy2=WINY2;
    ptr->orgx=ORGX;
    ptr->orgy=ORGY;
    ptr->style=STYLE;
    ptr->plottype=CUR_PLOT;
    ptr->frame_nu=CUR_FRAME;
    ptr->font_nu=CURFONT;
    return(slot_nu);
}

static find_envs()
{
    register int loop;

    for (loop=1; loop<MAX_ENVSLOT; loop++) {
        if (ENV_STACK[loop].status==NOT_USED)
            return(loop);
    }
    return(-1);
}

EnvRsto(slot_nu,flag)
unsigned int slot_nu;
register unsigned flag;
{
    register struct grad_env *ptr;

    
    if ((slot_nu>=MAX_ENVSLOT)) {
        graderror(4,2,slot_nu);
    }
    ptr=&ENV_STACK[slot_nu];
    if (ptr->status == NOT_USED) {
        graderror(2,3,slot_nu);
    }
    if (!(flag & KEEP_FRAME)) {
        SelectFrame(ptr->frame_nu);
    }
    if (!(flag & KEEP_WIN)) {
        SetWin(ptr->winx1-ORGX, ptr->winy1-ORGY,
               ptr->winx2-ORGX, ptr->winy2-ORGY);
    }
    if (!(flag & KEEP_ORG)) {
        SetOrg(ptr->orgx,ptr->orgy);
    }
    if (!(flag & KEEP_STYLE)) {
        SetStyle(ptr->style);
    }
    if (!(flag & KEEP_PLOTTYPE)) {
        PlotType(ptr->plottype);
    }
    if (!(flag & KEEP_FONT)) {
        SelectFont(ptr->font_nu);
    }
    if (!(flag & KEEP_SLOT) && (ptr->status != PERMANENT)) {
        ptr->status=NOT_USED;
    }
}


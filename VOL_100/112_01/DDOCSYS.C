/*  TAB s5,4
 *
 *  system dependant functions for diskdoc
 *
 *  required are:
 *  botmem,topmem,sysok,conin,conout,const
 *  seldrv,lnext,read,write,rstdrv
 */

/*
 *  this version is for small C and cp/m 8080 version 2.x.
 *  since bios must be used for sector read and write,
 *  character i/o functions use bios directly too.
 *  cp/m "compatible" operating systems will probably
 *  need to have these functions rewritten.
 */

char *xlt;	/* sector xlate table, used by cp/m version */

/*
 *  return pointer to bottom of free memory
 */

int lastglobal;     /* dirty trick, will work in small-c */

botmem()
{   return &lastglobal;
}

/*
 *  top of free memory
 *  remember that the stack needs some space too
 */

topmem()
{   char *p,topofstack;
    p=&topofstack;
    return p-300;
}

/*
 *  return true if enviroment seems to be ok
 */

sysok()
{   char *p;
#asm
    mvi     c,12	;check version number
    call    5
    ani     0f0h
    cpi     20h 	;ver 2.x?
    jnz     nogood
    mov     a,h
    ora     a		;cp/m?
    jz	    ok
nogood:
    lxi     h,0 	;false if no good
    pop     d
    ret
ok:
#endasm
    p=1;
    return (*p==3);    /* check if xsub or despool is active */
}

/*
 *  return console status, true if ready
 */

const()
{   return bios(2,0);
}

/*
 *  get character from console, no echo
 */

conin()
{   return bios(3,0);
}

/*
 *  put character to console
 *  no character conversion should be performed
 */

conout(ch)
    char ch;
{   bios(4,ch);
}

/*
 *  select drive, drive name is 'a','b' etc.
 *  set values for track, sector counts and first sector
 *  return true if ok
 *  to find out what this version does, 
 *  refer to the cp/m 2.0 alteration guide
 */

seldrv(drv,pt,ps,pf)
    char drv;
    int *pt,*ps,*pf;	    /* where to put track, sector and firstsector */
{   int *dph,*spt,*dsm,*off,halfsecs,trks;
    char *dpb,*bls;
    if ((dph=bios(9,drv-'a'))==0) return 0;
    xlt=dph[0]; 	    /* look at disk parameter header */
    dpb=dph[5];
    if (xlt) *pf=xlt[0];
    else *pf=255&bios(16,0);
    spt=&dpb[0]; *ps=*spt;  /* and at disk parameter block too */
    bls=&dpb[2];
    dsm=&dpb[5];
    off=&dpb[13];
    /* this is tricky since unsigned divide isn't supported */
    halfsecs=(*dsm+1)<<(*bls-1);
    trks=((halfsecs/(*spt))*2)+(((halfsecs%(*spt))+(*spt-1))/(*spt));
    *pt=trks+*off;
    return 1;
}

/*
 *  return next logical track/sector
 *  replace by a call to next() if not required
 *  cp/m version sets sector number according to translate table
 */

lnext(t,s)
    int *t,*s;
{   int l;
    if (xlt) {
	l=0;
	while (xlt[l++]!=*s);
	if (l>=sectors) {
	    nextt(t);
	    l=0;
	}
	*s=xlt[l];
    } else next(t,s);
}

/*
 *  reset the currently selected drive
 *  used prior to any other operation on a drive
 *  no error code is returned
 */

rstdrv()
{   bios(8,0);
}

/*
 *  read sector, false if error
 */

read(trk,sec,adr)
    int trk,sec;
    char *adr;
{   bios(10,trk);
    bios(11,sec);
    bios(12,adr);
    if (bios(13,1)) return 0;
    return 1;
}

/*
 *  write sector, false if error
 */

write(trk,sec,adr)
    int trk,sec;
    char *adr;
{   bios(10,trk);
    bios(11,sec);
    bios(12,adr);
    if (bios(14,1)) return 0;
    return 1;
}

/*
 *  bios call
 *  will only work with the original small C
 *  other C compilers will usually have the argument sequence reversed
 */

bios(fun,arg)
    int fun,arg;
{   char *ofs;
    ofs=(fun-1)*3;
#asm
    pop     d		;ofs
    pop     h		;ret
    pop     b		;arg
    push    b
    push    h
    push    d
    lhld    1		;get pointer to bios
    dad     d		;add offset
    lxi     d,retn1
    push    d
    mov     d,b 	;arg in de too
    mov     e,c
    pchl		;go
retn1:
    xchg
    mov     l,a
    mvi     h,0
    pop     b		;ofs
    push    b
    mov     a,c
    cpi     (9-1)*3	;select disk function?
    jz	    retn2
    cpi     (16-1)*3	;sector translate?
    jnz     retn3
retn2:
    xchg		;value came in hl
retn3:
#endasm
}

/*
 *  some small C library functions need redefinition
 */

#asm
ccgo:
    ret
;
qzexit:
    jmp     0
#endasm

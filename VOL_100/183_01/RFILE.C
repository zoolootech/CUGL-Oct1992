/*   These functions are for Random File Access and are written in
*    Lattice `C'.  The file "ALL.H" contains the following
*    definitions:    O_RDWR = 2
*                   O_CREAT = 0x0100
*                     O_RAW = 0x8000
*
*    When a file is open [ropen()] YOU MUST KEEP TRACK OF THE HI RECORD
*    NUMBER (endr).
*/

/*   RFILE.C  Random file access functions  */

#include "ALL.H"

int ropen(filspc)   /*  open file for random access  */

char *filspc;

{ int port;

  port=open(filspc,O_RDWR | O_CREAT | O_RAW);
  return(port);  /*  port<0 = error  */
}


int rfile(port,rec,data,sz,mode)     /*  random file access         */
                                  /*   assumes port assigned     */
int port,rec;
int sz;          /*  sizeof(*data)  */
char *data;      /*  random record structure  */
char mode;       /*  r = read, w = write  */

{ int stat,read(),write();
  long pos,lseek();

  stat=0;
  rec-=1;
  pos=rec*sz;
  lseek(port,pos,0);
  switch(mode)   {  case'r':stat=read(port,data,sz);
                            break;
                    case'w':stat=write(port,data,sz);
                            break;
                 };
  if(stat<=0) return(stat-1);   /* -1=EOF, <(-1)=error */
  else return(rec+1);
}


int hirec(port,sz)   /*  random record count  */

int port;
int sz;       /*  sizeof record  */

{ int ct;
  long ef,lseek();

  ef=lseek(port,0L,2);
  ef+=1;
  ct=ef/sz;
  return(ct);
}


int rclose(port,sz,endr)    /*  random file close  */

int port,endr;      /*  file pointer & end-record  */
int sz;             /*  size of record  */

{ long pos,er,lseek();
  int r;

  pos=sz*endr+1;
  er=lseek(port,pos,0);
  close(port);
  r=er;
  return(r);      /*  err<0 = error  */
}

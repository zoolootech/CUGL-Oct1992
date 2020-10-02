#include stdio.h
/*
** xtoi -- convert hex string to integer nbr
**         returns field size, else ERR on error
*/
  static int xd, t;

xtoi(hexstr, nbr)  char *hexstr;  int *nbr;  {
  xd = 0;
  *nbr=0;
  while(1)
    {
    if((*hexstr>='0')&(*hexstr<='9')) t=48;
    else if((*hexstr>='A')&(*hexstr<='F')) t=55;
    else if((*hexstr>='a')&(*hexstr<='f')) t=87;
    else break;
    if(xd<4) ++xd; else return ERR;
    *nbr=*nbr<<4;
    *nbr=*nbr+(*hexstr++)-t;
    }
  return xd;
  }

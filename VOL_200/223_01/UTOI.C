#include stdio.h
/*
** utoi -- convert unsigned decimal string to integer nbr
**          returns field size, else ERR on error
*/
  static int xd,t;

utoi(decstr, nbr)  char *decstr;  int *nbr;  {
  xd=0;
  *nbr=0;
  while((*decstr>='0')&(*decstr<='9')) {
    t=*nbr;t=(10*t) + (*decstr++ - '0');
    if ((t>=0)&(*nbr<0)) return ERR;
    xd++; *nbr=t;
    }
  return xd;
  }

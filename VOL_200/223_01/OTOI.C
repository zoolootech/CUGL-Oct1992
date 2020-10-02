#include stdio.h
/*
** otoi -- convert unsigned octal string to integer nbr
**          returns field size, else ERR on error
*/
   static int xd,t;

otoi(octstr, nbr)  char *octstr;  int *nbr;  {
  xd=0;
  *nbr=0;
  while((*octstr>='0')&(*octstr<='7')) {
    t=*nbr;
    t=(t<<3) + (*octstr++ - '0');
    if ((t>=0)&(*nbr<0)) return ERR;
    xd++; *nbr=t;
    }
  return xd;
  }

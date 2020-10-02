/*
** atoi -- convert signed decimal (ASCII) string to integer
*/
atoi(nptr)  char *nptr; {
  int atoi, sign;
  atoi = sign = 0;
  while(*nptr == '\t' || *nptr == ' ') nptr++;
  if(*nptr == '-') {
    sign = 1;
    nptr++;
    }
  while(*nptr >= '0' && *nptr <= '9') atoi = (10 * atoi) + (*nptr++ - '0');
  if(sign) return -atoi;
  else return atoi;
  }

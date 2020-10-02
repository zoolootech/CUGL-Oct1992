 
/*
** optimize strings beginning "\tJxx $+5\n" for "&&" and "||"
** ptr addresses 1st byte BEYOND jump
** jumpinst is "Jxx $+" (null terminated, no 5, \n, or \t)
** len is length of jump
** designed to be called from other jump optimizers
*/
optim9(ptr, jumpinst) char *ptr, jumpinst[]; {
  char label1[20], label2[20], label3[20], *ptr1, *ptr2;
  int bump, len1, len2, len3, match;
  len1 = len2 = len3 = match = 0;
  if(streq(ptr, "\tJMP CC")) {
    bump = 7;
    ptr1 = ptr + bump;
    ptr2 = label1;
    while((*ptr2++ = *ptr1++) > ' ') ++len1; /* save label #1 */
    *--ptr2 = 0;
    bump += len1 + 1;
    if(streq(ptr+bump, "\tMOV AX,1\n\tJMP CC")) {
      bump += 17;
      ptr1 = ptr + bump;
      ptr2 = label2;
      while((*ptr2++ = *ptr1++) > ' ') ++len2; /* save label #2 */
      *--ptr2 = 0;
      bump += len2 + 1;
      if(streq(ptr+bump, "CC") && streq(ptr+bump+2, label1) &&
          streq(ptr+bump+len1+2, ":\n\tMOV AX,0\nCC") &&
          streq(ptr+bump+len1+16, label2) &&
          streq(ptr+bump+len1+len2+16, ":\n\tAND AX,AX\n\tJNZ $+5\n\tJMP CC")) {
        bump += len1 + len2 + 45;
        ptr1 = ptr + bump;
        ptr2 = label3;
        while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save label #3 */
        *--ptr2 = 0;
        ++bump;
        ot(jumpinst);
        outstr("8");
        nl();
        outstr("CC");
        outstr(label1);
        outstr(":");
        nl();
        ol("MOV AX,0");
        ot("JMP CC");
        outstr(label3);
        nl();
        ol("MOV AX,1 ;optim9 - 1");
        ptr += bump;
        match = 1;
        }
      }
    else if(streq(ptr+bump, "\tMOV AX,0\n\tJMP CC")) {
      bump += 17;
      ptr1 = ptr + bump;
      ptr2 = label2;
      while((*ptr2++ = *ptr1++) > ' ') ++len2; /* save label #2 */
      *--ptr2 = 0;
      bump += len2 + 1;
      if(streq(ptr+bump, "CC") && streq(ptr+bump+2, label1) &&
          streq(ptr+bump+len1+2, ":\n\tMOV AX,1\nCC") &&
          streq(ptr+bump+len1+16, label2) &&
          streq(ptr+bump+len1+len2+16, ":\n\tAND AX,AX\n\tJNZ $+5\n\tJMP CC")) {
        bump += len1 + len2 + 45;
        ptr1 = ptr + bump;
        ptr2 = label3;
        while((*ptr2++ = *ptr1++) > ' ') ++bump; /* save label #3 */
        *--ptr2 = 0;
        ++bump;
        ot(jumpinst);
        outstr("5");
        nl();
        ot("JMP CC");
        outstr(label1);
        nl();
        ol("MOV AX,0");
        ot("JMP CC");
        outstr(label3);
        nl();
        outstr("CC");
        outstr(label1);
        outstr(":");
        nl();
        ol("MOV AX,1 ;optim9 - 2");
        ptr += bump;
        match = 1;
        }
      }
    }
  if (!match) {ot(jumpinst); outstr("5"); nl();}
  return ptr;
  }

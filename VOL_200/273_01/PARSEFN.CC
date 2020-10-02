parse_fn(char *fspec,char *fdrive, char *fpath, char *fn, char *fe)
/* This function will parse and break apart the filespec pointed to
   by *fspec.	It will pass back the drive, directory, filename, extension.
   If a given part does not exist a NULL character string will be passed
   back.
*/
{
        char work[85], *fpt, ch;
        int slash_found=0;
    *fpath=0x00; *fn=0x00; *fe=0x00;
    *fdrive=0x00;
    strcpy(work,fspec);
    if(work[1]==':') {
        toupper(work[0]);
        *fdrive=work[0];
        strcpy(&work[0],&work[2]);
        }
    fpt=work + strlen(work) - 1;
    while(fpt >= work) {
       if(*fpt=='.') {
            strcpy(fe,fpt+1);
            *fpt=0x00;
            fpt--;
       }
       else {
               if(*fpt=='\\') {
                   slash_found++;
                   if(slash_found==1) {
                       strcpy(fn,fpt+1);
                       *fpt=0x00;
                       fpt--;
                   }
                   else
                   if(fpt==work) {
                       strcpy(fpath,fpt);
                       fpt--;
                   }
                   else
                       fpt--;
               }
               else
                   fpt--;
                }
    }

    if(slash_found==1)
        strcpy(fpath,"\\");
    if(slash_found==0)
        strcpy(fn,work);
    upcase(fpath);
    upcase(fn);
    upcase(fe);
}

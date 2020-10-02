/*--------- DEL -- Version 1.1 --- 06 MAR 80 -- BDS C v1.2 ---------*/
/*                                                                  */
/*  DEL is a directory maintenance utility that provides a means    */
/*  for selectively erasing files from a CP/M disk directory.       */
/*  It is safer to use than the standard CP/M command ERA because   */
/*  the user must confirm a deletion by answering a prompt of the   */
/*  form                                                            */
/*                                                                  */
/*        DELETE FILE .... ? -- (Y/N/^C):                           */
/*                                                                  */
/*  If the response made to the prompt is "y" (upper or lower       */
/*  lower case), DEL deletes the file.  If the response is any-     */
/*  thing else, DEL retains the file.  A response of control-C      */
/*  (^C) terminates DEL.  All other responses cause DEL to con-     */
/*  tinue with the directory scan.                                  */
/*                                                                  */
/*  The execution syntax for DEL is the same as ERA, that is,       */
/*                                                                  */
/*       DEL <afn>                                                  */
/*                                                                  */
/*  where <afn> is a CP/M ambiguous file name.  The syntax for an   */
/*  <afn> is                                                        */
/*                                                                  */
/*       <afn> = [<drv>:]{<file> |*}.{<type> | *}                   */
/*                                                                  */
/*       <drv> is a drive designator (A, B, C, or D).               */
/*       <file> is a one-to-eight-character file name.              */
/*       <type> is a zero-to-three-character file type.             */
/*                                                                  */
/*  As usual [ ... ] indicates an optional parameter and            */
/*  { ... | ... } indicates that a choice of parameter is to be     */
/*  made.  The question mark (?) is allowed as a wild-card          */
/*  character in the <file> and <type> parameters.                  */
/*                                                                  */
/*------------------------------------------------------------------*/
 
#define     FIRST       17 
#define     NEXT        18 
#define     DELETE      19 
#define     DRIVE       25 
#define     ALL         255 
#define     STDFCB      0x005C 
#define     STDBFR      0x0080 
 
main()
    {
    char i, j, c, rq, rtn, skip, skp;
    char *arg, *dirp, file[15], fcb[33];
    int  lns, chrs;
    /* Move the standard FCB to a safe place. */
    movblk(STDFCB, fcb, 32);
    fcb[32] = '\0';
    /* If files are to be deleted on a different drive, */
    /* make allowance for this.                         */
    arg = STDBFR;
    if (arg[3] == ':') file[0] = arg[2];
    else file[0] = 'A' + bdos(DRIVE, 0);
    file[1] = ':';
    /* Initialize counters used to suppress repeating */
    /* prompt on directory rescan.                    */
    skip = skp = 0;
    /* End of initialization  -- start of main loop.   */
    /* BDOS calls bdos(FIRST, fcb) and bdos(NEXT, fcb) */
    /* cause directory scans for match with the <afn>. */
    rq = FIRST;
    while ((rtn = bdos(rq, fcb)) != ALL)
        {
        rq = NEXT;
        /* Skip over previously scanned but undeleted entries. */
        if (++skp <= skip) continue;
        /* Point to directory entry matching <afn> in STDBFR. */
        dirp = STDBFR + ((rtn & 3) << 5);
        /* Get file name from directory entry. */
        i = 1;
        for (j = 1; (j <= 8) && ((c = dirp[j]) != ' '); ++j)
            {
            file[++i] = c;
            }
        file[++i] = '.';
        for (j = 9; (j <= 11) && ((c = dirp[j]) != ' '); ++j)
            {
            file[++i] = c;
            }
        file[++i] = '\0';
        /* Prompt for permission to delete. */
        printf("\nDELETE FILE %s? -- (Y/N/^C): ", file);
        if (toupper(getchar()) == 'Y')
            {
            /* Permission granted -- delete file and */
            /* request rescan of directory.          */
            unlink(file);
            rq = FIRST;
            skp = 0;
            }
        else
            {
            /* Permission denied -- retain file and continue scan.   */
            /* Note additional entry to be skipped if rescan occurs. */
            ++skip;
            }
        }
        /* End of main loop -- directory scan completed. */
        puts("\nDONE");
    }

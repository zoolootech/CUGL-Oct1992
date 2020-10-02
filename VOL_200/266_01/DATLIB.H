/*         Data Handling Package       06 May 1989     File:DATLIB.H
                                   rev 02 Jul 1990
            Robert L. Patton, Jr.
            1713 Parkcrest Terrace
            Arlington, TX 76012
*/
void AdHoc    (char *FileName);
int  CumDays  (int  Month, int Year);
int  Evaluate (char *Word, char Type, float *Value);
int  Leap     (int  Year);
void GetTwo   (char *FileName, float **Xptr, char Xtype, int Xitem,
                               float **Yptr, char Ytype, int Yitem);
void SavePlox (FILE *PloxPtr);
void Split    (char *YYMMDD, int *Y, int *M, int *D);

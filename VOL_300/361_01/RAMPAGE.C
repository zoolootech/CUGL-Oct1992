

/* RamPage.c ---> Transparently Manage a Page of RAM & a File.
 *
 * J.Ekwall                                             11 January 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 18 September 91/EK
 */

/* Notes:
      1. "Puka"  (Poo'-kah) is an Hawaiian Pigeon Hole.
      2. "Nui"   (New'-E)   is    Hawaiian for "Big".
      3. "Pau"   (Pow)      is    Hawaiian for "End" or "Finished".
      4. "Dkine" (da-kIn')  is    Hawaiian for "That".

How it Works:

   An Array of File Pointers is created when a File is "Loaded".  When a Line
is changed, this Array is altered to point at the text of that Line stored in
a "Page" of RAM.  When that "Page" gets full, the file is "Saved" by renaming
it w/ a ".BAK" extension and writing a new one under the original name,
pulling text from the original file and RAM guided by the Array, which is
updated to point at the New File.

   The above Array ("NuiPuka") doesn't really exist.  A macro redirects Array
access actions to an array of pointers, each of which points to a dynamically
allocated array of 1024 long integers (4K of RAM).  In other words, small
files use the RAM-Page plus 4K, larger files consume 4K for each additonal
1024 lines (or fraction thereof), a real RAM savings. */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdek.h>
#include <gadgets.h>


/* Declare Local Macros */
#define NuiPuka(x)    Puka[(x)/1024][(x)%1024]

/* Declare Globals */
long SizeOfRamPage = 65534L;                    /* User Adjustable */

/* Declare Structures, Unions & TypeDefs */
typedef long far * PtrPuka;
static PtrPuka Puka[32];

/* Declar Local Prototypes */
static int  PtrAlloc(void);

int Rampage(char *Dkine, char *Text, int Index)
{
    int i;
    static int Dirty = 0, Fill, Nuff, StrPau;
    static unsigned int NuiPukaPau = 0, Pau;
    static char FileName[81], far *RamPage = NULL;
    static FILE *fp;

    if (!(Index >= 1 && Index <= max(Pau, 1))) return Pau;
    if (Pau > 32767U) { Rampage("Close", NULL, 1); exit(1); }
    if (fp IS NULL && toupper(*Dkine) != 'L') return 0;
    
    switch (toupper(*Dkine)) {
    case 'F':                                           /* Fetch */
       if (Index IS Pau) { strcpy(Text, "~"); return 0; }
       else if (NuiPuka(Index) < 0x400) 
          strcpy(Text, RamPage+StrPau*NuiPuka(Index));
       else {
          auto char *tp1;
       
          fseek(fp, NuiPuka(Index)-0x400, SEEK_SET); fgets(Text, 255, fp);
          if ((tp1 = strrchr(Text, NL)) != NULL) *tp1 = NULL;
       }
       break;
    case 'I':                                           /* Insert */
       if (Pau IS NuiPukaPau) NuiPukaPau += PtrAlloc();
       for (i = Pau++; i >= Index; i--) NuiPuka(i+1) = NuiPuka(i);
       NuiPuka(Index) = 0x1000;
    case 'P':                                           /* Push */
       if (Index IS Pau) {
          NuiPuka(Pau) = 0x1000;
          if (Pau++ IS NuiPukaPau) NuiPukaPau += PtrAlloc();
       }
       if (NuiPuka(Index) < 0x400)
          strcpy(RamPage+StrPau*NuiPuka(Index), Text);
       else {
          if (strlen(Text) > StrPau) {
             if (strlen(Text) > 255) Text[256] = NULL;
             Rampage("Save", "", 1); StrPau = min(strlen(Text)+11, 255);
             Nuff = min(SizeOfRamPage/StrPau, 1023);
          }
          if (Fill >= Nuff) Rampage("Save", "", 1); NuiPuka(Index) = Fill;
          strcpy(RamPage + StrPau*Fill, Text); Fill++;
       }
       Dirty++; return Pau;
    case 'D':                                           /* Delete */
       if (Index < 1 || Index >= Pau) return FALSE;
       for (i = Index; i < Pau; i++) NuiPuka(i) = NuiPuka(i+1);
       Dirty++; return --Pau;
    case 'S':                                           /* Save */
       if (Dirty || *Text) {
          auto char Line[256];

          if (Dirty) {
             auto char BakFileName[81];
             auto FILE *wfp;

             NewExt(FileName, BakFileName, ".BAK"); fclose(fp);
             if (stricmp(FileName, BakFileName)) unlink(BakFileName);
             else { tmpnam(BakFileName); Index = 0; }
             rename(FileName, BakFileName);
             fp = fopen(BakFileName, "r"); wfp = fopen(FileName, "w");
             for (i = 1; i < Pau; i++) {
                Rampage("Fetch", Line, i); NuiPuka(i)   = ftell(wfp) + 0x400;
                fprintf(wfp, "%s\n", RTrim(Line));
             }
             fclose(fp); fclose(wfp); if (!Index) unlink(BakFileName);
          } else fclose(fp);
          Fill  = Dirty = 0;
          if (Index > 1 && *Text) {
             sprintf(Line, "COPY %s %s", FileName, Text);
             system(Line); strcpy(FileName, Text);
          }
          fp = fopen(FileName, "r");
       }
       break;
    case 'L':                                           /* Load */
       {
          auto  char Line[256];

          if (fp != NULL) Rampage("Close", NULL, Index);
          if (RamPage IS NULL)
             while ((RamPage =  malloc(SizeOfRamPage)) IS NULL)
                if ((SizeOfRamPage >>= 2) < 256) exit(1);
          if (!FileExists(Text)) return 0;
          fp = fopen(Text, "r"); strcpy(FileName, Text);
          if (!NuiPukaPau) NuiPukaPau += PtrAlloc();
          Pau = Fill = Dirty = 0; StrPau = 21;
          while (++Pau < 32767) {               /* 32K Lines Max */
             if (Pau >= NuiPukaPau) NuiPukaPau += PtrAlloc();
             NuiPuka(Pau) = ftell(fp) + 0x400;
             if (fgets(Line, 255, fp) IS NULL) break;
             StrPau = max(StrPau, min(strlen(Line)+11, 255));
          }
          Nuff = min(SizeOfRamPage/StrPau, 1023);
          return Pau;
       }
    case 'C':                                           /* Close */
       if (Index IS 1 && Dirty) Rampage("Save", NULL, 1);
       for (i = 0; i < 32; i++) 
          if (Puka[i] != NULL) { farfree(Puka[i]); Puka[i] = NULL; }
       if (RamPage != NULL) farfree(RamPage); RamPage = NULL; 
       NuiPukaPau = Pau = 0; fclose(fp);
    }
    return TRUE;
}

static int PtrAlloc(void)
{
 /* Allocate another 4K RAM Block & Return Incr for NuiPukaPau */
    int Index;

    for (Index = 0; Index < 31 && (Puka[Index] != NULL); Index++);
    if (Index > 31) return 0;
    Puka[Index] = calloc(1024, sizeof(long));
    if (Puka[Index] IS NULL) { Rampage("Close", NULL, 1); exit(1); }
    if (Index IS 31) return 1023; else return 1024;
}


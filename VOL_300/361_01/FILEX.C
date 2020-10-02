
/* Filex.c --> A Collection of File Name Tools
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <dir.h>
#include <stdio.h>
#include <stdek.h>
#include <gadgets.h>
#include <string.h>

char *BaseName(char *FileName)
{
    char *tp1;
    
    if ((tp1 = strrchr(FileName, BKSL)) != NULL) return ++tp1;
    if ((tp1 = strrchr(FileName, COLON)) != NULL) return ++tp1;
    return FileName;
}

long GetFileSize(char *FileName)
{
    struct ffblk fb;

    if(!findfirst(FileName, &fb, 0)) return (fb.ff_fsize); else  return (-1);
}

void NewExt(char *Old, char *New, char *Ext)
{
    char *tp1;

    strcpy(New, Old); if ((tp1 = strrchr(New, DOT)) != NULL) *tp1 = NULL;
    if (!*Ext) return;
    strcat(New, "."); if (*(tp1 = Ext) IS DOT) tp1++; strcat(New, tp1);
}


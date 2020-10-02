
/* TisHelp.c --> Context Sensive Help via a Shell Out to VU.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdek.h>
#include <gadgets.h>

char HelpFileName[81], HelpTag[20];
int HelpKey = 0;

void TisHelp(void)
{
    char Cmd[128];

    if (!*HelpFileName) return; Trim(HelpTag);
    sprintf(Cmd, "VU /C1F301E /F \">%s\" %s", HelpTag, HelpFileName); 
    system(Cmd);
}


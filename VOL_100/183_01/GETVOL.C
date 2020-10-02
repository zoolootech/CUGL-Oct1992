/****************************************************************/
/*                                                              */
/* Title:       Getvol                                          */
/*                                                              */
/* Abstract:    This routine will return the volume name for    */
/*              the specified disk drive (0..n). If the drive   */
/*              does not have a volume label, a null string is  */
/*              returned.                                       */
/*                                                              */
/* Call:        int getvol (drive, name);                       */
/*              int drive;                                      */
/*              char *name;                                     */
/*                                                              */
/* Arguments:   drive:  Drive number to obtain the volume       */
/*                      label. 1 == A, 2 == B, etc.             */
/*                      drive 0 is the current drive.           */
/*              name:   Array of characters [0..11] to receive  */
/*                      the volume label.                       */
/*                                                              */
/* Returns:     0 : no error in the operation.                  */
/*              1 : Invalid drive                               */
/*              2 : Invalid DOS revison (1.x)                   */
/*              3 : Volume label is not defined                 */
/*                                                              */
/****************************************************************/

#include "STDIO.H"
#include "DOS.H"

extern int _dos;                /* Flag for DOS revision level  */

int getvol(drive, name)
int  drive;                     /* Drive number */
int *name;                      /* Volume name  */

{

union REGS inregs;
union REGS outregs;

char    dta_buffer[128];        /* Disk buffer area     */
struct
    {
    char dr_flag;               /* Flag to indicate extended FCB */
    char filler1[5];            /* reserved (should be 0)        */
    char attr;                  /* Volume attribute byte         */
    char drive_code;            /* Drive code                    */
    char filler2[11];           /* Filler for the file name      */
    char filler3[25];           /* Remainder of the FCB          */
    } fcbs;

int indx;

/*
*     Ensure that the drive number is valid
*/

if ((drive < 0) || (drive > 16))
    {
    return (1);
    }

/*
*     Reject the request for Dos 1.x
*/

if (!(_dos))
    {
    return (2);
    }

/*
*     Build the File Control Block area
*/

repmem (&fcbs, "\000", 1, 44);
fcbs.dr_flag    = 0xff;
fcbs.attr       = 8;
fcbs.drive_code = (char) drive;
repmem (&fcbs.filler2, "?", 1, 11);

/*
*     Define the disk transfer area
*/

inregs.h.ah = 26;
inregs.x.dx = (int) &dta_buffer;
int86 (0x21, &inregs, &outregs);

/*
*     Search for the volume label only
*/

inregs.h.ah = 17;
inregs.x.dx = (int) &fcbs;
int86 (0x21, &inregs, &outregs);

/*
*     Return the volume label if no error occured
*/

if (outregs.h.al != 0xff)
    {
    stccpy (name, &dta_buffer[8], 12);
    return (0);
    }
else
    {
    return (3);
    }
}

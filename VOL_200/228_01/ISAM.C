/*
HEADER:         CUGXXX;
TITLE:          Global Data Module (of ISAM system);
DATE:           3-31-86;
DESCRIPTION:    Part of ISAM Library;
KEYWORDS:       ISAM;
FILENAME:       ISAM.C;
WARNINGS:       None;
AUTHORS:        John M. Dashner;
COMPILER:       Lattice C;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/*
**                 ISAMC - Written by John M. Dashner
*/

/*
**                  Global Data Module
*/

int isam_err;       /* error returns from isam routines */

char *isam_msg[] =  /* error messages for each defined error */
{
    "No Error",
    "Record Not Found",
    "Exceeded File Boundary",
    "Not Defined",
    "Key Length Invalid",
    "Not Defined",
    "Not Defined",
    "Created New Index",
    "I/O Error",
    "Insufficient Storage for Control Blocks"
};


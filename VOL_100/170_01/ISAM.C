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


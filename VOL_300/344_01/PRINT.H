/* PRINT.H
 *
 * Header file for PRINT.C
 *
 * Functions to access resident portion of PRINT.COM
 * For documentation see PRINT.C
 *
 * Author: M. Wiedmann, December 1990
 */


/* prototypes                   */

unsigned int    prn_installed(void);
unsigned int    prn_active(void);
unsigned int    prn_submit(char *file);
unsigned int    prn_cancel_file(char *file);
unsigned int    prn_cancel_all(void);
unsigned int    prn_stop(void);
unsigned int    prn_resume(void);


/* error codes                  */

#define PRN_OK                  0x0000
#define PRN_INVALID_FUNCTION    0x0001
#define PRN_FILE_NOT_FOUND      0x0002
#define PRN_WRONG_PATH          0x0003
#define PRN_TOO_MANY_FILES      0x0004
#define PRN_ACCESS_DENIED       0x0005
#define PRN_QUEUE_FULL          0x0008
#define PRN_QUEUE_BUSY          0x0009
#define PRN_NAME_TOO_LONG       0x000C
#define PRN_WRONG_DRIVE         0x000F


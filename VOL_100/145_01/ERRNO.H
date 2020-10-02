/*	error return definitions for math functions
*/

extern int errno;		/* contains the error indicator */

#define EDOM 33			/* arg not in domain of function */
#define ERANGE 34		/* result too large */

/*	error return definitions for system function calls
*/

#define EFNUM 1			/* invalid function number */
#define ENOENT 2		/* File not found */
#define EPNFOUND 3		/* Path not found */
#define EMFILES 4		/* Too many open files (no handles left) */
#define ENACCESS 5		/* Access denied */
#define EHANDLE	6		/* Invalid Handle */
#define ECNTLBLK 7		/* Memory Control Block Destroyed */
#define ENOMEM 8		/* Insufficient memory */
#define EMBA 9			/* Invalid memory block address */
#define EENV 10			/* Invalid Environment */
#define EFORMAT 11		/* Invalid Format */
#define ECODE 12		/* Invalid access code */
#define EDATA 13		/* Invalid Data */
#define EDRIVE 15		/* Invalid drive was specified */
#define ECDIR 16		/* Attempt to remove the current directory */
#define EDEVICE 17		/* Not same device */
#define EFILES 18		/* No more files */

/* CI-C86 I/O error codes
*/

#define EFULDSK 119		/* Full disk */
#define ESKCDEV	120		/* Seek on Charcter Device */

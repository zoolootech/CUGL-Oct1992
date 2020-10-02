/* config.h - system configuration					*/

/* This file defines some constants to configure the system.  You can	*/
/* edit some definitions to change work space, file names, and/or ope-	*/
/* ration mode options.							*/

/* size of working storage						*/
#define MEMSIZE		( (int)0x7F00 ) /* size of main memory in bytes */

/* options for 8080 machine simulater: set 1 to have one, 0 otherwise	*/
#define TRACE		1		/* add trace facility		*/
#define HFLAG		1		/* enable H flag simulation	*/
#define PFLAG		1		/* enable P flag simulation	*/
#define CKPSW		0		/* check validity of flags	*/
#define INOUT		1		/* support IN/OUT instructions	*/

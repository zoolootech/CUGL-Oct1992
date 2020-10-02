/*	Standard header file for programs using the "cmdenv" functions	*/

/*	Option table structure	   */

struct option {
	char * opt_text;
	int opt_type;
	};

#define NAKED_KWD 0
#define SVAL_KWD 1
#define NVAL_KWD 2
#define MSVL_KWD 3
#define MNVL_KWD 4

/*	I/O streams (Leor should have put these in BDSCIO.H)	*/

#define STD_IN	0	/* Standard input device */
#define STD_OUT	1	/* Standard output device */
#define DEV_LST	2	/* List device */
#define DEV_RDR 3	/* Reader device */
#define DEV_PCH 3	/* Punch device */
#define STD_ERR 4	/* Error message I/O and operator communications */
eady to begin moving /tools/c
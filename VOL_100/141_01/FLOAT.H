/* float.h -- constants in octal & useful declarations */
/* 1984 apr 24 PM Krasno */

#define PIOVER4	"\171\356\207\144\0"
#define HALFPI  "\171\356\207\144\1"	/* fp constants */
#define PI      "\171\356\207\144\2"
#define TWOPI   "\171\356\207\144\3"
#define RADINDEG	"\71\36\175\107\373"
#define DEGINRAD	"\12\162\227\162\6"
#define MTWOPI	"\207\21\170\233\3"
#define MPI	"\207\21\170\233\2"
#define MHALFPI	"\207\21\170\233\1"

#define ZERO	"\0\0\0\0\0"
#define HALF	"\0\0\0\100\0"
#define ONE	"\0\0\0\100\1"
#define SQRTTEN	"\304\145\61\145\2"
#define TEN	"\0\0\0\120\4"
#define THTY8	"\0\0\0\114\6"
#define EGHTY6	"\0\0\0\126\7"
#define MEGHTY6	"\0\0\0\252\7"
#define LARGE	"\0\0\0\100\175"

#define byte char
struct _fp { byte __fp[5] ; } ;
#define float struct _fp 
#define fpoct char *


#define ATNC4	"\34\360\273\142\375"
#define ATNC5	"\206\30\177\215\3
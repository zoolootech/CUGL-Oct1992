/*   Micro PLOX standard header file */
/*               11 May 1988         */

/* ------------------ Plot Command codes  */
#define OPEN     0
#define ON       1
#define OFF      2
#define MARK     3
#define MOVE     4
#define LTYP     5 /* line-type */
#define TXTH     6
#define TXTV     7
#define TX2H     8
#define TX2V     9
#define SYMB    10 /* symmetrical centered symbol */
#define ORIG    11
#define MRGN    12
#define HTYP    13 /* set hatch type */
#define FILL    14 /* fill area with pattern */
#define HUE     15 /* set color */
#define CLOS    99
/* ---------------  End of Plot Command codes  */
#define NO       0
#define YES      1
#define INCH    72 /* pixels per inch */
#define SOLID  077 /* solid line code */
#define XMAX   503 /*  7 inches */
#define YMAX   359 /*  5 inches */
#define MAXITEMS 14
#define ARRAY(Z)  &Z[0]  /* used to pass floating array as parameter */
#define EQUAL(S,T) !strcmp(S,T)
#define PIXELS(V)  ((int)(V*INCH+0.5)-1)

typedef struct {               /*   Scaling Record     */
                double Start;  /* offset, data units   */
                double DeltaB; /* data units per box   */
                double DeltaP; /* pixels per data unit */
               } SCALING;


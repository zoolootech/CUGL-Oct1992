/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOLIB.H
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Modify to data array instead of individual fields.
 *  022891 BVM  :   Program uses 'short' instead of 'int' so will work
 *              :   on 32 bit machines.
 *  091190 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#if !defined (DIOLIB_H_DEFINED)
#define     DIOLIB_H_DEFINED    1   /* prevent multiple inclusion   */

/*  Definitions ------------------------------*/

/*  The address definitions are set up for the 8255.
 *  The base address of the 8255 should be passed to the
 *  dio_init() function.  This is the 80X86 PORT address
 *  where the 8255 is located.  The address used in the
 *  outportb() or outp() calls ( out DX, AL where DX =
 *  8255 Base Address in assembly language).
 *  These are the offsets from the base address.
 *  Note that these definitions also correspond to
 *  the port numeric identification as well.
 *  Port 0 is Port A, Port 1 is Port B, Port 2 is Port C.
 *  These are also acceptable definitions where a
 *  numeric port identification is required
 *  ( dio_put_byte and dio_get_byte ).
 */

#define DIO_PORTA   0       /* 8255 PORT A  offset  */
#define DIO_PORTB   1       /* 8255 PORT B  offset  */
#define DIO_PORTC   2       /* 8255 PORT C  offset  */
#define DIO_CNTRL   3       /* 8255 CONTROL offset  */

#define DIO_MAXCH   3       /* 8255 has three ports */

#define DIO_SET     0x80    /* configure set bit    */

#define DIO_PA_M0   0x20    /* Port A I/O Mode 0 : Basic            */
#define DIO_PA_M1   0x40    /* Port A I/O Mode 1 : Strobed          */
#define DIO_PA_M2   0x60    /* Port A I/O Mode 2 : Bidirectional    */
#define DIO_PB_M0   0x00    /* Port B I/O Mode 0 : Basic            */
#define DIO_PB_M1   0x04    /* Port B I/O Mode 1 : Strobed          */

#define DIO_CL_IN   0x01    /* Port C Low  Input    */
#define DIO_PB_IN   0x02    /* Port B      Input    */
#define DIO_CH_IN   0x08    /* Port C High Input    */
#define DIO_PA_IN   0x10    /* Port A      Input    */

/*  8255 Digital select mode:
 *  PA 0-7 output, PB 0-7 output, PC 0-3 output, PC 4-7 output
 *  ALL ports in Mode 0
 */
#define DIO_ALL_OP  DIO_SET

/*  8255 Digital select mode:
 *  PA 0-7 input, PB 0-7 input, PC 0-3 input, PC 4-7 input
 *  ALL ports in Mode 0
 */
#define DIO_ALL_IP  (DIO_SET | DIO_CL_IN | DIO_PB_IN | DIO_CH_IN | DIO_PA_IN)

/*  Error Definitions ------------------------*/

#define DIO_ST_OK   0x0000  /* OK                       */
#define DIO_ST_BB   0x0001  /* Bad bit number           */
#define DIO_ST_BP   0x0002  /* Bad port number          */
#define DIO_ST_BE   0x0004  /* Bad array element number */
#define DIO_ST_NM   0x0008  /* No memory available      */
#define DIO_ST_MA   0x0010  /* Memory already allcoated */

/*- Data Structure ---------------------------*
 *  The layout of the data closely matches the port
 *  available on the I8255.  The base address and
 *  status flag are extra.
 */
#if !defined ( DIODAT_DEFINED )
typedef struct diodat_struct {

	short           base;               /* 8255 address */
	short           stat;               /* status flag  */
	unsigned char   mode;               /* current mode */
	unsigned char   pdat[DIO_MAXCH];    /* port data    */

	} DIODAT;

#define DIODAT_DEFINED  1
#endif

/*  Function Prototypes ----------------------*/

#if !defined ( DIOFNC01_C_DEFINED )
extern void dio_init (DIODAT *, short);
#endif

#if !defined ( DIOFNC02_C_DEFINED )
extern void dio_config (DIODAT *, short, short, short, short);
#endif
#if !defined ( DIOFNC03_C_DEFINED )
extern void dio_bitput (DIODAT *, short, short);
#endif
#if !defined ( DIOFNC04_C_DEFINED )
extern void dio_bitget (DIODAT *, short, short *);
#endif

#if !defined ( DIOFNC05_C_DEFINED )
extern void dio_put_byte (DIODAT *, short, unsigned char);
#endif
#if !defined ( DIOFNC06_C_DEFINED )
extern void dio_get_byte (DIODAT *, short, unsigned char *);
#endif

#if !defined ( DIOFNC07_C_DEFINED )
extern void dio_dump_bytes (DIODAT *);
#endif
#if !defined ( DIOFNC08_C_DEFINED )
extern void dio_load_bytes (DIODAT *);
#endif

#if !defined ( DIOFNC09_C_DEFINED )
extern void dio_bput (short, unsigned char);
#endif
#if !defined ( DIOFNC10_C_DEFINED )
extern void dio_bget (short, unsigned char *);
#endif

#if !defined ( DIOFNC11_C_DEFINED )
extern short dio_pa_aloc   (short);
extern void  dio_pa_free   (void);
extern short dio_pa_setadr (short, short);
extern short dio_pa_bitput (short, short);
extern short dio_pa_bitget (short, short *);
extern short dio_pa_getptr (short, DIODAT **);
extern short dio_pa_config (short, short, short, short, short);
#endif

#endif  /* prevent multiple inclusion   */
/*-
 *  ----------------------------------------------------------------------
 *  END DIOLIB.H Header File
 *  ----------------------------------------------------------------------
 */

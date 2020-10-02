/*-
 *  ----------------------------------------------------------------------
 *  File        :   8255FN.H
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *              :   Microsoft C         Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *  ----------------------------------------------------------------------
 */

#define I8255FN_H_DEFINED   1

/*  Definitions ------------------------------*/

/*- The address definitions are set up for the 8255.
 *  The base address should be passed to the I8255_init() function.
 */

#define I8255_PORTA(X)   (X + 0) /* 8255 PORT A address  */
#define I8255_PORTB(X)   (X + 1) /* 8255 PORT B address  */
#define I8255_PORTC(X)   (X + 2) /* 8255 PORT C address  */
#define I8255_CNTRL(X)   (X + 3) /* 8255 CONTROL address */

#define I8255_MAXCH  0x03    /* three IO ports       */

#define I8255_SET    0x80    /* configure set bit    */

#define I8255_PA_M0  0x20    /* Port A I/O Mode 0 : Basic            */
#define I8255_PA_M1  0x40    /* Port A I/O Mode 1 : Strobed          */
#define I8255_PA_M2  0x60    /* Port A I/O Mode 2 : Bidirectional    */
#define I8255_PB_M0  0x00    /* Port B I/O Mode 0 : Basic            */
#define I8255_PB_M1  0x04    /* Port B I/O Mode 1 : Strobed          */

/*- These are the bit masks for the port input configurations.
 *  These corresponding bits are 0 if the port is configured for output.
 */
#define I8255_CL_IN  0x01    /* Port C Low  Input    */
#define I8255_PB_IN  0x02    /* Port B      Input    */
#define I8255_CH_IN  0x08    /* Port C High Input    */
#define I8255_PA_IN  0x10    /* Port A      Input    */

/*- 8255 Digital select mode:
 *  PA 0-7 output, PB 0-7 output, PC 0-3 output, PC 4-7 output
 *  ALL ports in Mode 0
 */
#define I8255_ALL_OP I8255_SET

/*- 8255 Digital select mode:
 *  PA 0-7 input, PB 0-7 input, PC 0-3 input, PC 4-7 input
 *  ALL ports in Mode 0
 */
#define I8255_ALL_IP ( I8255_SET | I8255_CL_IN | I8255_PB_IN | \
					   I8255_CH_IN | I8255_PA_IN )

/*  Error Definitions ------------------------*/

#define I8255_ST_OK  0x0000  /* OK               */
#define I8255_ST_BB  0x0001  /* Bad Bit number   */
#define I8255_ST_BP  0x0001  /* Bad Port number  */

/*  Data Structure ---------------------------*/

#if !defined ( I8255DAT_DEFINED )

typedef struct I8255dat_struct {

	int             base;   /* 8255 address */
	int             stat;   /* status flag  */
	unsigned char   mode;   /* current mode */
	unsigned char   adat;   /* port A data  */
	unsigned char   bdat;   /* port B data  */
	unsigned char   cdat;   /* port C data  */

	} I8255DAT;

#define I8255DAT_DEFINED 1
#endif

/*  Function Prototypes ----------------------*/

/*  These are the INP and OUTP functions used to
 *  write and read from an 80X86 IO Port.
 */
extern  void chp_portwt (int, unsigned char  );
extern  void chp_portrd (int, unsigned char *);

#if !defined ( I8255F01_C_DEFINED )
extern void I8255_init (I8255DAT *, int);
#endif
#if !defined ( I8255F02_C_DEFINED )
extern void I8255_config (I8255DAT *, int, int, int, int);
#endif

#if !defined ( I8255F03_C_DEFINED )
extern void I8255_bitput (I8255DAT *, int, int);
#endif
#if !defined ( I8255F04_C_DEFINED )
extern void I8255_bitget (I8255DAT *, int, int *);
#endif

#if !defined ( I8255F05_C_DEFINED )
extern void I8255_put_byte (I8255DAT *, int, unsigned char);
#endif
#if !defined ( I8255F06_C_DEFINED )
extern void I8255_get_byte (I8255DAT *, int, unsigned char *);
#endif

#if !defined ( I8255F07_C_DEFINED )
extern void I8255_dump_bytes (I8255DAT *);
#endif
#if !defined ( I8255F08_C_DEFINED )
extern void I8255_load_bytes (I8255DAT *);
#endif

#if !defined ( I8255F09_C_DEFINED )
extern  void I8255_bittog (I8255DAT *, int);
#endif

/*-
 *  ----------------------------------------------------------------------
 *  END I8255FN.H Header File
 *  ----------------------------------------------------------------------
 */

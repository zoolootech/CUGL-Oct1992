/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC11.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Port Array Functions
 *  ----------------------------------------------------------------------
 *  WARNING : I do not have a multi-port board on which to test
 *  these functions.  The single-port functions work correctly, and I
 *  wrote these multi-port functions as an extension to those
 *  single-port functions for those of you with multi-port boards.
 *  These functions here 'seem' correct, but are not truly debugged.
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC11_C_DEFINED  1
#include    "malloc.h"      /* malloc() free()  */
#include    "stdio.h"       /* NULL definition  */
#include    "DIOLIB.H"
#undef      DIOFNC11_C_DEFINED

static DIODAT  *dadat = (DIODAT *)NULL; /* array : base pointer     */
static short    dasiz = 0;              /* array : element count    */
static short    mxbit = 0;              /* array : maximum bit      */

short dio_pa_aloc   (short);
void  dio_pa_free   (void);
short dio_pa_setadr (short, short);
short dio_pa_bitput (short, short);
short dio_pa_bitget (short, short *);
short dio_pa_getptr (short, DIODAT **);
short dio_pa_config (short, short, short, short, short);


/*- DIO : Allocate Data Array Space ----------**
 *  Allocate memory for the DIODAT data array.
 *  Passed:
 *      short   :   number of DIODAT elements to be in array
 *  Returns:
 *      short   :   DIO_ST_OK = OK
 *              :   DIO_ST_NM = No memory available
 *              :   DIO_ST_MA = Existing memory not yet free
 */
short dio_pa_aloc (short danum)
	{
	/*  Memory already in use.
	 */
	if ( dadat != (DIODAT *)NULL )
		return (DIO_ST_MA);

	/*  Allocate memory.
	 *  Check for failure.
	 */
	dadat = malloc ( danum * sizeof(DIODAT) );
	if ( dadat == (DIODAT *)NULL )
		return (DIO_ST_NM);

	/*  Save count of array elements.
	 *  Default data even though don't know addresses yet.
	 *  Calculate maximum bit for this array size.
	 */
	dasiz = danum;
	for ( danum = 0; danum < dasiz; danum++ ){
		dio_init ( &dadat[danum], 0);
		}

	mxbit = danum * 24;

	return (DIO_ST_OK);
	}


/*- DIO : Free Data Array Space --------------**
 *  Allocate memory for the DIODAT data array.
 *  It does no harm to call this function if the
 *  memory has not already been allocated.
 *  Passed:
 *      short   :   number of DIODAT elements to be in array
 *  Returns:
 *      Nothing
 */
void dio_pa_free (void)
	{
	if ( dadat != (DIODAT *)NULL ){
		dasiz = 0;          /* set count to zero    */
		free ( dadat );     /* free array memory    */
		}
	}


/*- DIO : Set Element Address ----------------**
 *  Set the 8255 address of one of the array elements.
 *  Passed:
 *      short   :   DIODAT element affected (Base 0)
 *      short   :   8255 base address
 *  Returns:
 *      short   :   DIO_ST_OK = OK
 *              :   DIO_ST_NM = Memory not allocated
 *              :   DIO_ST_BE = Bad element
 */
short dio_pa_setadr (short danum, short basadr)
	{

	/*  Check for errors.
	 *      No memory.
	 *      Bad element number.
	 *  Otherwise assign address.
	 */
	if ( dasiz == 0 )
		return (DIO_ST_NM);
	if ( (danum < 0) || (danum >= dasiz) )
		return (DIO_ST_BE);

	dadat[danum].base = basadr;
	dadat[danum].stat = DIO_ST_OK;
	return (DIO_ST_OK);
	}


/*- DIO : Array Bit Put ----------------------**
 *  Set/Clear one of the bits in the 8255 port array.
 *  A state of !0 sets the bit and a state of 0 clears the bit.
 *  The bit number should be from 0 -> mxbit-1 (Base 0).
 *  Passed:
 *      short   :   bit number
 *      short   :   state : TRUE (!0) = SET, FALSE (0) = CLEAR
 *  Returns:
 *      short   :   DIO_ST_OK = OK
 *              :   DIO_ST_NM = Memory not allocated
 *              :   DIO_ST_BB = Bad bit number
 */
short dio_pa_bitput (short bit, short state)
	{
	short   danum;

	if ( dasiz == 0 )
		return (DIO_ST_NM);
	if ( (bit < 0) || (bit >= mxbit) )
		return (DIO_ST_BB);

	danum = ( bit / 24);    /* 8255 array element number    */
	bit = bit % 24;         /* 8255 compensated bit number  */

	dio_bitput (&dadat[danum], bit, state);
	dadat[danum].stat = DIO_ST_OK;
	return (DIO_ST_OK);
	}



/*- DIO : Array Bit Get ----------------------**
 *  Read one of the bits in the 8255 port array.
 *  A state of 1 indicates a set bit and a state of 0
 *  indicates a clear bit.
 *  The bit number should be from 0 -> mxbit-1 (Base 0).
 *  Passed:
 *      short   :   bit number
 *      pointer :   short :   state : 1 = SET, 0 = CLEAR
 *  Returns:
 *      short   :   DIO_ST_OK = OK
 *              :   DIO_ST_NM = Memory not allocated
 *              :   DIO_ST_BB = Bad bit number
 *      Loads state with 0 or 1.
 */
short dio_pa_bitget (short bit, short *state)
	{
	short   danum;

	if ( dasiz == 0 )
		return (DIO_ST_NM);
	if ( (bit < 0) || (bit >= mxbit) )
		return (DIO_ST_BB);

	danum = ( bit / 24);    /* 8255 array element number    */
	bit = bit % 24;         /* 8255 compensated bit number  */

	dio_bitget (&dadat[danum], bit, state);
	dadat[danum].stat = DIO_ST_OK;
	return (DIO_ST_OK);
	}


/*- DIO : Get Array Address ------------------**
 *  Return address of the DIODAT array.
 *  Note that NULL is returned if memory not allocated.
 *  Passed:
 *      short   :   element number
 *      pointer :   DIODAT *
 *  Returns:
 *      short   :   DIO_ST_OK = OK
 *              :   DIO_ST_NM = Memory not allocated
 *              :   DIO_ST_BE = Bad element number
 */
short dio_pa_getptr (short danum, DIODAT **diopp)
	{
	if ( dasiz == 0 )
		return (DIO_ST_NM);
	if ( (danum < 0) || (danum >= dasiz) )
		return (DIO_ST_BE);
	*diopp = &dadat[danum];
	return (DIO_ST_OK);
	}


/*- DIO : Array Element Configure ------------**
 *  Configure port direction for one of the 8255s in the array.
 *  Passed:
 *      short   :   element number
 *      short   :   direction Port A
 *      short   :   direction Port B
 *      short   :   direction Port C Low
 *      short   :   direction Port C High
 *  Returns:
 *      short   :   DIO_ST_OK = OK
 *              :   DIO_ST_NM = Memory not allocated
 *              :   DIO_ST_BE = Bad element number
 */
short dio_pa_config (short danum, short pa_dir, short pb_dir,
								  short cl_dir, short ch_dir)
	{
	if ( dasiz == 0 )
		return (DIO_ST_NM);
	if ( (danum < 0) || (danum >= dasiz) )
		return (DIO_ST_BE);

	/*  Initially set mode to DIO_SET since we are
	 *  going to configure the 8255.
	 *  Then OR in the IO direction bits as appropriate.
	 */

	dadat[danum].mode = DIO_SET;

	if ( pa_dir )   dadat[danum].mode |= DIO_PA_IN;
	if ( pb_dir )   dadat[danum].mode |= DIO_PB_IN;
	if ( cl_dir )   dadat[danum].mode |= DIO_CL_IN;
	if ( ch_dir )   dadat[danum].mode |= DIO_CH_IN;

	dio_bput ( dadat[danum].base + DIO_CNTRL, dadat[danum].mode );
	dadat[danum].stat = DIO_ST_OK;
	return (DIO_ST_OK);
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC11.C Source File
 *  ----------------------------------------------------------------------
 */

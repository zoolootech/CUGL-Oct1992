
/* ------------------------------------------------------------ */
/*      This is a portion of the SOUND EFFECTS LIBRARY.         */
/*                                                              */
/*      Copyright (C) 1986 by Paul Canniff.                     */
/*      All rights reserved.                                    */
/*                                                              */
/*      This library has been placed into the public domain     */
/*      by the author.  Use is granted for non-commercial       */
/*      pusposes, or as an IMBEDDED PORTION of a commercial     */
/*      product.                                                */
/*                                                              */
/*      Paul Canniff                                            */
/*      PO Box 1056                                             */
/*      Marlton, NJ 08053                                       */
/*                                                              */
/*      CompuServe ID: 73047,3715                               */
/*                                                              */
/* ------------------------------------------------------------ */

delay(n)
{
	register int i,j;

	while (n--)
	{
		for (i=0; i<250; i++) j = i * 3;
	}
}

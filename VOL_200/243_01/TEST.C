/* This file tests out the MS-DOS specific and memory model specific
** predefines, and the sizeof operator.   To use it, type
**	cpp test.c -M<x>
**		where <x> is the mode you want to test.
*/

I8086	= "I8086"
MSDOS	= "MSDOS"
M_I86	= "M_I86"
M_I86SM	= "M_I86SM"
M_I86MM	= "M_I86MM"
M_I86LM	= "M_I86LM"
I8086S	= "I8086S"
I8086P	= "I8086P"
I8086D	= "I8086D"
I8086L	= "I8086L"
SPTR	= "SPTR"
LPTR	= "LPTR"

#if	sizeof (int *) == 2
sizeof (int *) == 2
#elif	sizeof (int *) == 4
sizeof (int *) == 4
#else
sizeof (int *) is bigger than expected
#endif

#if	sizeof (int (*)()) == 2
sizeof (int (*)()) == 2
#elif	sizeof (int (*)()) == 4
sizeof (int (*)()) == 4
#else
sizeof (int (*)()) is bigger than expected
#endif

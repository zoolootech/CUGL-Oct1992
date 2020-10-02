
/*GLOBAL********************************************************************

	_event_wait - wait for event or timeout

***************************************************************************/

#include <dos.h>
#include <mnpdat.h>

extern USIGN_16 frame_snt,
	frame_rcvd,
	frame_dne;

SIGN_16 event_wait(time,flag)

USIGN_16 time,
	flag;
{

register USIGN_16 i;

i = 0;
time -= 2;		/* to offset overhead in this routine */
				/* 2/10 of a second - just an estimate */

switch (flag)
	{
	
	case FRAME_SND:
		if (frame_snt)
			{
			frame_snt = FALSE;
			break;
			}
		set_int();
		for (;(frame_snt == FALSE) && (i <= time); i++)
			{
			suspend (1);
			if (lne_stat())
				return (NO_PHYSICAL);
			}
		if (frame_snt)
			{
			frame_snt = FALSE;
			return(SUCCESS);
			}
		else
			return(TIME_OUT);

	case FRAME_RCV:
		if (frame_rcvd)
			{
			frame_rcvd = FALSE;
			break;
			}
		set_int();
		for (;(frame_rcvd == FALSE) && (i <=time); i++)
			{
			suspend(1);
			if (lne_stat())
				return (NO_PHYSICAL);
			}
		if (frame_rcvd)
			{
			frame_rcvd = FALSE;
			return(SUCCESS);
			}
		else
			return(TIME_OUT);

	case FRAME_DN:
		if (frame_dne)
			{
			frame_dne = FALSE;
			break;
			}
		set_int();
		for (;(frame_dne == FALSE) && (i <= time); i++)
			{
			suspend (1);
			if (lne_stat())
				return (NO_PHYSICAL);
			}
		if (frame_dne)
			{
			frame_dne = FALSE;
			return(SUCCESS);
			}
		else
			return(TIME_OUT);

	default:
		set_int();
		return(FAILURE);
	}

set_int();
return(SUCCESS);

}

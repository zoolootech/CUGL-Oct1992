/* Determine the baud rate to operate at by getting characters until
we find something recognizable. Return with the global 'rate' set and
Novation type modems put in transparent mode. The initial delay is because
the answer modem returns connect info much earlier than the originate. */

connect() {

unsigned n;

        delay(100);                             /* delay */
        flush(1);                               /* flush garbage */
        cd_flag= 0;                             /* watch carrier */

        while (1) {
                rate= 300;
                baud(rate);                     /* test at 300 baud */
                n= mrconin();                   /* get a raw key */

/* At 1200 and above, one byte (CR or anything) fits into 1/4th a character,
so the rest of it is ones. If we find f0h, try more or less the same thing
as the 300 vs. 600 case, but at 1200 baud. */

                if ((n & 0xf0) == 0xf0) {
                        rate= 1200;
                        baud(rate);
                        n= mrconin();           /* RAW Modem conin() */
                        if ((n & 0x7f) == CR) break;
                        if ((n & 0x7f) == ' ') break;
                        if ((n & 0xf0) == 0xf0) {
                                rate= 2400;
                                baud(rate);
                                break;
                        }

/* For 300 baud, we should get a CR. If so, get another to make sure. It
might have been garbage or a leftover result code. Baud rate is already
set to 300. */

                } else {
                        n= mrconin();
                        if ((n & 0x7f) == CR) break;
                        if ((n & 0x7f) == ' ') break;
                }

/* None of the above. Now we need to flush before we try again, as we
somehow might have ended up receiving 300 baud chars at 1200, and that
generates multiple garbage bytes. */

                flush(5);                     /* 50 mS quiet, flush garbage */
        }

/* All set. Put Novation modems to sleep, and flush the software ring buffer
and things. */

        if (mdmtype == SMARTCAT) atp("\016U 0",CR);
        if (mdmtype == IBMJR) atp("\016T 0",CR);
        if ((mdmtype == IBMJR) || (mdmtype == SMARTCAT)) delay(100);
        mconflush();
}
{

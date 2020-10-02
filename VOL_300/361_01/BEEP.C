
/* Beep.c --> Right out of Turbo-C Bible.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <stdek.h>
#include <gadgets.h>
#include <dos.h>

void Beep(unsigned int Pitch, unsigned int Duration)
{ sound(Pitch); delay(Duration); nosound(); }


/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255PC.H
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Definitions for the IBM PC 8255.
 *              :   It is located at 80X86 port address 60H.
 *  ----------------------------------------------------------------------
 */

/*- The 8255 in the PC has several assorted purposes
 *  assigned to its ports.
 *  Port A is configured for input.
 *  It can be set to read the keyboard scan code or
 *  the hardware SW1 settings.
 *  Port B is configured for output.
 *  It has some general control signals.
 *  Port C is set for input.
 *  It has CL set aside for the hardware SW1 settings
 *  and CH for some general purpose indicators.
 */

#define PCPPI_HSW1_PORT   1     /* hardware switch 1 input port     */
#define PCPPI_KBSC_PORT   1     /* keyboard scan code input port    */
#define PCPPI_HSW2_PORT   3     /* hardware switch 2 input port     */
#define PCPPI_SPKR_PORT   2     /* speaker control output port      */

/*- Port bit numbers should be from 1 - 24 as follows:
 *   1 = Port A Bit 0;   8 = Port A Bit 7
 *   9 = Port B Bit 0;  16 = Port B Bit 7
 *  17 = Port C Bit 0;  24 = Port C Bit 7
 */

#define PCPPI_SW10   1      /* selected SW1 inputs  */
#define PCPPI_SW11   2
#define PCPPI_SW12   3
#define PCPPI_SW13   4
#define PCPPI_SW14   5
#define PCPPI_SW15   6
#define PCPPI_SW16   7
#define PCPPI_SW17   8

#define PCPPI_TC2G   9      /* timer counter 2 gate */
#define PCPPI_SPKR  10      /* speaker enable bit   */

/*  When low   the switches 0 - 4 are read into PC0.
 *  When high, the switches 5 - 8 are read into PC0.
 */
#define PCPPI_PC0C  11      /* PC0 control inputs           */
#define PCPPI_CSMO  12      /* cassette motor off           */
#define PCPPI_EMPC  13      /* enable memory parity check   */

#define PCPPI_EICC  14      /* enable IO channel check      */
#define PCPPI_EKBD  15      /* disable keyboard             */
#define PCPPI_SCSW  16      /* PA scan code / switch select */

#define PCPPI_SW20  17      /* selected SW2 inputs          */
#define PCPPI_SW21  18
#define PCPPI_SW22  19
#define PCPPI_SW23  20

#define PCPPI_CSDI  21      /* cassette data in                 */
#define PCPPI_TC2O  22      /* timer counter 2 output           */
#define PCPPI_IOCC  23      /* disable IO channel check         */
#define PCPPI_OBMC  24      /* disable on board memory check    */

/*-
 *  ----------------------------------------------------------------------
 *  END I8255PC.H Source File
 *  ----------------------------------------------------------------------
 */

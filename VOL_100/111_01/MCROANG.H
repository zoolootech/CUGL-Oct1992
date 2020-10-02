/*                 MICROANGELO GENERAL FUNCTION FILE

			 Author - Joe Shannon
			    566 Rock Road
			State College, PA 16801
			    (814) 238-0029

			      Version 1.0
			   February 28, 1982

**This program is released to public domain with the rquest that author credit
  be retained in the source.
*/


#define	DATAPORT	0xF0
#define	STATPORT	0xF1	/* output a 1 then a 0 to STAT resets */
#define	SENDBIT		1	/* loop on non zero to test port ready */
#define	RECVBIT		2	/* loop on zero to await char from MA  */

/*================
 ALPHA MODE
==================
 */

#define	AMOD_SET	0x80	/* set the alpha mode to AMOD_  */
#define	AMOD_SC		0x80	/* clear screen and home ALPHA cursor */
#define	AMOD_EC		0x40	/* enable special ASCII code interpret */
#define	AMOD_HS		0x20	/* enable HIGH SPEED alpha mode */
#define	AMOD_CU		0x10	/* disable display of ALPHA cursor  */
#define	AMOD_FO		0x08	/* select user defined CHAR SET  */
#define	AMOD_OS		0x04	/* enable OVERSTRIKE mode */
#define	AMOD_UL		0x02	/* turn on UNDERLINING  */
#define	AMOD_FG		0x01	/* invert char FIGURE GND */

#define	ACUR_SET	0x81	/* position alpha cur @<row><col> */
#define	ACUR_GET	0x82	/* get alph cur pos <row><col> */
#define	SCRL_SET	0x83	/* set alpha scroll to <0-39> */
#define	SCRN_SPL	0xB8	/* set screen size <l> */
#define	CONT_CDS	0xB9	/* define ALPHA cont codes <c10x0x0xc8> */

/*================
 GRAFICS CURSOR
==================
 */

#define	GCUR_SET	0x84	/* set grafics cursor <xh,xl,yh,yl> */
#define	GCUR_GET	0x85	/* read the graf cur <xh,xl,yh,yl>  */
#define	GCUR_ALP	0x86	/* set grafics cur to alpha cursor  */
#define	GCUR_TRX	0x87	/* set grafics cur to tracking cross */
#define	GCUR_REL	0xB4	/* gr cur relitivly <dxh,dxl,dyh,dyl> */

/*================
 SCREEN 
==================
 */


#define	SCRN_CLR	0x88	/* clear screen */
#define	SCRN_GND	0x89	/* set screen ground <0 or 1> */
#define	SCRN_TOG	0x8A	/* toggle screen ground */
#define	SCRN_GET	0x8B	/* get screen GROUND <0 or 1>  */

/*================
 POINT
==================
 */

#define	PNT_OFF		0x8C	/* POINT ON <4 bytes>  */
#define	PNT_ON		0x8D	/* POINT OFF <4 bytes>  */
#define	PNT_CPL		0x8E	/* compliment point <4 bytes>  */
#define	PNT_GET		0x8F	/* READ POINT <val>   */
#define	RPNT_OFF	0xBC	/* REL POINT ON <4 bytes>  */
#define	RPNT_ON       	0xBD	/* REL POINT OFF <4 bytes>  */
#define	RPNT_CPL	0xBE	/* REL compliment point <4 bytes>  */
#define	RPNT_GET	0xBF	/* REL READ POINT <val>   */

/*================
 VECTOR 
==================
 */

#define	VECT_OFF	0x90	/* vector on <4 bytes> */
#define	VECT_ON		0x91	/* VECTOR OFF <4 bytes> */
#define	VECT_CPL	0x92	/* compliment vector <4 bytes> */
#define	RVEC_OFF		0xC0	/* REL vector on <4 bytes> */
#define	RVEC_ON		0xC1	/* REL VECTOR OFF <4 bytes> */
#define	RVEC_CPL	0xC2	/* REL compliment vector <4 bytes> */

/*================
 REGION 
==================
 */

#define	REG_OFF		0x94	/* region on  <8 bytes>  */
#define	REG_ON		0x95	/* region off <8 bytes>  */
#define	REG_CPL		0x96	/* compliment region  <8 bytes> */
#define	RREG_OFF	0xC4	/*REL region on  <8 bytes>  */
#define	RREG_ON		0xC5	/*REL region off <8 bytes>  */
#define	RREG_CPL	0xC6	/*REL compliment region  <8 bytes> */

/*================
 GRAFIC CHAR MODE 
==================
 */

#define	CHAR_PLT	0x98	/* plot character <c>  */
#define	CHAR_MOD	0x99	/* set plot char mode <CMODE_>  */
#define	CHAR_DEF	0x9A	/* def an ASCII char <asc><s110x0x0x0x0xs0>*/
#define	CHAR_LOD	0x9B	/* load the default char set */

/*================
 LIGHTPEN
==================
 */

#define	TRAX_OFF	0x9C	/* turn tracking cross off */
#define	TRAX_ON		0x9D	/* turn tracking cross on  */
#define	TRAX_GET	0x9E	/* read tracking cross <4 bytes> */
#define	PEN_GET		0x9F	/* read light pen <00> or <01,4 bytes> */

/*================
 CROSSHAIRS
==================
 */

#define	CROS_OFF	0xA0	/* crosshairs off */
#define	CROS_ON		0xA1	/* crosshairs on @loc <4 byte>  */
#define	CROS_GET	0xA2	/* read crosshairs <4 byte> */
#define	CROS_CX		0xA3	/* draw crosshairs at CX,CY  */

/*================
 MEMORY
==================
 */

#define	MEMY_DMP	0xA4	/* dump screen read <b10x0x0x0x0xb7800> */
#define	MEMY_LOD	0xA5	/* load screen send <b10x0x0x0x0xb7800> */
#define	MEMY_EXM	0xA6	/* examine memory <nh,nl,ah,al,b10x0x0xbn>*/
#define	MEMY_DEP	0xA7	/* deposit memory <nh,nl,ah,al,b10x0x0xbn>*/

/*================
 UTILITY
==================
 */

#define	USER_ADD	0xA8	/* set user command address <ah,al> */
#define	USER_CAL	0xA9	/* call user code <ah,al,imask,ih,il>*/
#define	IMSK_LP		4	/* interupt mask to enable light pen int */
#define	IMSK_DT		2	/* mask for data to host */
#define	IMSK_DF		1	/* mask for data from host */
#define	INTR_SWT	0xAA	/* switch real time interupts */
#define	COLD_STR	0xAB	/* force a cold start of MicroAngelo */

/*================
 USER 
==================
 */

#define	USER_1		0xAC	/* user defined  */
#define	USER_2		0xAD	/* user defined  */
#define	USER_3		0xAE	/* user defined  */
#define	USER_4		0xAF	/* user defined  */

/*================
 TEST
==================
 */

#define	TEST_PRM	0xB0	/* test EPROM send <#blocks> get <chsum> */
#define	TEST_RAM	0xB1	/* test ram get <0> or <1><ah,al> */
#define	TEST_ALP	0xB2	/* test alpha */
#define	TEST_SQU	0xB3	/* munching squares <s,i,n>*/

/*================
 CIRCLE
==================
 */

#define	CIRC_OFF	0xC8	/* turn circle off <radius> */
#define	CIRC_ON		0xC9	/* circle off <rad> */
#define	CIRC_CPL	0xCA	/* COMPLIMENT CIRCLE <RAD> */

/*================
 FLOOD
==================
 */

#define	FLOD_OFF	0xCC	/* flood reg with zeros <xh,xl,yh,yl> */
#define	FLOD_ON		0xCD	/* flood region with ones     "     "*/
#define	RFLD_OFF	0xCE	/* flood relitive <dxh,dxl,dyh,dyl> */
#define	RFLD_ON		0xCF	/* same above */

/*================
 MACRO
==================
 */

#define	MACR_STR	0xD0	/* start/stop  def <n> or <FF> get <code>*/
#define	MACR_NXT	0xD1	/* add next macro byte <byte> get <code> */
#define	MACR_CLR	0xD2	/* era macro or clear get<n> or <FF,sh,sl>*/
#define	MACR_INV	0xD3	/* invoke macro <n> */

def
/****************************************************************************
 *                                                                          *
 *                            jjbset.h                                      *
 *                                                                          *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                    *
 *                                                                          *
 *   Purpose:                                                               *
 *                                                                          *
 *      The main reason for the get( and set( functions is to provide an    *
 *      easy way to fetch and store integers and characters so they are     *
 *      TRULY GLOBAL for all source and object files linked. Plus, using    *
 *      arrays allows all variables to be initalized fast and easy.         *
 *                                                                          *
 *     JJB, 9236 church Rd suite 1082, Dallas, Tx 75231 (214) 341-1635      *
 ****************************************************************************/

/*
JJB has defined the following arrays as follows:

      #define IARRYSIZE  80;
      #define CARRYSIZE  10;

       unsigned  int  iarry[IARRAYSIZE];
       unsigned char  carry[CARRAYSIZE];

  The function jjb_initalize(), initalizes the integer and character
  arrays to zero and initalizes some of the variables such as the

Prototypes:
  unsigned int  get(int)           for the integer array
  void set(int, int)
  unsigned char getc(int)          for the character array
  void setc(int,char)
*/

#ifndef IARRYSIZE
#define IARRYSIZE      80
#define IASIZE          0  /* holds size of the array.                    */
#define VIDEOMODE       1  /* video mode of user of your program          */
#define VATTRIB         2  /* the attribute used for all 'vf' functions   */
#define VNCOLOR         3  /* the attribute set for your normal color     */
#define VRCOLOR         4  /* the attribute set for your reverse color    */
#define VUCOLOR         5  /* the attribute set for your underline color  */
#define VICOLOR         6  /* the attribute set for your input color      */
#define VNCOLORMENU     7  /* the normal attribute for pull-down menus    */
#define VRCOLORMENU     8  /* the reverse attribute for pull-down menus   */
#define VUCOLORMENU     9  /* the underline attribute for pull-down menus */
#define VWCOLORMENU    10  /* the border attribute for pull-down windows  */
#define VEXITATTRIB    11  /* attribute saved so exiting can use same     */
#define VDOWN          12  /* number of rows to come down in 'vfsdo('     */
#define VOVER          13  /* come over to this column for 'vfsdo('       */
#define WROW           14  /* window row for last window made             */
#define WCOL           15  /* window column for last window made          */
#define WWIDTH         16  /* window width for last window made           */
#define WDEPTH         17  /* window depth for last window made           */

#define IK_AL_REG      20  /* low byte reg of kbd interrupt in kbint()    */
#define IK_AH_REG      21  /* high byte reg of kbd interrupt in kbint()   */
#define IKEY           22  /* integer value of last keypress  KEY         */
#define IKEYU          23  /* integer value of KEYU                       */
#define IK_EXT         24  /* integer value of K_EXT                      */
#define IK_CODE        25  /* integer value of keyboard scan code K_CODE  */

#define UPPER_SW       26   /* set T forces all keypresses to upper case. */
#define VCLRW_SW       27   /* set T turns off big JJB window             */
#define SHOWSETUP_SW   28   /* set T shows jjb_setup() on screen          */
#define BEEPOFF_SW     29   /* set T turns off beep                       */
#define HELPOFF_SW     30   /* set T turns off F1 help.                   */
#define HEADINGOFF_SW  31   /* set T turns off heading row display        */
#define BOTOFF_SW      32   /* set T turns off bottom row display         */
#define ALTOFF_SW      33   /* set T turns off ALT key for opt selection  */
#define ALTXOFF_SW     34   /* set T disables ALT X to exit to DOS        */
#define ALTXSURE_SW    35   /* set T asks question before DOS exit        */

#define KBDDEL         40   /* number 1-255 to slow up store_keys(        */
#define JDEL           41   /* value of last delay. jdelay(SAME) uses it. */
#define INT_INPUTTED   42   /* number input_num( accepted                 */
#define DEFAULTOPT     43   /* holds a number from 1 to 50 which refers
                 to the default option. The default option is
                 the option JJB will call when it completes
                 the option it is executing. You may change
                 this number from any function.             */
/* internal JJB variable and switches for the enter( functions            */
#define EPTR           44   /* enter pointer                              */
#define ESTART         45   /* start with this pointer                    */
#define EMAX           46   /* largest possible pointer for editing       */
#define EDSP_SW        47   /* when set T, all enter functions display    */


/****************************************************************************
 *                                                                          *
 *                     GET A CHARACTER FROM JJB                             *
 *                     SET A CHARACTER IN JJB                               *
 *                                                                          *
 *   'getc( and setc( are the same as the integer function except they      *
 *   fetch and store characters.                                            *
 *                                                                          *
 *   Do not confuse 'getc(KE)' with 'get_ch()'. The function get_ch() waits *
 *   for a key to be pressed and then stores the values in the character    *
 *   array under the definitions: KE, KEU, KEE and KCODE. get_ch() also     *
 *   stores the integer equivalent in the integer arrays under the          *
 *   definitions: IKE, IKEU, IKEE and IKCODE.                               *
 *                                                                          *
 *   char getc(int);          ex:  ch = getc(KE); reads last keypress       *
 *   void setc(int,char);          setc(USERC1,ch); stores char in USERC1   *
 *                                                                          *
 ****************************************************************************/

#define CARRYSIZE  10   /* size of the character array */
#define K_AL_REG   81   /* save low byte from last keyboard interrupt     */
#define K_AH_REG   82   /* save high byte from last keyboard interrupt    */
/* in kbint(), KEY   is made equal to K_AL_REG and
         K_EXT is made equal to K_AH_REG                            */
#define KEY        83   /* last keypress character                        */
#define KEYU       84   /* last keypress taken to upper case              */
#define K_EXT      85   /* if KEY= '\0', K_EXT hold byte of extention     */
#define K_CODE     86   /* keyboard scan code                             */
#define REMOVE     87   /* remove key for enter( functions  '*'           */
/* Note: You may be thinking that KEY above should be defined as 1,
   instead of 81. You are correct. However JJB has made it 81 for
   a reason.

   The get( and set( functions check to make sure that the value being
   sent to it is between 0 and 79. If not, an error message will occur.

   Therefore if you use 'get(KEY)' instead of the proper 'getc(KEY)',
   JJB will catch your error.

   'getc(' and 'setc('  substracts 80 from the value being sent  to
   it before it fetches or stores a character.
 */

/* These are defined for the setting the color functions.                 */

#define BLACK   0
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define BROWN   6
#define WHITE   7
#define GRAY    8

#define ONBLACK    + 0
#define ONBLUE     + ( BLUE    * 16)
#define ONMAGENTA  + ( MAGENTA * 16)
#define ONGREEN    + ( GREEN   * 16)
#define ONCYAN     + ( CYAN    * 16)
#define ONRED      + ( RED     * 16)
#define ONBROWN    + ( BROWN   * 16)
#define ONWHITE    + ( WHITE   * 16)
#define ONGRAY     + ( GRAY    * 16)
#define MAKEBRIGHT | 8

#define T 1                 /* true                                       */
#define DRAWLINE 99         /* use in option( to separate functions.      */
#define ATEND -1            /* use in enter functions to input ATEND      */
#define SAME -1             /* use in jdelay(SAME) & vfsdo(" ",SAME)      */
#endif


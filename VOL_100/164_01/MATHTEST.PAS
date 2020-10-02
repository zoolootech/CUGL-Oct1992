{$PAGESIZE:60   $TITLE:'TEST MATH EXT. PRECISION SUBROUTINES'}
{$SUBTITLE:'HIGH SPEED VERSION - WITHOUT TEST DISPLAYS      '}
{$DEBUG-                                                     }
{$INCLUDE:'A:MATH.INT'}

PROGRAM MATHTEST (INPUT, OUTPUT);
USES MATH;

VAR  A,B,C,D,E,F:                  XINT;
     CA,CB,CC,CD,CE,CF:            XCHAR;
     I:                            INTEGER;
     OVERFLOW:                     BOOLEAN;
     COMPARE_RESULT:               XCOMP_TYPE;
     TIME_OF_DAY:                  STRING (8);

procedure time (var s: string);    extern;

BEGIN

  repeat

     writeln ('enter a....');
     reset   (input);
     readln  (input, ca);
     IF   CA [1]  = '.'  THEN BREAK;
     writeln ('enter b....');
     reset   (input);
     readln  (input, cb);

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('EXTENDED PRECISION MATH  -  TEST - STARTED  ',
              TIME_OF_DAY);
     WRITELN;

{*************************************************************}
{*******************                  ************************}
{*******************      X_CTOX      ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('      X_CTOX      TEST',
              '    STARTED  ', TIME_OF_DAY);
     WRITELN;

     X_CTOX (CA, A, OVERFLOW);
     WRITELN
           ('OVERFLOW FROM CONVERSION OF A = ', ORD(OVERFLOW));

     X_CTOX (CB, B, OVERFLOW);
     WRITELN
           ('OVERFLOW FROM CONVERSION OF B = ', ORD(OVERFLOW));

{*************************************************************}
{*******************                  ************************}
{*******************      X_ADD       ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('      X_ADD       TEST',
              '    STARTED  ', TIME_OF_DAY);

     X_ADD  (A, B, C, OVERFLOW);
     X_XTOC (C, CC);

     WRITELN;
     WRITELN ('  ', CA);
     WRITELN ('+ ', CB);
     WRITELN ('= ', CC);
     WRITELN ('OVERFLOW = ', ORD(OVERFLOW));

{*************************************************************}
{*******************                  ************************}
{*******************      X_COMP      ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('      X_COMP      TEST',
              '    STARTED  ', TIME_OF_DAY);

     WRITELN;
     COMPARE_RESULT := X_COMP (A,B);
     CASE  COMPARE_RESULT  OF
          X_A_LT_B:  WRITELN (CA, ' IS LESS THAN ',    CB);
          X_A_EQ_B:  WRITELN (CA, ' IS EQUAL TO ',     CB);
          X_A_GT_B:  WRITELN (CA, ' IS GREATER THAN ', CB);
          OTHERWISE  WRITELN ('Otherwise is impossible');
          END;  {CASE}

{*************************************************************}
{*******************                  ************************}
{*******************      X_DIV       ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('      X_DIV       TEST',
              '    STARTED  ', TIME_OF_DAY);

     X_DIV  (A,B,C,D,OVERFLOW);
     X_XTOC (C, CC);
     X_XTOC (D, CD);

     WRITELN;
     WRITELN ('  ', CA);
     WRITELN ('/ ', CB);
     WRITELN ('= ', CC);
     WRITELN ('REM      = ', CD);
     WRITELN ('OVERFLOW = ', ORD(OVERFLOW));

{*************************************************************}
{*******************                  ************************}
{*******************      X_MULT      ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('      X_MULT      TEST',
              '    STARTED  ', TIME_OF_DAY);

     X_MULT (A,B,C,OVERFLOW);
     X_XTOC (C, CC);

     WRITELN;
     WRITELN ('  ', CA);
     WRITELN ('* ', CB);
     WRITELN ('= ', CC);
     WRITELN ('OVERFLOW = ', ORD(OVERFLOW));

{*************************************************************}
{*******************                  ************************}
{*******************      X_SUB       ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('      X_SUB       TEST',
              '    STARTED  ', TIME_OF_DAY);

     X_SUB  (A, B, C, OVERFLOW);
     X_XTOC (C, CC);

     WRITELN;
     WRITELN ('  ', CA);
     WRITELN ('- ', CB);
     WRITELN ('= ', CC);
     WRITELN ('OVERFLOW = ', ORD(OVERFLOW));

{*************************************************************}
{*******************                  ************************}
{*******************       TIME       ************************}
{*******************                  ************************}
{*************************************************************}

     time (TIME_OF_DAY);
     WRITELN;
     WRITELN ('EXTENDED PRECISION MATH  -  TEST  -  ENDED  ',
              TIME_OF_DAY);
     WRITELN;

    UNTIL 1 = 0;  {repeat end}

END.

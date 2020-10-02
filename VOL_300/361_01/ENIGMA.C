

/* Enigma --> A 40/79 version of Louis XIV's "Great Cypher".    15 July 91
 *
 * J.Ekwall
 *
 * Based on "The Bazeries Cylinder" by Rinaldo F. Prisco, BYTE June 83
 * ppg 352-360.
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 */

#include <ctype.h>
#include <stdek.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *BadKey[] = {
    "",
    "Bad Key.  Try Again using these Guidelines:",
    "",
    "   1. Use at least 6 characters (Spaces Don't Count).",
    "   2. Use Mixed Case Text.  (Both Upper & Lower Case).",
    "   3. Include at least One Numberic Digit.",
    "   4. Include at least One Non-AlphaNumeric Character.",
    "   5. Don't Use Control Characters or Extended Characters.",
    "   6. The first 40 (non-Space) characters are Significant.",
    "",
    NULL };

char *Cylinder[] = {
"N3@%gGoOWDFYX6ELsinHU7t&10ruzBQ)+?A/#jvy*abdpZ^(-mIK=VqSh9fw$Rk28~_cPe54!JxTlCM",
"oGyLsU47rMNhezY2K9T~^Sufgt%IV&XW*(kvH)-=+qlF!Q6abcBdpACJ13DR@$80?_/#imn5xjZwEOP",
"t)i#AgsY~EIhnuDL@*j(?^mC&Tx8NFbrzoMlaQZ3yB269!UVX$-=vdO41H5wJSqKW0_PRkcp+/fG7%e",
"+?RVO~S/%#yY=higsBzLjEFcwpNPvGqnHekIJQX4bK6dA7rmxuot8f9!l0@DZ2W5$1Ca3^&TM*(U)-_",
"FuOdWfoYnMVZ3v6JQ25CGRezLgKUqAh7r8tlXxB40N9~c!@s$%mk&()-I=Dy_+1iEPS^j?b/#HT*apw",
"oP~!5BO)A-=(bt?xYlnZ3iwGVeyCIqjrXUDdu2L8&9+4/6#Q$%*aF_TgmpNshv7ES^@kHzKMfRJ1c0W",
"rfjqFygiHOR$@v9pd~)4x=CtL_e+IbS8AcUuZ0ksoGPnVlN2!%W1^*D7JwEBKTY3&z5Qm(M-?h/aX6#",
"+HZ6%XRk&?GaSC20#qltzeivjxusDJLTU^cwWE-5/A~hnryBNdIm9*(of!Y3@)pV7$=bQ148F_gPKMO",
"CM6aksIcorNEHUVY157hyf9eilzGSpW80TnL&3*)_bxXjK4$%(+Bd?Z-gA!F/m#t=quP2OQR@vwJ~D^",
"lQfwcgU$nqhR()pAI3rMJ@-ZKT9!=+?v/WjEaF_#emD2dY8^sHLzOVyGb14u6CiPt7~oBxkN5%&0*SX",
"awAClxz5~fU$7^_+&Z8?/%bSqnJvDFeHoWXY239!g@(ci=RdsrNIkyLO0*BmthMTEp46)-#uGKjPQV1",
"sLWoHn24=e+!X^aIM?myJF%AZ*_#36cSl7(UhxRN8pDgfktjuwEOqQVYr59z@0&1)$/bdKTiv~P-BCG",
"sV1H%JI*)t2-lQ5&Gxu=_?zO7^anmryDThRFKZ30P+/X(#ULg!86qvfo4@bENeBCpiMSkjwW9~A$Ycd",
"i)r-=+A?/#EUcK&_O9qgnbtj2N6oxwBGJVRu1aZ@fImLXY~CyPS!$Q%*HvlpFMT3hzWe45s7Dd^k80(",
"fzL&=w_IMHXN$^U*+?/k6#Zg35dsyCFJShBxVolQieamncqvPRObYp27jG0~TA8u@rD1%tKWE94!()-",
"iPSlDr&(4$kzGQubR3jn=o^8T_dX~?/mUC1p#M+wWeHVBc5O0tvIN7hJK6*)-Yg!saAEyqFLZ2f9@%x",
"e08kTgqX7ADfIRU1%_r/GyS4bBnpxCH2htjzP59~6J!c@OQWL$^iwM)-Fm=lsdvaEKNYZV3ou&(+?#*",
"dXcfsTZ-a*=Gz0()3BC_+?/#yEpItnqKNDQFwAPRUW1h2revV56489~mxHLuJibgjoOYlS7!M@$%k^&",
"+!?/pQsEvxbZ9#@e$mDGLqKT0M3IadikyBgRVWjXYh4cfNPSJtnw7%&6*OuAU8~(H)z2F-51=olrC_^",
"INxpUWYE6P90!c&*-F@GQbsST~=1yOVq7+w?/8g5kdHoZJ3v%)R_X2Mj#C^atK$(4LnilfmrzheuABD",
"n!W^8J*7)-UpTM=_t+?/~#lorszdjuxehmwBCvDHKcLORSFXIiqYGf1k25Zb3gayAP4Q690EN@$%&V(",
"fsU^*-=_61nHOb4gqlIC0A~+?k/#SjmFGeDpZctPzwQYWh2789xVdiBN3!$5%MRoXaT&uyL@()EJrvK",
"oxf6h@_G)M+Dl89?/#QPvbKS147L!ukwHX0qjFmaCOdzT3V$%Eit^&*2grI(yc-nN=ZsAJWRe5UpYB~",
"rtRZ3i@Obs$^~Ca)mES48%1W+?oLuvK5=N/U#dxfQyHelMhqFjGcIw6PX7gB90DJV&*(-_nz2p!kATY",
"mVFn7tvIS@~&bueklwxDhLzY5o)pZN!W-EOd4i^=?TsU2KCQR18$%3PyJ90(6_/cA#fHMX*gjB+raqG",
"avAIXY4ijnGOhSTU~0Jdy!@*oWt(-sHK5$L3=?e9NBPpkrcwEbCFDmMzuVZ28)gfR%7^&q_/#6x+Q1l",
"tzQ9~$(-KDuw5FmYhI!_ejG23X4%cgJ+8?/nR#r6)alyAO7dikUPSWLs=Z@CV1&qMTv^p*fN0BxHEob",
"inbz$!%-h=_y*p+x#vZ^j25?Io/Dq1@3CT9ekm46QuX~KGNftFHPwgBrlSJMUWdEaOcVRY70&()8LsA",
"oILdsOVPjD78!yQC9tS~NnFU1g%2^4e56&*cTkAEXpixZW@$v()-=a+MlfzHmGY30bB?/_#qKrJuRhw",
"nRSrOY478%&DiEaIJhqV3Lu9C!g)-^X=mH(z_@+?wk~pFWNZ12cy0$*/#xBTolsdAKMtevbPGQUj56f",
"+fQ(&T=^?/H#voXlS71tzyAbBIkFDnEKMO3wY5mZqj6dJr490acxCheGLgN~sPW2R8U@i!u*)-pV$%_",
"J89~@$w6^t%N3)=n1o!(m+7?jzx_/U#CATPeWbacV&*YfqDK-5Q0gMuEFhivsRLpkyGHlIrdOSXZB24",
"+@?/P5_0HoX#tibqR$FJ^DdfwEKQjkZ1xAWpT4czCGYemg267Su8O9s~!&*(LB)lhnryIMNV%vU3a-=",
"Eh+D?#2RajgP/3kTsQt4d07%p6wJUFBWzcuKCqoMGNVXYZ1mr5SLlbyH8~fIeAO!@9$nxi^&*()-v=_",
"kq_&RnrfcMS1deshbuvzKZGJ36p80%ET2gWw$()O?iVX^o/#alBHjFAIQ!-CN=+tPU4DLmy57~@xY9*",
"w@^K$+NisQjlmISZ1zP238x7%-_MLoE?TbCrGqX9!&hfW(50=VO6J/kg#4d*yDUtRc~puavBA)FenHY",
"acB3FKDGLnMR4jg29@Q&*$d(ty68wmYSzbJOusEWH1!%i^)-=f_+v?CVXqr5/lI#p7~0xehANPTokUZ",
"l25m7~x@%GOBMYg$)-sPUo_EdR*DIZh460!+?/#9t&kyWS(^=vKbfirwLQ8eanjFHTVcpz1CJ3AquXN",
"lX61nh7qf5&=_io3IzTV0W!xta%BeuygsMcDOSZ$*)-HLRFA9kwNQdmrpvGEPY28K~?@+(b/CJU#j^4",
"+?vc/K#NujkoyE6Z2T8tB9s&V)XgPm-riOQ~(L=eIJp7wzYd0nD%CHA1xGq@$a4!Uh^WMFbl35*RSf_",
    NULL };

char *Documentation[] = {
    "",
    "Usage:",
    "      Enigma [options] KeyPhrase [file]  --> a Full Ascii Cypher.",
    "",
    "Options:",
    "       /D ---> Decode.",
    "       /H ---> An Historical Note.",
    "       /N ---> Neaten up the output w/ Spaces and LineFeeds.",
    "       /S ---> Silent.  Verbose is Default when Redirected.",
    "",
    " --> NOT APPROVED for Classified Information. <---",
    "",
    "Last Update: 16 July 91/EK",
    NULL};

char *History[] = {
    "",
    "   When Louis XIV King of France died in 1715, he left behind",
    "messages coded in \"The Great Cypher\" created for his exclusive",
    "use by mathematician Anotoine Rossignol.",
    "",
    "   For more than a Century, the Great Cypher held off all attacks",
    "until Commander Bazeries of the French Army's \"Black Chamber\"",
    "cracked it, discovering the secret of \"Cylinder Cyphers\".",
    "",
    "   A Century later the British cracked the German Enigma machine",
    "cypher used to encypher Field Orders.  Declassified 30 years later,",
    "the UTRA Secret Project rewrote the History of World War II.",
    "WW II's Enigma was a 7 disk Bazeries Cylinder cypher, much easier to",
    "crack than the 20/26 systems used by diplomats until the middle 50's.",
    "",
    "   This is a 40/79 Cylinder Cypher which will handle the full Ascii",
    "character set include IBM extended characters.  Pencil Pushers",
    "won't crack this one!",
    "",
    NULL };

/* Declare Globals */
int DeCode = 0, NeatFlag = 0, Silent = 0;
char HexCode[] = "~!@$%^&*()-=_+?/";
FILE *fp = stdin;

/* Declare ProtoTypes */
void Decode(void);
void Encode(void);
void Help(char **dp);
void Swizzle(char *Key);
void Usage(int ReturnCode);

main (int argc, char *argv[])
{
    char *tp1;

 /* Set Option Flags */
    for (; *argv[1] IS SLASH; argc--, argv++) {
       for (tp1 = argv[1] + 1; *tp1 != NULL; ) {
          switch (toupper(*tp1++)) {
          case  'D': DeCode++;  break;
          case  'H': Help(History);
          case  'N': NeatFlag++; break;
          case  'S': Silent++;  break;
          default:
             fprintf(stderr,"\nInvalid  Option [/%c].\n\n",*tp1); Usage(1);
          }
       }

    }

 /* Capture Key Word/Phrase */
    if (!OUTFLOW_EXISTS) Silent++;
    if (argc-- < 2) Usage(0); else Swizzle(argv[1]); argv++;

 /* Open Specified File (If Any) */
    if (argc > 2) Usage(2);
    if (argc IS 2 && (fp = fopen(argv[1], "r")) IS NULL) {
       perror(argv[1]); Usage(3); }

 /* Mainline */
    if (!Silent) fprintf(stderr, "\n"); if (Decode) NeatFlag = 0;
    if (!DeCode) Encode(); else Decode();
    if (!Silent) fprintf(stderr, "\n"); if (NeatFlag) putchar(NL);
}

void Decode(void)
{
    int c, i = 0, Flag = 0, Offset, Row = 1;
    char **Disk, *tp1;

    for (Disk = Cylinder; (c = fgetc(fp)) != EOF; Disk++) {

    /* PreFilter Text */
       if (c IS SPACE || c IS NL) { Disk--; continue; }
       if (*Disk IS NULL) { Disk = Cylinder; Row++; Row %= 40; }

    /* Set the Disk to the Letter, Offset by "Row" & Find Value */
       if ((tp1 = strchr(*Disk, c)) IS NULL) exit(1);
       Offset = 74 - Row + (tp1 - *Disk);
       c = ((*Disk)[Offset % 80]);

    /* PostProcess Text */
       if (c IS SHARP) c = SPACE;
       else if (isdigit(c));
       else if (isalpha(c));
       else if (Flag) {
          c = 16 * i +  strchr(HexCode, c) - HexCode; Flag--; }
       else { Flag++; i = strchr(HexCode, c) - HexCode; continue; }
       putchar(c); if (!Silent) fprintf(stderr, "%c", c);
    }
}

void Encode(void)
{
    int c, n, Flag = 0, Offset, Row = 0, Tally = 0;
    char **Disk;

    for (Disk = NULL; (c = fgetc(fp)) != EOF; Disk++) {

    /* PreProcess Text */
       if (c IS SPACE) c = SHARP;
       else if (isdigit(c));
       else if (isalpha(c));
       else if (Flag) Flag--;
       else { ungetc(HexCode[c & 0xF], fp); c = HexCode[c/16]; Flag++; }

    /* Set the Disk to the Letter, Offset by "Row" & Report Value */
       if (*Disk IS NULL) { Disk = Cylinder; Row++; Row %= 40; }
       Offset = (strchr(*Disk, c) - *Disk) + Row + 6;
       putchar((c = (*Disk)[Offset % 80])); if (!Silent) putch(c);
       if (!(++Tally % 5)) {
          if (!Silent)
             if (!(Tally != 60)) fprintf(stderr,"\n");  else putch(SPACE);
          if (NeatFlag)
             if (!(Tally != 60)) putchar(NL); else putchar(SPACE);
          if (Tally IS  60) Tally = 0;
       }
    }
}

void Help(char **dp)
{
    for ( ; *dp; dp++) fprintf(stderr,"    %s\n", *dp);
    exit(0);
}

void Swizzle(char *Text)
{
    int c = 1, i, j, Flag;
    int SawDigit = 0, SawUpper = 0, SawNalpha = 0, SawLower = 0;
    char Key[41], *tp1, *tp2;

 /* Load & Pack the Key */
    if (!Silent) fprintf(stderr, "Loading/Packing Key.\n");
    for (i = j = 0, tp1 = Text; i < 40; ) {
       if (!*tp1) tp1 = Key;
       if ((c = *tp1++) IS SPACE) { j++; continue; }
       else if (isdigit(c)) SawDigit++;
       else if (islower(c)) SawLower++;
       else if (isupper(c)) SawUpper++;
       else SawNalpha++;
       Key[i++] = c;
    }
    Key[40] = NULL;

 /* Check for Valid Key */
    if ((strlen(Text) - j < 6) || !SawDigit || !SawUpper || !SawLower ||
       !SawNalpha) Help(BadKey);

 /* Swizzle Disks */
    if (!Silent) fprintf(stderr, "Swizzling Code Disks.\n");
    for (j = Flag = 39; j && Flag; --j)
       for (Flag = i = 0; i < j; i++)
          if (Key[i] >  Key[i+1]) {
             c = Key[i]; Key[i] = Key[i+1]; Key[i+1] = c;
             tp1 = Cylinder[i]; Cylinder[i] = Cylinder[i+1];
             Cylinder[i+1] = tp1; Flag++;
          }
}

void Usage(int ReturnCode)
{
    char   **dp = Documentation;

    if (ReturnCode)
         fprintf(stderr, "\nBailing Out w/ Exit Code %d.\n", ReturnCode);
    for ( ; *dp; dp++) fprintf(stderr,"%s\n", *dp);
    if (OUTFLOW_EXISTS) { rewind(stdout); putchar(DOS_EOF); }
    exit(ReturnCode);
}


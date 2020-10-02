/* LOGON PROGRAM IN BDS C 
   
   r p sarna -- 19 jan 1984

   dirigo data support systems 
   p o box 259
   auburn, me 04210

*/

#include <bdscio.h>

/* Make logon id's in memory: */
char id0[11];
char id1[11];
char id2[11];
char id3[11];
char id4[11];
char id5[11];
char id6[11];
char id7[11];
char id8[11];
char id9[11];
char id10[11];
char id11[11];
char id12[11];
char id13[11];
char id14[11];
char id15[11];

/* Make passwords in memory: */
char pw0[11];
char pw1[11];
char pw2[11];
char pw3[11];
char pw4[11];
char pw5[11];
char pw6[11];
char pw7[11];
char pw8[11];
char pw9[11];
char pw10[11];
char pw11[11];
char pw12[11];
char pw13[11];
char pw14[11];
char pw15[11];

int ptr;		/* pointer (value of)  proper user number */
int unum;		/* user number */      
char idin[11];		/* logon id (answer from person logging on) */ 
char pwin[11];		/* password (answer from person logging on) */ 
char badfile;		/* flag -- can't write out 'secret' file */

main() 
{
	getstuff();	/* read 'secret' file that has legal passwords */

nogood:	clear();
	badfile = NO;
	idin[0] = '\0';
	printf("logon identification? "); 	/* ask "Who's there?" */
	getid();
	ptr = -1;
	if (strcmp(idin, id0) == 0) ptr = 0;	/* see if we know them */
	else if (strcmp(idin, id1) == 0) ptr = 1;
	else if (strcmp(idin, id2) == 0) ptr = 2;
	else if (strcmp(idin, id3) == 0) ptr = 3;
	else if (strcmp(idin, id4) == 0) ptr = 4;
	else if (strcmp(idin, id5) == 0) ptr = 5;
	else if (strcmp(idin, id6) == 0) ptr = 6;
	else if (strcmp(idin, id7) == 0) ptr = 7;
	else if (strcmp(idin, id8) == 0) ptr = 8;
	else if (strcmp(idin, id9) == 0) ptr = 9;
	else if (strcmp(idin, id10) == 0) ptr = 10;
	else if (strcmp(idin, id11) == 0) ptr = 11;
	else if (strcmp(idin, id12) == 0) ptr = 12;
	else if (strcmp(idin, id13) == 0) ptr = 13;
	else if (strcmp(idin, id14) == 0) ptr = 14;
	else if (strcmp(idin, id15) == 0) ptr = 15;

	if (ptr < 0) goto nogood;	/* we don't know them - ask again */

	pwin[0] = '\0';			/* clean out password string */

	switch (ptr) {
		case 0: {
			printf("\n\npassword? ");	/* ask password */
			getpw();
			if (strcmp(pwin, pw0) != 0) goto nogood;
			usrset( 0); 

doover:			printf("\n\nchange user number = ? (16 to save & end) ");
			scanf("%d", &unum);

			switch (unum) {

				case 0: {
/* if logged in to 0; change? */	printf("\npresent logon id: '%s'", id0);
					printf("\npresent password: '%s'", pw0);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id0, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw0, pwin);
					goto doover;
					}



				case 1: {
					printf("\npresent logon id: '%s'", id1);
					printf("\npresent password: '%s'", pw1);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id1, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw1, pwin);

					goto doover;
					}



				case 2: {
					printf("\npresent logon id: '%s'", id2);
					printf("\npresent password: '%s'", pw2);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id2, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw2, pwin);

					goto doover;
					}


				case 3: {
					printf("\npresent logon id: '%s'", id3);
					printf("\npresent password: '%s'", pw3);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id3, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw3, pwin);

					goto doover;
					}



				case 4: {
					printf("\npresent logon id: '%s'", id4);
					printf("\npresent password: '%s'", pw4);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id4, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw4, pwin);

					goto doover;
					}


				case 5: {
					printf("\npresent logon id: '%s'", id5);
					printf("\npresent password: '%s'", pw5);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id5, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw5, pwin);

					goto doover;
					}



				case 6: {
					printf("\npresent logon id: '%s'", id6);
					printf("\npresent password: '%s'", pw6);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id6, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw6, pwin);

					goto doover;
					}


				case 7: {
					printf("\npresent logon id: '%s'", id7);
					printf("\npresent password: '%s'", pw7);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id7, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw7, pwin);

					goto doover;
					}



				case 8: {
					printf("\npresent logon id: '%s'", id8);
					printf("\npresent password: '%s'", pw8);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id8, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw8, pwin);

					goto doover;
					}


				case 9: {
					printf("\npresent logon id: '%s'", id9);
					printf("\npresent password: '%s'", pw9);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id9, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw9, pwin);

					goto doover;
					}



				case 10: {
					printf("\npresent logon id: '%s'", id10);
					printf("\npresent password: '%s'", pw10);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id10, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw10, pwin);

					goto doover;
					}


				case 11: {
					printf("\npresent logon id: '%s'", id11);
					printf("\npresent password: '%s'", pw11);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id11, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw11, pwin);

					goto doover;
					}



				case 12: {
					printf("\npresent logon id: '%s'", id12);
					printf("\npresent password: '%s'", pw12);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id12, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw12, pwin);

					goto doover;
					}


				case 13: {
					printf("\npresent logon id: '%s'", id13);
					printf("\npresent password: '%s'", pw13);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id13, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw13, pwin);

					goto doover;
					}



				case 14: {
					printf("\npresent logon id: '%s'", id14);
					printf("\npresent password: '%s'", pw14);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id14, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw14, pwin);

					goto doover;
					}


				case 15: {
					printf("\npresent logon id: '%s'", id15);
					printf("\npresent password: '%s'", pw15);
					printf("\n\nnew logon identification ? ");
					scanf("%s", idin);

					strcpy(id15, idin);

					printf("\nnew password = ? ");
					scanf("%s", pwin);

					strcpy(pw15, pwin);

					goto doover;
					}

				case 16: {      
/* dump out file: */			savestuff();
					break;
					}

				default: goto nogood;
				}

			if (badfile == YES) goto nogood;
			break;
			}

		case 1: {
/* for each user:   */	printf("\n\npassword? ");
/* get his password */	getpw();
/* if wrong, abort  */	if (strcmp(pwin, pw1) != 0) goto nogood;
/* if OK set user # */	usrset( 1); 
/* all done         */	break;
			}

		case 2: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw2) != 0) goto nogood;
			usrset( 2); 
			break;
			}

		case 3: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw3) != 0) goto nogood;
			usrset( 3); 
			break;
			}

		case 4: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw4) != 0) goto nogood;
			usrset( 4); 
			break;
			}

		case 5: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw5) != 0) goto nogood;
			usrset( 5); 
			break;
			}

		case 6: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw6) != 0) goto nogood;
			usrset( 6); 
			break;
			}

		case 7: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw7) != 0) goto nogood;
			usrset( 7); 
			break;
			}

		case 8: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw8) != 0) goto nogood;
			usrset( 8); 
			break;
			}

		case 9: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw9) != 0) goto nogood;
			usrset( 9); 
			break;
			}

		case 10: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw10) != 0) goto nogood;
			usrset( 10); 
			break;
			}

		case 11: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw11) != 0) goto nogood;
			usrset( 11); 
			break;
			}

		case 12: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw12) != 0) goto nogood;
			usrset( 12); 
			break;
			}

		case 13: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw13) != 0) goto nogood;
			usrset( 13); 
			break;
			}

		case 14: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw14) != 0) goto nogood;
			usrset( 14); 
			break;
			}

		case 15: {
			printf("\n\npassword? ");
			getpw();
			if (strcmp(pwin, pw15) != 0) goto nogood;
			usrset( 15); 
			break;
			}

		}

aldone:	clear();

}


getpw()		/* routine to get password, but print '*' on screen: */
{
	char cc;
	int i;

	i = 0;

	do {
		while (bios(2,0)) ;
		cc = bios(3,0);     
		putch('*');
		pwin[i] = cc;
		i++;
		} while ((cc != '\r') && ( i < 10));

		i--;
		pwin[i] = '\0';

}


getid()		/* routine to get logon id - OK to print that */
{
	char cc;
	int i;

	i = 0;

	do {
		while (bios(2,0)) ;
		cc = bios(3,0);     
		putch(cc);
		idin[i] = cc;
		i++;
		} while ((cc != '\r') && ( i < 10));

		i--;
		idin[i] = '\0';

}


savestuff()		/* routine to save 'secret' id & passwords */
{
	int frv;
	FILE obuf;
	int i;
	char filename[16];

/* make 'T' in 'DTA' to a 0xD4 in order to make a 'SYS' file */
	filename[0] = '0';
	filename[1] = '/';
	filename[2] = 'A';
	filename[3] = ':';
	filename[4] = 'L';
	filename[5] = 'O';
	filename[6] = 'G';
	filename[7] = 'I';
	filename[8] = 'N';
	filename[9] = 'F';
	filename[10] = 'O';
	filename[11] = '.';
	filename[12] = 'D';
	filename[13] = 0xD4;	/* a 'T' with high bit set for 'sys' */
	filename[14] = 'A';
	filename[15] = '\0';

	frv = fcreat(filename, obuf);

	if (frv == 0) {    
			printf("\n\n\nFILE ERROR");
			sleep(20);
			badfile = YES;
			}

	for (i = 0; i <= 10; i++) { 		/* XOR with 'X' to confuse: */
		putc((pw0[i] ^ 'X'), obuf);	
		putc((id0[i] ^ 'X'), obuf);	/* note id, pw "staggered" */
		putc((pw1[i] ^ 'X'), obuf);
		putc((id1[i] ^ 'X'), obuf);
		putc((pw2[i] ^ 'X'), obuf);
		putc((id2[i] ^ 'X'), obuf);
		putc((pw3[i] ^ 'X'), obuf);
		putc((id3[i] ^ 'X'), obuf);
		putc((pw4[i] ^ 'X'), obuf);
		putc((id4[i] ^ 'X'), obuf);
		putc((pw5[i] ^ 'X'), obuf);
		putc((id5[i] ^ 'X'), obuf);
		putc((pw6[i] ^ 'X'), obuf);
		putc((id6[i] ^ 'X'), obuf);
		putc((pw7[i] ^ 'X'), obuf);
		putc((id7[i] ^ 'X'), obuf);
		putc((pw8[i] ^ 'X'), obuf);
		putc((id8[i] ^ 'X'), obuf);
		putc((pw9[i] ^ 'X'), obuf);
		putc((id9[i] ^ 'X'), obuf);
		putc((pw10[i] ^ 'X'), obuf);
		putc((id10[i] ^ 'X'), obuf);
		putc((pw11[i] ^ 'X'), obuf);
		putc((id11[i] ^ 'X'), obuf);
		putc((pw12[i] ^ 'X'), obuf);
		putc((id12[i] ^ 'X'), obuf);
		putc((pw13[i] ^ 'X'), obuf);
		putc((id13[i] ^ 'X'), obuf);
		putc((pw14[i] ^ 'X'), obuf);
		putc((id14[i] ^ 'X'), obuf);
		putc((pw15[i] ^ 'X'), obuf);
		putc((id15[i] ^ 'X'), obuf);
		}

	fclose(obuf);

}


getstuff()		/* get info from 'secret' id & password file */
{
	int frv;
	FILE ibuf;
	int i;

	frv = fopen("0/A:LOGINFO.DTA", ibuf);

	if (frv == 0) {    
			printf("\n\n\nFILE ERROR");
			sleep(20);
			badfile = YES;
			}

	for (i = 0; i <= 10; i++) { 	/* XOR w/ 'X' to get info back */
		pw0[i] = getc(ibuf) ^ 'X';
		id0[i] = getc(ibuf) ^ 'X';
		pw1[i] = getc(ibuf) ^ 'X';
		id1[i] = getc(ibuf) ^ 'X';
		pw2[i] = getc(ibuf) ^ 'X';
		id2[i] = getc(ibuf) ^ 'X';
		pw3[i] = getc(ibuf) ^ 'X';
		id3[i] = getc(ibuf) ^ 'X';
		pw4[i] = getc(ibuf) ^ 'X';
		id4[i] = getc(ibuf) ^ 'X';
		pw5[i] = getc(ibuf) ^ 'X';
		id5[i] = getc(ibuf) ^ 'X';
		pw6[i] = getc(ibuf) ^ 'X';
		id6[i] = getc(ibuf) ^ 'X';
		pw7[i] = getc(ibuf) ^ 'X';
		id7[i] = getc(ibuf) ^ 'X';
		pw8[i] = getc(ibuf) ^ 'X';
		id8[i] = getc(ibuf) ^ 'X';
		pw9[i] = getc(ibuf) ^ 'X';
		id9[i] = getc(ibuf) ^ 'X';
		pw10[i] = getc(ibuf) ^ 'X';
		id10[i] = getc(ibuf) ^ 'X';
		pw11[i] = getc(ibuf) ^ 'X';
		id11[i] = getc(ibuf) ^ 'X';
		pw12[i] = getc(ibuf) ^ 'X';
		id12[i] = getc(ibuf) ^ 'X';
		pw13[i] = getc(ibuf) ^ 'X';
		id13[i] = getc(ibuf) ^ 'X';
		pw14[i] = getc(ibuf) ^ 'X';
		id14[i] = getc(ibuf) ^ 'X';
		pw15[i] = getc(ibuf) ^ 'X';
		id15[i] = getc(ibuf) ^ 'X';
		}

	fclose(ibuf);

}


usrset(u)	/* routine to change which user area we're logged into */
int u;
{
	char *addr;	/* pointer to CP/M 'user' address */

	addr = 0x0004;

	*addr = (*addr & 0x0F) + (u * 16); /* user number is left nibble */

}

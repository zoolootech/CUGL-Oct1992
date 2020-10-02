
#include <stdio.h>
#include <io.h>
#include <stdek.h>
#include <string.h>

/* TEE ---> Squirt Stdin-Stdout Stream into a Specified File
 *
 * J.Ekwall 20 September 1989
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * Last Update: 20 September 1989
 */

/* Declare ProtoTypes */
void PX_Pipe(char *);
void Usage(void);

main (int argc, char *argv[])
{
    int c;
    char File[128];
    FILE *fp;

 /* Check for Parameters */
    if (INFLOW_EXISTS IS FALSE) Usage();
    if (argc != 2) Usage();
    strcpy(File,argv[1]); if (*File IS '$') PX_Pipe(File);
    if ((fp = fopen(File,"w")) IS NULL) { perror(File); exit(1); }

 /* Do Business */
    while ((c = getchar()) != EOF) { putc(c,fp); putchar(c); }
    fclose(fp);
}

void PX_Pipe(char *Text)
{
    char Tbuf[128];

    strcpy(Tbuf,Text+ 1);
    if ((strlen(Tbuf) < 1) || (strlen(Tbuf) > 4)) return;
    strcpy(Text,"\\STD "); strcat(Text,Tbuf);
}

void Usage(void)
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr,
    "       TEE file_name ---> Squirt Stdin/out Text into a File.\n\n");
    fprintf(stderr,
    "       Full Pathname OK.  PX Pipes ($Pipe) are Legal.\n\n");
    exit(1);
}

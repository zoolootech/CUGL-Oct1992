

/*
 *      HEADER:         ;
 *	TITLE:          Phone Book;
 *      DATE:           10/28/89;
 *      DESCRIPTION:    "quick and dirty telephone directory to
 *                      illustrate the use of blksplit() & stristr()";
 *      VERSION:        1.0;
 *      FILENAME:       PB.C;
 *      USAGE:          PB [search string];
 *      SEE-ALSO:       STRISTR.C, BLKSPLIT.C;
 *      AUTHORS:        Michael Kelly;
 *      WARNINGS:       "Requires STRISTR.OBJ and BLKSPLIT.OBJ.
 *                      Telephone data file must be ASCII text with
 *                      one record per line.
 *                      Data file name is #defined as FILENAME";
 *
 *      COMMENTS:       Tested using TubroC V 2.0  Large Model.
 *                      Should be easy port to any ANSI C;
 */



/*
 *    ***    Phone Book    ***
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "blksplit.h"
#include "stristr.h"


/*	#define DEBUG 1		*/


/*
 *  maximum # of char * in array
 */
#define MAX_PHONE_NUMS 500


/*
 *  Phone Book data file
 */
#define FILENAME "phonebk.dat"

/*
 *  maximum length of substring + 1
 */
#define REPLY_SIZE 128


/*
 *  module functions
 */
static void error_exit(char *errormsg, int line_no);
static int get_data(void);

/*
 *  global variables
 */
FILE *fp;
char filename[] = FILENAME;
char diskbuf[BUFSIZ];
long length = 0L;
char **phonenum;
char *buffer;

/*
 *  set record_sep to end of line delimiter(s) used on your file system
 */
#if defined(__MSDOS__)
char record_sep[] = "\r\n";
#else
char record_sep[] = "\n";
#endif

/*
 *  Main
 */
int main(int argc, char *argv[])
{
    register int num_rec = 0, cur_rec = 0;
    int matches = 0;
    char reply[REPLY_SIZE] = "";


    phonenum = (char **) calloc(MAX_PHONE_NUMS,sizeof(char *));
    if(!phonenum)
	error_exit("Not enough memory : Program terminating.",__LINE__);

    num_rec = get_data();

    if(num_rec < 1)
	error_exit("Data file " FILENAME " invalid!",__LINE__);

#ifdef DEBUG

    for(cur_rec = 0;cur_rec < num_rec;cur_rec++)  {
	size_t k;
	k = strlen(phonenum[cur_rec]);
	if(k) --k;

	printf("\n%s\nFirst char = %d  Last char = %d",
	    phonenum[cur_rec],phonenum[cur_rec][0],
	    phonenum[cur_rec][k]
	);
    }

    printf("\n\tNum records = %d\n",num_rec);

    gets(reply);

#endif


    if(num_rec < 1)
	error_exit("Data file " FILENAME " invalid!",__LINE__);


    printf("\n\n\t\t\tP h o n e  B o o k\n");

    /*
     *  command process loop
     */
    do  {

	if(argc < 2)  {

	    printf("\n\t\t***   Press <Enter> to Quit   ***");
	    printf(
		"\n\n\tEnter search string  { not case sensitive }\n\n\t>  "
	    );
	    gets(reply);
	    reply[REPLY_SIZE] = '\0';

	}

	else  {

	    strncpy(reply,argv[1],REPLY_SIZE - 1);
	    --argc;

	}

	if(!*reply)
	    break;

	matches = 0;

	printf("\n");

	for(cur_rec = 0;cur_rec < num_rec;cur_rec++)
	    if(stristr(phonenum[cur_rec],reply))  {
		matches++;
		printf("\n%s",phonenum[cur_rec]);
	    }

	printf("\n\n\t%d record(s) matching \n\t>  %s\n",matches,reply);

    } while(*reply);



    if(phonenum)
	free(phonenum);

    if(buffer)
	free(buffer);

    return(EXIT_SUCCESS);
}


/*
 *  support functions
 */


/*
 *  display error message and exit
 */
static void error_exit(char *errormsg, int line_no)
{
    fprintf(stderr,"\n\t%s",errormsg);


#ifdef DEBUG
    if(line_no)
	fprintf(stderr,"\t(# %d)\n",line_no);
#endif

    exit(EXIT_FAILURE);
}

/*
 *  gets file size, allocates memory for and reads data file, then
 *  calls blksplit() to store address of each record.
 *
 *  returns # of file lines read
 *
 */
static int get_data()
{

    fp = fopen(filename,"rb");
    if(!fp)
	error_exit("Cannot open " FILENAME,__LINE__);

    /*
     *  determination of file length may not be portable
     */
    length = lseek(fileno(fp),0L,SEEK_END);
    rewind(fp);

    if(length == -1L)
	error_exit("Error determining input file length",__LINE__);

    else  {

	/*
	 *  allocate extra byte so block is NULL terminated
	 */
	buffer = (char *) calloc(1,(size_t)(length + 1));
	if(!buffer)
	    error_exit("Not enough memory : Program terminating.",__LINE__);

	if(setvbuf(fp,diskbuf,_IOFBF,BUFSIZ))
	    error_exit("Data file buffer error",__LINE__);

	if(!fread(buffer,(size_t)length,1,fp))
	    error_exit("Error reading data file.",__LINE__);

	fclose(fp);

    }

    return(blksplit(buffer,phonenum,record_sep,MAX_PHONE_NUMS));
}



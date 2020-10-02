/*
	bbscfile.c

	Support routines used by BBSc.c to do file i/o for the
	message file.
				Mike Kelly

	06/12/83 v1.0	written
	07/07/83 v1.0	updated
*/

#include "bdscio.h"
#include "bbscdef.h"


#define LASTDATE  " 07/07/83 "

#define PGMNAME "BBSCFILE "
#define VERSION " 1.0 "


/*	.	t.	e.	s.	t.	.	


main(argc,argv)
char	**argv;
int	argc;
{

	char	xtra[500];

	printf("starting %s\n",PGMNAME);
	debug = 1;
	strcpy(h_date,"01/01/83");
	h_next = 1;

	hdrwrt();
	hdrread();
	strcpy(h_date,"02/01/83");
	h_next = 4;
	hdrwrt();
	hdrread();
	strcpy(xtra,".......160.......170.......180.......190.......200.......210.......220.......230.......240.......250.......260.......270.......280.......290");
	strcpy(msg_text,"1.......10........20........30........40........50........60........70........80........90.......100.......110.......120.....128.........140.......150");
	strcat(msg_text,xtra);
	strcpy(msg_date,"01/01/83");
	strcpy(msg_time,"01:01:10 PM");
	strcpy(msg_to,"whom it may ");
	strcpy(msg_from,"that guy");
	strcpy(msg_pass,"abc");
	strcpy(msg_subject,"this and that");
	if (debug)
	{
		printf("after init, msg_text = ");
		prthex(msg_text);
		printf("\n");
	}
	h_next = 0;
	msgwrt();
	msgwrt();

	strfill(msg_text,0,800);
	msgread(4);
	printf("msg 4 = ");
	prthex(msg_text);
	printf("\n");

	msgread(0);
	printf("msg 0 = ");
	prthex(msg_text);
	printf("\n");

	printf("ending %s\n",PGMNAME);
}

	.	t.	e.	s.	t.	.	*/


hdrwrt()		/* write the header from memory variables */
{			/* header is a 1 record file */
	int	fd;
	char	buf128[MSGSECT];

	if ((fd = open(HEADER,2)) == ERROR)	/* open i/o */
	{
		printf("can't open header.bbs will create it\n");
		if ((fd = creat(HEADER)) == ERROR)
		{
			printf("can't create header.bbs - aborting\n");
			return(ERROR);
		}
	}

	itoa(h_next_msg,h_next);	/* convert int to char */
	strfill(buf128,26,MSGSECT);		/* init buf128 to all hex 1a */
	sprintf(buf128,"%s~%s~",	/* build record */
		h_next_msg,
		h_date);
#ifdef DEBUG
	if (debug)
	{
		printf("after sprintf buf128 = ");
		prthex(buf128);
		printf("\n");
	}
#endif
	write(fd,buf128,1);		/* write it */
	close(fd);			/* no need to leave it open */
	return(OK);
}

hdrread()		/* read the header file into memory */
{
	int	fd,
		cnt;
	char	buf128[MSGSECT];

	if ((fd = open(HEADER,0)) == ERROR)	/* open input */
	{
		printf("can't open header.bbs \n");
		h_next = 0;
		h_next_msg[0] = '0';
		h_date[0] = '0';
		hdrwrt();
	}
	read(fd,buf128,1);
	cnt = sscanf(buf128,"%s~%s~",
			h_next_msg,
			h_date);
#ifdef DEBUG
	if (debug)
	{
		printf("after sscanf buf128 = ");
		prthex(buf128);
		printf("\n");
		printf("count returned from sscanf = %d\n",cnt);
		printf("h_next_msg  = %s\n",h_next_msg);
		printf("h_date = %s\n",h_date);
	}
#endif
	close(fd);		/* no need to leave it open */

	if (cnt != 2)
	{
		return(ERROR);
	}
	h_next = atoi(h_next_msg);
	return(OK);
}

msgopen(how)
int	how;		/* how to open 0=input, 1=output, 2=i/o */
{
	int	fd;

	if ((fd = open(MESSAGES,how)) == ERROR)	/* open i/o */
	{
		printf("can't open %s will create it\n",MESSAGES);
		if ((fd = creat(MESSAGES)) == ERROR)
		{
			printf("can't create %s - aborting\n",MESSAGES);
			return(ERROR);
		}
	}

#ifdef DEBUG
	if (debug)
	{
		printf("in msgopen - fd = %d\n",fd);
	}
#endif
	return(fd);
}

msgclose(fd)
int	fd;
{

#ifdef DEBUG
	if (debug)
	{
		printf("in msgclose - fd = %d\n",fd);
	}
#endif
	return(close(fd));
}

msgwrt(fd)		/* write the message file from memory variables */
int	fd;		/* writes a message starting with the h_next msg # */
{
	int	rc,			/* return code */
		cnt1,
		cnt2,
		len;
	char	bufmsg0[MSG1MAX+1],
		buf128[MSGSECT],
		this1[10],
		next1[10];

	rc = cnt1 = len = cnt2 = 0;
#ifdef DEBUG
	if (debug)
	{
		printf("in msgwrt\n");
	}
#endif
	rc = seek(fd,h_next,0);		/* seek to next available sector */
#ifdef DEBUG
	if (debug)
	{
		printf("file open\n");
		printf("seek = %d\n",rc);
	}
#endif
	itoa(this1,h_next);		/* convert int to char */
	h_next++;
	itoa(next1,h_next);

#ifdef DEBUG
	if (debug)
	{
		printf("before strfill of nulls\n");
		printf("\n");
	}
#endif
	strfill(buf128,0,MSGSECT);		/* init buf128 to all hex 00 */

/*
*			build first piece of msg record
*/

	sprintf(buf128,"%s~%s~%s~%s~%s~%s~%s~%s~%s~",	/* build record */
		this1,					/* this rcd # */
		next1,					/*  points to next rcd # */
		msg_delete,				/* delete byte */
		msg_date,
		msg_time,
		msg_to,
		msg_from,
		msg_pass,
		msg_subject);
#ifdef DEBUG
	if (debug)
	{
		printf("before write of 1st buf128 = ");
		prthex(buf128);
		printf("\n");
	}
#endif
	cnt1 = tell(fd);
#ifdef DEBUG
	if (debug)
	{
		printf("tell() value = %d\n",cnt1);
	}
#endif
	rc = write(fd,buf128,1);	/* write the first 128 byte record */
					/*  for a message record */
#ifdef DEBUG
	if (debug)
	{
		printf("after write of 1st rcd return code = %d\n",rc);
	}
#endif
/*
*			build the n+1 piece of msg record
*/

	len = (strlen(msg_text) / MSG1MAX) + 1; /* calc how many more 128 */
						/*  byte records to write */
#ifdef DEBUG
	if (debug)
	{
		printf("length of msg_text/128 + 1 = %d\n",len);
	}
#endif
	cnt2 = 1;			/* init for substr */
	while (len--)
	{
		itoa(this1,h_next);		/* calc/convert record #'s */
		h_next++;
		if (len == 0)
		{
			strcpy(next1,"0");	/* marks last 128 byte piece */
		}				/*  of a msg */
		else
		{
			itoa(next1,h_next);
		}
		strfill(bufmsg0,0,MSG1MAX);
		substr(msg_text,bufmsg0,cnt2,MSG1MAX); /* move MSG1MAX bytes to buffer */
#ifdef DEBUG
		if (debug)
		{
			printf("after substr bufmsg0 = ");
			prthex(bufmsg0);
			printf("\n");
		}
#endif
		cnt2 += MSG1MAX;		/* up cnt2 by MSG1MAX */
#ifdef DEBUG
		if (debug)
		{
			printf("after add cnt2 = %d\n",cnt2);
		}
#endif
		strfill(buf128,0,MSGSECT);	/* init buf128 to all hex 00 */
		sprintf(buf128,"%s~%s~%s~%s~",
			this1,			/* this rcd # */
			next1,			/* point to next rcd # */
			msg_delete,		/* delete byte */
			bufmsg0);		/* piece of msg */
#ifdef DEBUG
		if (debug)
		{
			printf("after sprintf, msg_text = ");
			prthex(msg_text);
			printf("\n");
			printf("after sprintf, buf128 = ");
			prthex(buf128);
			printf("\n");
			printf("after sprintf, bufmsg0 = ");
			prthex(bufmsg0);
			printf("\n");
		}
#endif
		cnt1 = tell(fd);
#ifdef DEBUG
		if (debug)
		{
			printf("tell() value = %d\n",cnt1);
		}
#endif
		rc = write(fd,buf128,1);	/* write the n+1 128 byte record */
#ifdef DEBUG
		if (debug)
		{
			printf("after write of n+1 return code = %d\n",rc);
			printf("after write of n+1 buf128 = ");
			prthex(buf128);
			printf("\n");
		}
#endif
	}

	strfill(buf128,26,MSGSECT);		/* fill with all hex 1a */
	cnt1 = tell(fd);
#ifdef DEBUG
	if (debug)
	{
		printf("tell() value = %d\n",cnt1);
	}
#endif
	rc = write(fd,buf128,1);	/* write the all hex 1a 128 byte record */
#ifdef DEBUG
	if (debug)
	{
		printf("after write of all hex 1a return code = %d\n",rc);
		printf("after write of hex 1a buf128 = ");
		prthex(buf128);
		printf("\n");
		printf("leaving msgwrt\n");
	}
#endif
	return(OK);
}

msgrewrt(fd,r_msg)	/* re-write the message file from memory variables */
int	fd,		/* re-writes only the 1st part of a message */
	r_msg;		/* used to update the delete byte */
{
	int	rc,			/* return code */
		cnt1,
		file_size;
	char	buf128[MSGSECT],
		this1[10],
		next1[10];

	rc = cnt1 = 0;
#ifdef DEBUG
	if (debug)
	{
		printf("in msgrewrt\n");
		printf("rewrite msg = %d\n",r_msg);
	}
#endif
	file_size = rcfsiz(fd);		/* find how many 128 byte sectors */
#ifdef DEBUG
	if (debug)			/*  are in the file and don't seek */
	{				/*  past that */ 
		printf("file_size = %d<%04x>\n",file_size,file_size);
	}
#endif
	if (r_msg >= (file_size - 1))	/* don't try to seek past end of file */
	{
		return(ERROR);
	}
	if ((rc = seek(fd,r_msg,0)) == ERROR)	/* seek to requested sector */
	{
		return(ERROR);
	}
#ifdef DEBUG
	if (debug)
	{
		printf("file open\n");
		printf("seek = %d\n",rc);
	}
#endif
	itoa(this1,r_msg);		/* convert int to char */
	r_msg++;
	itoa(next1,r_msg);
#ifdef DEBUG
	if (debug)
	{
		printf("before strfill of nulls\n");
		printf("\n");
	}
#endif
	strfill(buf128,0,MSGSECT);		/* init buf128 to all hex 00 */

/*
*			build first piece of msg record
*/

	sprintf(buf128,"%s~%s~%s~%s~%s~%s~%s~%s~%s~",	/* build record */
		this1,					/* this rcd # */
		next1,					/*  points to next rcd # */
		msg_delete,				/* delete byte */
		msg_date,
		msg_time,
		msg_to,
		msg_from,
		msg_pass,
		msg_subject);
#ifdef DEBUG
	if (debug)
	{
		printf("before re-write of 1st buf128 = ");
		prthex(buf128);
		printf("\n");
		cnt1 = tell(fd);
		printf("tell() value = %d\n",cnt1);
	}
#endif
	rc = write(fd,buf128,1);	/* write the first 128 byte record */
					/*  for a message record */
#ifdef DEBUG
	if (debug)
	{
		printf("after rewrite of 1st rcd return code = %d\n",rc);
	}
#endif
	return(OK);
}


msgread(fd,msgno)		/* read message number requested */
int	fd,			/* returns ERROR if msg past eof */
	msgno;			/* returns 0 if msg is not 1st piece */
				/*   of a message */
				/* returns 0 if msg is deleted */
				/* returns msg # if successful */
{
	int	rc,			/* return code */
		cnt1,
		cnt2,
		len,
		next,
		ret_this,
		file_size;
	char	bufmsg0[MSG1MAX+1],
		buf128[MSGSECT+256],
		this1[10],
		next1[10];

#ifdef DEBUG
	if (debug)
	{
		printf("in msgread-message to read = %d\n",msgno);
		printf("fd passed = %d\n",fd);
	}
#endif
	file_size = rcfsiz(fd);		/* find how many 128 byte sectors */
#ifdef DEBUG
	if (debug)			/*  are in the file and don't seek */
	{				/*  past that */ 
		printf("file_size = %d<%04x>\n",file_size,file_size);
	}
#endif
	if (msgno >= (file_size - 1))	/* don't try to seek past end of file */
	{
		return(ERROR);
	}
	if ((rc = seek(fd,msgno,0)) == ERROR)
	{
		printf("can't seek on %s\n",MESSAGES);
		printf("rc on seek to record %d = %d<%04x>\n",msgno,rc,rc);
		return(ERROR);		/* when cant find it */
	}
#ifdef DEBUG
	if (debug)
	{
		printf("after seek of 1st sector, seek = %d\n",rc);
	}
#endif
	if ((rc = read(fd,buf128,1)) == ERROR)	/* read one 128 byte sector */
	{
		printf("can't read %s\n",MESSAGES);
		printf("rc on read to 1st record %d = %d<%04x>\n",
			msgno,rc,rc);
		return(ERROR);
	}
#ifdef DEBUG
	if (debug)
	{
		printf("after read rc = %d\n",rc);
	}
#endif
/*
*			get first piece of msg record
*/
	rc = sscanf(buf128,"%s~%s~%s~%s~%s~%s~%s~%s~%s~",
		this1,				/* this rcd # */
		next1,				/*  points to next rcd # */
		msg_delete,			/* delete byte */
		msg_date,
		msg_time,
		msg_to,
		msg_from,
		msg_pass,
		msg_subject);
#ifdef DEBUG
	if (debug)
	{
		printf("after sscanf of 1st piece - rc = %d\n",rc);
		printf("msg_delete = ");
		prthex(msg_delete);
		printf("\n");
	}
#endif
	if (rc != 9)		/* makes sure we read the 1st piece */
	{			/*  of a message and not in the middle */
#ifdef DEBUG
		if (debug)
		{
			printf("rc != 9\n");
		}
#endif
		return(0);	/* 0 when is not the msg header */
	}

	if (msg_delete[0] == '9')	/* check for deleted messages */
	{				/*  if so, return as if not found */
#ifdef DEBUG
		if (debug)
		{
			printf("msg_delete[0] = 9\n");
		}
#endif
		return(0);
	}

	ret_this = atoi(this1);	/* return this msg no. */
	next = atoi(next1);
	strcpy(msg_no,this1);
	msg_text[0] = '\0';
#ifdef DEBUG
	if (debug)
	{
		printf("after sscanf of 1st sector, buf128 = ");
		prthex(buf128);
		printf("\n");
		printf("this1 = %s   next1 = %s\n",this1,next1);
		printf("after atoi, next = %d\n",next);
	}
#endif
	while (next)			/* read until no more pieces for */
	{				/*  this message */
		if ((rc = read(fd,buf128,1)) == ERROR)	/* read next 128 byte sector */
		{
			printf("can't read %s\n",MESSAGES);
			printf("rc on n+1 read to record %d = %d<%04x>\n",
				msgno,rc,rc);
			return(ERROR);
		}
#ifdef DEBUG
		if (debug)
		{
			printf("rc on n+1 read to record %d = %d<%04x>\n",
				msgno,rc,rc);
		}
#endif
		strfill(bufmsg0,0,MSG1MAX);	/* init bufmsg0 to all hex 00 */
		rc = sscanf(buf128,"%s~%s~%s~%s~",
			this1,			/* this rcd # */
			next1,			/* point to next rcd # */
			msg_delete,		/* delete byte */
			bufmsg0);		/* piece of msg */
		next = atoi(next1);
#ifdef DEBUG
		if (debug)
		{
			printf("return from sscanf = %d\n",rc);
			printf("after sscanf of n+1, buf128 = ");
			prthex(buf128);
			printf("\n");
			printf("this1=%s  next1=%s  next=%d\n",
				this1,next1,next);
		}
#endif
		strcat(msg_text,bufmsg0);	/* tag piece of msg to */
						/*  whole msg array */
	}
	return(ret_this);	/* if all ok, return the msg no. found */
}

/*	end of program  	*/

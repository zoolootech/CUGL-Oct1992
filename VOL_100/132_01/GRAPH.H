#DEFINE	NBYTES	32
#DEFINE	PIXBYT	8	/* number of x pixels per byte, hi res */
#DEFINE NULL 0
#DEFINE	EOF -1
#DEFINE	UPDATE	32768
#DEFINE	UPPG0	16384
#DEFINE	UPPG1	8192	/* master,page0,page1 update flags */
#DEFINE	UPALL	57344	/* Update both pages and flag it   */
#DEFINE MOVER	128
#DEFINE ERASE	64
#DEFINE WRITE	32
#DEFINE	NEWOBJ	16
#DEFINE	EXPLODE	8
#DEFINE BLANKED 1
/*
	Define list positions.
*/
#DEFINE FORWARD 0
#DEFINE NUMBER  1
#DEFINE X	2
#DEFINE Y	3
#DEFINE FLAGS	4
#DEFINE OBJPTR  5
/*
	Page block stops here. Master block continues.
*/
#DEFINE	XINCR	6
#DEFINE	YINCR	7
#DEFINE	XACCUM	8
#DEFINE	YACCUM	9
#DEFINE	LISTP	10
#DEFINE	COUNT	11
#DEFINE	PROC	12
#DEFINE	PROCEXP	13
#DEFINE	CLASS	14
#DEFINE	SPAREF	15
#DEFINE MASLEN  16  /* length of master list block */
#DEFINE PGLEN  6  /* length of a display page block */
#DEFINE NMASTER 160 /* total size of master array (10 OBJECTS) */
#DEFINE NPAGE	60 /* total size of page list (10 objects)    */
/*
	Screen Parameters
*/
#DEFINE PAGE0	1536
#DEFINE PAGE1	7680
#DEFINE	PGSIZE	6144
/*
	List offsets in an object definition block
*/
#DEFINE	OBJNUM	1
#DEFINE	XMAX	2
#DEFINE	YMAX	3
/*
	Joystick locations
*/
#DEFINE	JOYRX 	346
#DEFINE	JOYRY	347
#DEFINE	JOYLX	348
#DEFINE	JOYLY	349	/* joystick left y and x, joystick right */

� _ѯ���#7��#y��������y����^>E�>�2�82�8�P��>�w##^#V
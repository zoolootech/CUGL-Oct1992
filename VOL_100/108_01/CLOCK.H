#define CLOCK	32		/* Base port for Mountain Hardware clock */
#define USEC100	CLOCK		/* Hundreds of microseconds digit port */
#define MSEC1	CLOCK+1		/* Unit milliseconds digit port */
#define MSEC10	CLOCK+2		/* Tens of milliseconds digit port */
#define MSEC100	CLOCK+3		/* Hundreds of milliseconds digit port */
#define SEC1	CLOCK+4		/* Unit seconds digit port */
#define SEC10	CLOCK+5		/* Tens of seconds digit port */
#define MIN1	CLOCK+6		/* Unit miniutes digit port */
#define MIN10	CLOCK+7		/* Tens of minutes digit port */
#define HOUR1	CLOCK+8		/* Unit hours digit port */
#define HOUR10	CLOCK+9		/* Tens of hours digit port */
#define DAY1	CLOCK+10	/* Unit days digit port */
#define DAY10	CLOCK+11	/* Tens of days digit port */
#define DAY100	CLOCK+12	/* Hundreds of days digit port */
#define DAY1K	CLOCK+13	/* Thousands of days digit port */
#define DAY10K	CLOCK+14	/* Ten of thousands of days digit port */
#define CINTER	CLOCK+15	/* Interupt set port */
#define MASK	0X000F		/* Digit bit mask, lower 4 bits BDC digit */

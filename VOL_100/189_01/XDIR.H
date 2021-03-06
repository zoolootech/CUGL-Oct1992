/* fdir.h       extended file info */

#define NORMAL 0
#define RDONLY 0x01
#define HIDDEN 0x02
#define SYSTEM 0x04
#define VOLUME 0x08
#define SUBDIR 0x10
#define ARCHIV 0x20
#define FILES  0x27
#define ALL    0x3f
#define MOST   0x37 /* all except volume */

struct DTA {
    char reserved[21];
    char attribute;
    unsigned sec: 5;
    unsigned min: 6;
    unsigned hour: 5;
    unsigned day: 5;
    unsigned month: 4;
    unsigned year: 7;
    long size;
    char name[13];
};

struct file_info {
    char *name;
    char attr;
    long size;
    char mon;
    char day;
    char year;
    char hour;
    char min;
};

extern xdir();
              �}" �  CASE    C             �t�0�  FTRIM   C             ڐ} V  
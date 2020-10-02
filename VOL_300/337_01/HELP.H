/**********************   HELP.H   ***************************/

/* function prototypes */

/* makehelp.c */

void edit(int x, int y);
int getsize(void);
int val_field( char *string,int length,int field_number);

/* hlp_menu.c */

void main_menu(void);
int browse(void);
int file(void);
int verify_save(void);
int add(void);
void status(int page);

/* hlp_io */

void get_name(char *filename);
int save_page(int page);
int load_page(int page);
int append(char *filename, char *buffer, int page_size);

struct hlp{
        int width;            /* the width of the edit window */
        int height;           /* the height of the edit window */
        char filename[80];    /* name of the help file in use */
        int number_pages;     /* the number of pages in the file */
        int edit;             /* handle for edit window */
        int menu;             /* handle for the menu window */
        int status;           /* handle for the status window */
        };

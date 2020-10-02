#define NULLF (int (*)()) 0

/*
These variables are assigned from within the panel_execute
function.  They indicate the row, column and field where the
cursor was positioned prior to the user pressing a response
key.
*/
extern int pan_row, pan_col, pan_field_sub;
extern char pan_field_name[];

void      pan_init();

void      pan_activate(char []);
void      pan_destroy();
void      pan_error(int, int, char *);

unsigned  pan_execute(char [], int, int);
unsigned  pan_get_key();

void      pan_sound(int, int);
void      pan_get_field(char *, int, char []);
void      pan_put_field(char *, int, char []);
void      pan_delay(int);
void      pan_field_attr(char *, int, char []);

void      pan_idle_func(int (*)());
void      pan_resp_keys(unsigned *);

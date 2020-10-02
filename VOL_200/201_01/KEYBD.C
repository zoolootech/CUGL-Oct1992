#define TRUE    1
#define FALSE   0
#define CNTL_C  3

#define STOPPED 1
#define RUNNING 2

char got_char = FALSE;
       /* either true or false */
char gotten_char;

int state = STOPPED;

/* keyboard ready -- Returns true
   or false, according to whether not a character
   is available from the keyboard.  It can be
   called repeatedly before calling kbd_in() */

kbd_rdy()
{
if (got_char)
    return TRUE;

#asm
    mov dl,255
    mov ah,6
    int 21h
    mov sp,bp
    jz  not_rdy
    mov byte got_char_,1
    mov byte gotten_char_,al
   not_rdy:
#

return (int) got_char;
}

/* keyboard in -- Returns a character,
   as raw input, from the keyboard.  The NULL
   character, 0 (CNTL @), can be returned */

kbd_in()
{
while ( ! kbd_rdy())
   {}
got_char = FALSE;
return (int) gotten_char;
}

/* keyboard character -- Returns a character,
   as raw input, from the keyboard.  Control-C is
   flagged for exiting the program. This routine
   includes a background process of putting
   periods to the console.  */

kbd_ch()
{
int ci;

for (;;) {
    if (kbd_rdy()) {
        if ((ci = kbd_in()) == CNTL_C)
            exit();
        return ci;
        }
    
    /* Virtually any background process could
       be carried out here.  It could be driven
       as a finite state machine.  The current
       state of the process could be stored in
       a state variable and examined with a switch
       statement.     */
    
    if (state == RUNNING)
        co('.');
    }
}


/* keyboard purge -- Eliminates any buffered
   keystrokes.  It uses raw input. */

kbd_purge()
{

while (kbd_rdy())
    kbd_in();
}

/******************************************/

main()
{
int ci;

kbd_purge();
for (;;) {
    ci = kbd_ch();
    if (ci == '.') {
        if (state == STOPPED)
            state = RUNNING;
        else
            state = STOPPED;
        }
    co(ci);
    /* co(...) is a DesMet C library function
       for direct console output
       (teletypewriter-style) */
    }
}

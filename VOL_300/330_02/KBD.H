/*
   --- Version 2.1 90-10-12 10:34 ---

   KBD.H - CTask - Keyboard hotkey scan-code and flag definitions.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/


/* flags1: Keyboard flag byte 1 (at 0040:0017) */

#define KF1_RIGHTSHIFT   0x01     /* right shift key down */
#define KF1_LEFTSHIFT    0x02     /* left shift key down */
#define KF1_CONTROL      0x04     /* either control key down */
#define KF1_ALT          0x08     /* either alt key down */
#define KF1_SCROLLOCK    0x10     /* scroll lock toggle */
#define KF1_NUMLOCK      0x20     /* num lock toggle */
#define KF1_CAPSLOCK     0x40     /* caps lock toggle */
#define KF1_INSLOCK      0x80     /* ins toggle */

/* flags2: Keyboard flag byte 2 (at 0040:0018) */

#define KF2_LEFT_CTL     0x01     /* left control key down */
#define KF2_LEFT_ALT     0x02     /* left alt key down */
#define KF2_SYSREQKEY    0x04     /* sys req key down */
#define KF2_PAUSELOCK    0x08     /* pause toggle */
#define KF2_SCRLOCKKEY   0x10     /* scroll lock key down */
#define KF2_NUMLOCKKEY   0x20     /* num lock key down */
#define KF2_CAPSKEY      0x40     /* caps lock key down */
#define KF2_INSKEY       0x80     /* ins key down */

/* flags3: AT enhanced BIOS only keyboard flags at 0040:0096 */

#define KF3_LAST_E1      0x01     /* Last scancode was E1 prefix */
#define KF3_LAST_E0      0x02     /* Last scancode was E0 prefix */
#define KF3_RIGHT_CTL    0x04     /* right control key down */
#define KF3_RIGHT_ALT    0x08     /* right alt key (AltGr key) down */
#define KF3_IS_101       0x10     /* 101/102-key keyboard present */
#define KF3_FORCE_NUML   0x20     /* force numlock (init only) */
#define KF3_LAST_FIRSTID 0x40     /* last was read first ID (init only) */
#define KF3_READ_ID      0x80     /* read ID (init only) */

/* AT only keyboard flags at 0040:0097 (not used for hotkeys) */

#define KF4_LED_SCRLOCK  0x01     /* scroll lock LED state */
#define KF4_LED_NUMLOCK  0x02     /* num lock LED state */
#define KF4_LED_CAPLOCK  0x04     /* caps lock LED state */
#define KF4_USKEYB       0x08     /* US layout (some KEYBxx drivers) */
#define KF4_ACK          0x10     /* ACK received */
#define KF4_RESEND       0x20     /* Resend received */
#define KF4_LED_UPD      0x40     /* LED update in progress */
#define KF4_XMIT_ERR     0x80     /* Transmit error */


/* Keyboard scan codes */

#define SCAN_ESC        0x01
                        
#define SCAN_1          0x02
#define SCAN_2          0x03
#define SCAN_3          0x04
#define SCAN_4          0x05
#define SCAN_5          0x06
#define SCAN_6          0x07
#define SCAN_7          0x08
#define SCAN_8          0x09
#define SCAN_9          0x0a
#define SCAN_0          0x0b
                        
#define SCAN_MINUS      0x0c     /* Minus sign on US keyboard only */
#define SCAN_EQUAL      0x0d     /* Equals sign on US keyboard only */
                        
#define SCAN_BACKSPACE  0x0e
#define SCAN_TAB        0x0f
                        
#define SCAN_Q          0x10
#define SCAN_W          0x11
#define SCAN_E          0x12
#define SCAN_R          0x13
#define SCAN_T          0x14
#define SCAN_Y          0x15     /* Y on US keyboard, Z on German */
#define SCAN_U          0x16
#define SCAN_I          0x17
#define SCAN_O          0x18
#define SCAN_P          0x19
                        
#define SCAN_OBRACK     0x1a     /* Open bracket on US keyboard only */
#define SCAN_CBRACK     0x1b     /* Close bracket on US keyboard only */
                        
#define SCAN_ENTER      0x1c
#define SCAN_CONTROL    0x1d
                        
#define SCAN_A          0x1e
#define SCAN_S          0x1f
#define SCAN_D          0x20
#define SCAN_F          0x21
#define SCAN_G          0x22
#define SCAN_H          0x23
#define SCAN_J          0x24
#define SCAN_K          0x25
#define SCAN_L          0x26
                        
#define SCAN_SEMICOLON  0x27     /* Semicolon on US keyboard only */
#define SCAN_SQUOTE     0x28     /* Single quote on US keyboard only */
#define SCAN_BQUOTE     0x29     /* Back quote on US keyboard only */
                        
#define SCAN_LSHIFT     0x2a     /* Left shift */
                        
#define SCAN_BACKSLASH  0x2b     /* Back slash on US keyboard only */
                        
#define SCAN_Z          0x2c     /* Z on US keyboard, Y on German */
#define SCAN_X          0x2d
#define SCAN_C          0x2e
#define SCAN_V          0x2f
#define SCAN_B          0x30
#define SCAN_N          0x31
#define SCAN_M          0x32
                        
#define SCAN_COMMA      0x33     /* Comma on most keyboards */
#define SCAN_DOT        0x34     /* Dot on most keyboards */
#define SCAN_SLASH      0x35     /* Slash on US keyboard only */
                        
#define SCAN_RSHIFT     0x36     /* Right shift */
#define SCAN_PRTSCR     0x37     /* Print screen */
#define SCAN_ALT        0x38     /* Alt key */
#define SCAN_SPACE      0x39     /* Space bar */
#define SCAN_CAPLOCK    0x3a     /* Caps lock */
                        
#define SCAN_F1         0x3b
#define SCAN_F2         0x3c
#define SCAN_F3         0x3d
#define SCAN_F4         0x3e
#define SCAN_F5         0x3f
#define SCAN_F6         0x40
#define SCAN_F7         0x41
#define SCAN_F8         0x42
#define SCAN_F9         0x43
#define SCAN_F10        0x44
                        
#define SCAN_NUMLOCK    0x45     /* Num lock */
#define SCAN_SCRLOCK    0x46     /* Scroll lock */
                        
#define SCAN_HOME       0x47
#define SCAN_CSRUP      0x48
#define SCAN_PAGEUP     0x49
#define SCAN_GRAY_MINUS 0x4a
#define SCAN_CSRLEFT    0x4b
#define SCAN_NUMPAD_5   0x4c
#define SCAN_CSRRIGHT   0x4d
#define SCAN_GRAY_PLUS  0x4e
#define SCAN_END        0x4f
#define SCAN_CSRDOWN    0x50
#define SCAN_PAGEDOWN   0x51

#define SCAN_INS        0x52
#define SCAN_DEL        0x53

#define SCAN_F11        0x57
#define SCAN_F12        0x58


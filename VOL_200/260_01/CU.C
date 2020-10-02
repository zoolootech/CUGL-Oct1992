/* @(#)cu.c 1.1  Delta Date: 19:02:52 12/7/87 */
/*
 * Written by Francois Bergeon 1987 to compile under Microsoft C 4.00+
 * with stty.lib
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include <tty.h>

#define PARAM   CS8
#define COM300  B300  | PARAM
#define COM1200 B1200 | PARAM
#define COM2400 B2400 | PARAM
#define ETX     3
#define EOT     4
#define COM1    0
#define INIT    "AT E0\r"
#define DIAL    "AT D %s\r"
#define WAIT    'W'
/* IBM PC special keys return codes */
#define HOME   'G'
#define UP     'H'
#define LEFT   'K'
#define RIGHT  'M'
#define DOWN   'P'
#define ANSI_HOME  'H'
#define ANSI_UP    'A'
#define ANSI_LEFT  'D'
#define ANSI_RIGHT 'C'
#define ANSI_DOWN  'B'

int (*oldsig)(), ctrlc();
int fd, out = -1, silent = 0, divert = 0;
jmp_buf back;

main(argc, argv)
char *argv[];
int argc;
   {
   char *telno, *dev = "COM1";
   int speed = COM1200;

   if (--argc < 1)
      usage();
   telno = *++argv;
   while (--argc)
      {
      ++argv;
      if (argv[0][0] == '-')
         switch (tolower(argv[0][1]))
            {
            case 's':
               if (!strcmp(&argv[0][2], "300"))
                  speed = COM300;
               else if (!strcmp(&argv[0][2], "1200"))
                  speed = COM1200;
               else if (!strcmp(&argv[0][2], "2400"))
                  speed = COM2400;
               else
                  usage();
               break;
            case 'l':
               dev = &argv[0][2];
               break;
            default:
               usage();
            }
      }
   if ((fd = tty_open(dev, speed)) < 0)
      {
      fprintf(stderr, "cu: can't open %s\n", dev);
      exit(1);
      }
   oldsig = signal(SIGINT, ctrlc);
   if (!setjmp(back))
      cu(telno);
   tty_close(tty_open(dev, B0));
   if (divert)
      close(out);
   signal(SIGINT, oldsig);
   }

usage()
   {
   fprintf(stderr, "Usage: cu <phone> [-s<speed> -l<device>]\n");
   exit(1);
   }

cu(number)
char *number;
   {
   char rx[80], tx;
   int l;

   dial(number);
   while (1)
      {
      if ((l = tty_read(fd, rx, 80)) > 0)
         {
         if (!silent)
            fwrite(rx, l, 1, stdout);
         if (divert)
            write(out, rx, l);
         }
      if (kbhit())
         if ((tx = getch()) == '~')
            command();
         else
            transmit(tx);
      }
   }

dial(phone)
char *phone;
   {
   char dial_it[80];
   int i;

   _comflush(fd);
   if (!tty_write(fd, INIT, sizeof(INIT) - 1))
      {
      fprintf(stderr, "cu: modem not present\n");
      longjmp(back, 2);
      }
   sleep(2); 
   hyphen(phone);
   sprintf(dial_it, DIAL, phone);
   tty_write(fd, dial_it, strlen(dial_it));
   _comflush(fd);
   }

command()
   {
   char cmd[100];

   putchar('~');
   if (gets(cmd) != (char *)NULL)
      switch (cmd[0])
         {
         case '.':
         case EOT:
            longjmp(back, 1);
            break;
         case '!':
            if (cmd[1])
               system(&cmd[1]);
            else
               spawnl(P_WAIT, getenv("COMSPEC"), getenv("COMSPEC"), NULL);
            break;
         case '<':
            send(&cmd[1]);
            break;
         case '$':
            run(&cmd[1]);
            break;
         case '~':
            tty_write(fd, cmd, strlen(cmd));
            break;
         case '>':
            dvrt(&cmd[1]);
            break;
         case '%':
            xfer(&cmd[1]);
            break;
         case '#':
            _combrk(fd);
            break;
         }
   }

xfer(str)
char *str;
   {
   char *action, *from, *to;
   char cmd[100];
   char c = EOT;

   action = strtok(str, " ");
   from = strtok(action, NULL);
   to   = strtok(from  , NULL);
   to = (to ? to : from);
   if (!strcmp(action, "take"))
      {
      strcpy(cmd, "stty -echo\n");
      tty_write(fd, cmd, strlen(cmd));
      _comflush(fd);
      sprintf(cmd, "cat %s; echo \\#%o\n", from, EOT);
      tty_write(fd, cmd, strlen(cmd));
      receive(to);
      strcpy(cmd, "stty echo\n");
      tty_write(fd, cmd, strlen(cmd));
      _comflush(fd);
      }
   else if (!strcmp(action, "put"))
      {
      sprintf(cmd, "stty -echo; cat > %s; stty echo\n", to);
      tty_write(fd, cmd, strlen(cmd));
      _comflush(fd);
      send(from);
      tty_write(fd, &c, 1);
      _comflush(fd);
      }
   }

dvrt(str)
char *str;
   {
   int oflag = O_WRONLY | O_CREAT | O_TRUNC | O_BINARY;

   if (divert)
      {
      close(out);
      divert = 0;
      silent = 0;
      }
   if (*str == '>')
      {
      oflag = O_WRONLY | O_CREAT | O_APPEND;
      ++str;
      }
   if (*str == ':')
      {
      silent = 1;
      ++str;
      }
   if ((out = open(str, oflag, S_IREAD | S_IWRITE)) < 0)
      silent = 0;
   else
      divert = 1;
   }

send(str)
char *str;
   {
   static char line[100];
   FILE *fp;
   int len;

   if ((fp = fopen(str, "r")) != NULL)
      {
      while (fgets(line, 100, fp) != NULL)
         {
         fputs(line, stdout);
         len = strlen(line);
         tty_write(fd, line, len);
         if (line[len-1] == '\n')
            tty_write(fd, "\r", 1);
         _comflush();
         }
      fclose(fp);
      _comflush();
      }
   }

receive(str)
char *str;
   {
   FILE *fp;
   char c;

   if ((fp = fopen(str, "wb")) != NULL)
      {
      do
         if (tty_read(fd, &c, 1))
            putc(c, fp);
      while (c != EOT);
      fclose(fp);
      }
   }

run(str)
char *str;
   {
   char *temp;
   int sout;

   temp = mktemp("cuXXXXXX");
   strcat(str, " > ");
   strcat(str, temp);
   system(str);
   send(temp);
   unlink(temp);
   }

hyphen(str)
char *str;
   {
   do
      if (*str == '-')
         *str = WAIT;
   while (*(str++));
   }

int ctrlc()
   {
   char c = ETX;

   signal(SIGINT, ctrlc);
   tty_write(fd, &c, 1);
   }

transmit(c)
char c;
   {
   static char *ansistr = "\033[ ";

   if (c)
      tty_write(fd, &c, 1);
   else
      {
      switch ((c = getch()))
         {
         case HOME:
            ansistr[2] = ANSI_HOME;
            break;
         case UP:
            ansistr[2] = ANSI_UP;
            break;
         case LEFT:
            ansistr[2] = ANSI_LEFT;
            break;
         case RIGHT:
            ansistr[2] = ANSI_RIGHT;
            break;
         case DOWN:
            ansistr[2] = ANSI_DOWN;
            break;
         default:
            return;
         }
      tty_write(fd, ansistr, 3);
      }
   }


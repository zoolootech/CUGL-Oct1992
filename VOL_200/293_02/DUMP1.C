/*
 * Dumps a number of bytes from a specified offset from a file.
 * The bytes are output in hex and ascii.
 *
 * Usage: dump file [-o] [-n] [-d | -h] [-w | -b]
 *        -o    : offset (default 0)
 *        -n    : number (default 256)
 *        -d -h : decimal/hexadecimal (default h)
 *        -w -b : word/byte (default b)
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

main(argc, argv)
int argc;
char *argv[];
{
  FILE *fp;
  char  fn[256];
  int   offset;
  int   number;
  int   char_cnt;
  unsigned char  in_char[16];
  char  out_char[16];
  int   i;
  int   hexmode;
  int   bytemode;

  fn[0] = '\0'; 
  offset = 0;
  number = 256;
  hexmode = bytemode = 1;
  for (argc; argc > 1; argc--) {
    if (**++argv == '-') {
      switch (tolower(*(*argv + 1))) {
        case 'o':
          offset = atoi(*argv+2);
          break;
        case 'n':
          number = atoi(*argv+2);
          break;
        case 'd':
          hexmode = 0;
          break;
        case 'h':
          hexmode = 1;
          break;
        case 'b':
          bytemode = 1;
          break;
        case 'w':
          bytemode = 0;
          break;
      }
    }
    else
    strcpy(fn, *argv);
  }

  if (fn[0] == '\0' || offset < 0 || number <= 0) {
    printf("Usage: dump file [-o] [-n] [-d | -h] [-b | -w]\n");
    exit(0);
  }

  if ((fp = fopen(fn, "rb")) == NULL) {
    printf("Error opening %s\n", fn);
    exit(1);
  }

  fseek(fp, (long) offset, SEEK_SET);

  for (char_cnt = 0; char_cnt < number; char_cnt += 16) {
    fread(in_char, 1, 16, fp);
    if (bytemode)
      for (i = 0; i < 16; i++)
        if (hexmode)
          printf("%02x ", in_char[i]);
        else
          printf("%03d ", in_char[i]);
    else
      for (i = 0; i < 16; i += 2)
        if (hexmode)
          printf("%02x%02x ", in_char[i], in_char[i+1]);
        else
          printf("%06d ", in_char[i+1] * 0x100 + in_char[i]);

    for (i = 0; i < 16; i++)
      printf("%c", (isprint(in_char[i])) ? in_char[i] : '.');
    printf("\n");
  }

}
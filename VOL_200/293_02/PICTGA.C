/*
 * Converts a 256*n B/W picture file into a 256*256 TARGA-TIPS B/W or RGB file.
 *
 * Usage: pictga infile outfile [-c]
 *        -c   : color image RGB (blue background)
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#ifdef MSDOS
typedef int WORD;
#else
typedef short int WORD;
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

/* allocate all arrays as global */
char  fni[256], fno[256];
unsigned char inline[256];
unsigned char outrgb[256][3];


main(argc, argv)
int argc;
char *argv[];
{
  FILE *fpi, *fpo;
  WORD  tgaheadi;
  unsigned char tgaheadc;
  int   line, pixel;
  int   nlines;
  int   rgb;

  fni[0] = fno[0] = '\0'; 
  rgb = 0;
  for (argc; argc > 1; argc--) {
    if (**++argv == '-') {
      switch (tolower(*(*argv + 1))) {
        case 'c':
          rgb = 1;
          break;
        default:
          printf("Usage: pictga infile outfile [-c]\n");
          exit(0);
          break;
      }
    }
    else {
      if (fni[0] == '\0')
        strcpy(fni, *argv);
      else
        strcpy(fno, *argv);
    }
  }

  if (fni[0] == '\0' || fno[0] == '\0') {
    printf("Usage: pictga infile outfile [-c]\n");
    exit(0);
  }

  if ((fpi = fopen(fni, "rb")) == NULL) {
    printf("Error opening %s\n", fni);
    exit(1);
  }
  if ((fpo = fopen(fno, "wb")) == NULL) {
    printf("Error opening %s\n", fno);
    exit(1);
  }

  /* find number of lines in input file */
  fseek(fpi, 0, SEEK_END);
  nlines = ftell(fpi) / 256;
  printf("Number of lines in input = %d\n", nlines);
  rewind(fpi);

  /* write header to tga file */
  tgaheadc = 0;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  tgaheadc = (rgb) ? 3 : 2;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  tgaheadc = 0;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  tgaheadi = 0;
  fwrite(&tgaheadi, sizeof(WORD), 1, fpo);
  fwrite(&tgaheadi, sizeof(WORD), 1, fpo);
  tgaheadi = 256;
  fwrite(&tgaheadi, sizeof(WORD), 1, fpo);
  fwrite(&tgaheadi, sizeof(WORD), 1, fpo);
  tgaheadc = (rgb) ? 24 : 8;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  tgaheadc = 0;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);

  /* clear empty lines */
  if (rgb) {
    for (pixel = 0; pixel < 256; pixel++) {
      outrgb[pixel][0] = 200;
      outrgb[pixel][1] = outrgb[pixel][2] = 0;
    }
  }
  else {
    for (pixel = 0; pixel < 256; pixel++)
      inline[pixel] = 0;
  }
  for (line = 0; line < (256-nlines)/2; line++) {
    if (rgb)
      fwrite(outrgb, sizeof(unsigned char), 3*256, fpo);
    else
      fwrite(inline, sizeof(unsigned char), 256, fpo);
  }
 
  /* read/write one line at a time */
  for (line = 0; line < nlines; line++) {
    if (fread(inline, sizeof(unsigned char), 256, fpi) != 256) {
      printf("Error reading file %s\n", fni);
      exit(1);
    }
    if (rgb) {
      for (pixel = 0; pixel < 256; pixel++) {
        outrgb[pixel][0] = (inline[pixel] == 0) ? 200 : inline[pixel];
        outrgb[pixel][1] = outrgb[pixel][2] = inline[pixel];
      }
      fwrite(outrgb, sizeof(unsigned char), 3*256, fpo);
    }
    else
      fwrite(inline, sizeof(unsigned char), 256, fpo);
  }

  /* clear empty lines */
  if (rgb)
    for (pixel = 0; pixel < 256; pixel++) {
      outrgb[pixel][0] = 200;
      outrgb[pixel][1] = outrgb[pixel][2] = 0;
    }
  else
    for (pixel = 0; pixel < 256; pixel++)
      inline[pixel] = 0;
  for (line = 0; line < (257-nlines)/2; line++)
    if (rgb)
      fwrite(outrgb, sizeof(unsigned char), 3*256, fpo);
    else
      fwrite(inline, sizeof(unsigned char), 256, fpo);

  fclose(fpi);
  fclose(fpo);

}
/*
 * Converts a 256*256 ct file into a 256*256 TARGA-TIPS B/W file.
 *
 * Usage: cttga infile outfile [-l] [-u] [-h] [-s]
 *        -l    : lower threshold (default =  0)
 *        -u    : upper threshold (default =  255)
 *        -h    : number of header blocks (512 bytes) (default = 1)
 *        -s    : output statistics
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#ifdef MSDOS
typedef int WORD;
#else
typedef unsigned short int WORD;
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#endif

/* allocate all arrays as global */
char  fni[256], fno[256];
WORD   inline[256];
unsigned char outline[256];
char  *usestr = "Usage: cttga infile outfile [-l] [-u] [-h] [-s]";


main(argc, argv)
int argc;
char *argv[];
{
  FILE *fpi, *fpo;
  int   lower;
  int   upper;
  int   range;
  WORD  tgaheadi;
  unsigned char tgaheadc;
  int   line, pixel;
  int   statistics;
  int   minidens, maxidens;
  int   minodens, maxodens;
  int   header_blocks;

  fni[0] = fno[0] = '\0';
  statistics = 0;
  lower = 0;
  upper = 255;
  header_blocks = 1;
  for (argc; argc > 1; argc--) {
    if (**++argv == '-' || **argv == '/') {
      switch (tolower(*(*argv + 1))) {
        case 'l':
          lower = atoi(*argv+2);
          break;
        case 'u':
          upper = atoi(*argv+2);
          break;
        case 'h':
          header_blocks = atoi(*argv+2);
          break;
        case 's':
          statistics = 1;
          break;
        default:
          printf("%s\n", usestr);
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
    printf("%s\n", usestr);
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

  /* skip header in ct file */
  fseek(fpi, (long) (header_blocks*512), SEEK_SET);

  range = upper - lower;
  if (range <= 0) {
    printf("lower/upper are out of range (%d)\n", range);
    exit(1);
  }
  if (range < 255)
    printf("lower/upper range gives non optimal contrast (%d)\n", range);

  /* write header to tga file */
  tgaheadc = 0;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  tgaheadc = 3;
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
  tgaheadc = 8;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);
  tgaheadc = 0;
  fwrite(&tgaheadc, sizeof(unsigned char), 1, fpo);

  /* read in the file one line at a time and convert the density values */
  minidens = minodens = 16000;
  maxidens = maxodens = -16000;
  for (line = 0; line < 256; line++) {
    fread(inline, sizeof(WORD), 256, fpi);
    for (pixel = 0; pixel < 256; pixel++) {
      minidens = (minidens < inline[pixel]) ? minidens : inline[pixel];
      maxidens = (maxidens > inline[pixel]) ? maxidens : inline[pixel];
      if (inline[pixel] < lower)
        if (inline[pixel] < -16000)
          outline[pixel] = 255;
        else
          outline[pixel] = 0;
      else if (inline[pixel] > upper)
        outline[pixel] = 255;
      else {
        outline[pixel] = 
          (float) (inline[pixel] - lower) / (float) range * 255.;
      }
      minodens = (minodens < (int) outline[pixel]) ? 
                  minodens : (int) outline[pixel];
      maxodens = (maxodens > (int) outline[pixel]) ?
                  maxodens : (int) outline[pixel];
    }
    fwrite(outline, sizeof(unsigned char), 256, fpo);
  }

  fclose(fpi);
  fclose(fpo);

  if (statistics) {
    printf("input density  - minimum = %5d, maximum = %5d\n",
            minidens, maxidens);
    printf("output density - minimum = %5d, maximum = %5d\n",
            minodens, maxodens);
  } 

}
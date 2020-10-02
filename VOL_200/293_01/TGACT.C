/*
 * Converts a TARGA-TIPS RGB file into a 256*256 ct file
 * only the upper left part of the RGB file is used
 * the RGB file should be saved from TIPS using the window option
 *
 * Usage: tgact infile outfile headerfile [-l] [-u] [-h]
 *        -l    : lower output density (default = 0)
 *        -u    : upper output density (default = 255)
 *        -h    : number of header blocks (512 bytes) (default = 1)
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#ifdef MSDOS
typedef int WORD;
#else
typedef short int WORD;
#endif

/* allocate all arrays as global */
char  fni[256], fno[256], fnh[256];
WORD   outline[256];
unsigned char inline[256][3];
unsigned char inheader[512];
char *usestr = "Usage: tgact infile outfile headerfile [-l] -[u] [-h]";

main(argc, argv)
int argc;
char *argv[];
{
  FILE *fpi, *fpo, *fph;
  int   lower;
  int   upper;
  int   range;
  int   tgaheadi;
  unsigned char tgaheadc;
  int   xsize, ysize;
  int   tgaxsize, tgaysize;
  int   line, pixel;
  int   inlines, inpixels;
  int   i;
  int   header_blocks;

  fni[0] = fno[0] = fnh[0] = '\0'; 
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
        default:
          printf("%s\n", usestr);
          exit(0);
          break;
      }
    }
    else {
      if (fni[0] == '\0')
        strcpy(fni, *argv);
      else if (fno[0] == '\0')
        strcpy(fno, *argv);
      else
        strcpy(fnh, *argv);
    }
  }

  if (fni[0] == '\0' || fno[0] == '\0' || fnh[0] == '\0') {
    printf("%s\n", usestr);
    exit(0);
  }

  if ((fpi = fopen(fni, "rb")) == NULL) {
    printf("Error opening %s\n", fni);
    exit(1);
  }
  if ((fph = fopen(fnh, "rb")) == NULL) {
    printf("Error opening %s\n", fnh);
    exit(1);
  }
  if ((fpo = fopen(fno, "wb")) == NULL) {
    printf("Error opening %s\n", fno);
    exit(1);
  }

  /* copy header from ct file */
  for (i = 0; i < header_blocks; i++) {
    fread(inheader, sizeof(unsigned char), 512, fph);
    fwrite(inheader, sizeof(unsigned char), 512, fpo);
  }

  range = upper - lower;
  if (range <= 0) {
    printf("lower/upper are out of range (%d)\n", range);
    exit(1);
  }
  if (range < 255)
    printf("lower/upper range gives non optimal contrast (%d)\n", range);

  /* test header of tga file */
  fread(&tgaheadc, sizeof(unsigned char), 1, fpi);
  if (tgaheadc != 0)
    printf("Warning Field 1 is not 0 (%d)\n", tgaheadc);
  fread(&tgaheadc, sizeof(unsigned char), 1, fpi);
  if (tgaheadc != 0)
    printf("Warning Field 2 is not 0 (%d)\n", tgaheadc);
  fread(&tgaheadc, sizeof(unsigned char), 1, fpi);
  if (tgaheadc != 2)
    printf("Warning Field 3 is not 2 (%d)\n", tgaheadc);
  for (i = 1; i <= 5; i++) {
    fread(&tgaheadc, sizeof(unsigned char), 1, fpi);
    if (tgaheadc != 0)
      printf("Warning Field 4.%d is not 0 (%d)\n", i, tgaheadc);
  }
  fread(&tgaheadi, sizeof(int), 1, fpi);
  fread(&tgaheadi, sizeof(int), 1, fpi);

  fread(&tgaxsize, sizeof(int), 1, fpi);
  fread(&tgaysize, sizeof(int), 1, fpi);

  fread(&tgaheadc, sizeof(unsigned char), 1, fpi);
  if (tgaheadc != 24)
    printf("Warning Field 5.5 is not 24 (%d)\n", tgaheadc);
  fread(&tgaheadc, sizeof(unsigned char), 1, fpi);
  if (tgaheadc != 0)
    printf("Warning Field 5.6 is not 0 (%d)\n", tgaheadc);

  /* skip lines from input or write empty lines to output */
  inlines = 256;
  if (tgaysize > 256)
    for (line = 256; line < tgaysize; line++)
      for (pixel = 0; pixel < tgaxsize; pixel++)
        fread(inline, sizeof(unsigned char), 3, fpi);
  else if (tgaysize < 256) {
    for (pixel = 0; pixel < 256; pixel++)
      outline[pixel] = lower;
    for (line = tgaysize; line < 256; line++)
      fwrite(outline, sizeof(int), 256, fpo);
    inlines = tgaysize;
  }
  inpixels = 256;
  if (tgaxsize < 256) inpixels = tgaxsize;

  /* read in the file one line at a time, convert the density values
     and write to file 256 words */
  for (line = 0; line < inlines; line++) {
    for (pixel = 0; pixel < inpixels; pixel++) {
      fread(inline[pixel], sizeof(unsigned char), 3, fpi);
      outline[pixel] = (float) inline[pixel][0] * (float) range / 255.;
      outline[pixel] += lower;
    }

    if (inpixels < 256)
      for ( ; pixel < 256; pixel++)
        outline[pixel] = lower;
    else if (tgaxsize > 256)
      for (pixel = 256; pixel < tgaxsize; pixel++)
        fread(inline, sizeof(unsigned char), 3, fpi);

    fwrite(outline, sizeof(int), 256, fpo);
  }

  fclose(fpi);
  fclose(fpo);
  fclose(fph);

}
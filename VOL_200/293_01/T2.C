/*
 *  Using FSEEK to position in a newly created file.
 *  Writing forwards and backwards.
 *
 */

#include <stdio.h>

main()
{
  FILE *fp;
  char c[32];
  int i, j;
  long int l;

  /* forwards */
  if ((fp = fopen("t2f.tmp", "wb")) == NULL) {
    printf("Error creating file\n");
    exit(1);
  }

  for (i = 0; i < 32; i++) {
    l = (long) i * 32;
    if (fseek(fp, l, SEEK_SET)) {
      printf("Error seeking\n");
      exit(1);
    }
    for (j = 0; j < 32; j++)
      c[j] = i;
    if (fwrite(c, sizeof(char), 32, fp) != 32*sizeof(char)) {
      printf("Error writing\n");
	  exit(1);
    }
  }

  /* backwards */
  if ((fp = freopen("t2b.tmp", "wb", fp)) == NULL) {
    printf("Error creating file\n");
    exit(1);
  }

  for (i = 31; i >= 0; i--) {
    l = (long) i * 32;
    if (fseek(fp, l, SEEK_SET)) {
      printf("Error seeking\n");
      exit(1);
    }
    for (j = 0; j < 32; j++)
      c[j] = i;
    if (fwrite(c, sizeof(char), 32, fp) != 32*sizeof(char)) {
      printf("Error writing\n");
	  exit(1);
    }
  }
}

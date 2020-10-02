/*
** stdio.h -- header for STDIO & run-time services
*/
/* temps */
#define getc fgetc
#define putc fputc
#define _errno errno
 
#define FILE int
#define EOF (-1)
#define YES 1
#define NO 0
#define NULL 0
extern int
  *stdin,
  *stdout,
  *stderr,
  errno;
extern int
  /* std I/O routines */
  abort(),
  exit(),
  _exit(),
  fopen(),
  fclose(),
  fflush(),
  fread(),
  fwrite(),
  fseek2(),
  fgetc(),
  ungetc(),
  getchar(),
  fgets(),
  gets(),
  fputc(),
  putchar(),
  fputs(),
  puts(),
  printf(),
  fprintf(),
  sprintf(),
  malloc(),
  calloc(),
  free();

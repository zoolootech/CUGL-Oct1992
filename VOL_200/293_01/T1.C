/*
 * This programm reads in a number from the command line and uses that
 * number to chose video mode. It gives the number of pixels in x and y
 * before waiting for the users input, then it switches to default mode.
 */



#include <stdio.h>
#include <stdlib.h>
#include <graph.h>

main(argc, argv)
int argc;
char *argv[];
{
  int i;
  char line[256];
  struct videoconfig config;

  if (argc != 2) exit();
  switch (**++argv) {
    case '0':
    default:
      _setvideomode(_DEFAULTMODE);
      break;
    case '1':
      _setvideomode(_TEXTBW40);
      break;
    case '2':
      _setvideomode(_TEXTC40);
      break;
    case '3':
      _setvideomode(_TEXTBW80);
      break;
    case '4':
      _setvideomode(_TEXTC80);
      break;
    case '5':
      _setvideomode(_MRES4COLOR);
      break;
    case '6':
      _setvideomode(_MRESNOCOLOR);
      break;
    case '7':
      _setvideomode(_HRESBW);
      break;
  }
  _getvideoconfig(&config);
  printf("x = %d, y = %d\n", config.numxpixels, config.numypixels);
  gets(line);
  _setvideomode(_DEFAULTMODE);
}

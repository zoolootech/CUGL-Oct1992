/* bios call numbers, for standard bios library function, v1.44 */

#define WARMSTART 1
#define CONST 2
#define CONIN 3
#define CONOUT 4
#define LIST 5
#define HOME 8
#define SELDSK 9
#define SETTRK 10
#define SETSEC 11
#define SETDMA 12
#define READ 13
#define WRITE 14


struct dskadr {char sector; char track;}
  ;

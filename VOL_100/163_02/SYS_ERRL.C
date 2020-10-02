/*
** list of error messages corresponding to return codes in errno.h
*/
#include <errno.h>
int sys_nerr = 35;
#asm
       PUBLIC QSYS_ERRL
QSYS_ERRL DW OFFSET EM00
          DW OFFSET EM01
          DW OFFSET EM02
          DW OFFSET EM03
          DW OFFSET EM04
          DW OFFSET EM05
          DW OFFSET EM06
          DW OFFSET EM07
          DW OFFSET EM08
          DW OFFSET EM09
          DW OFFSET EM10
          DW OFFSET EM11
          DW OFFSET EM12
          DW OFFSET EM13
          DW OFFSET EM14
          DW OFFSET EM15
          DW OFFSET EM16
          DW OFFSET EM17
          DW OFFSET EM18
          DW OFFSET EM19
          DW OFFSET EM20
          DW OFFSET EM21
          DW OFFSET EM22
          DW OFFSET EM23
          DW OFFSET EM24
          DW OFFSET EM25
          DW OFFSET EM26
          DW OFFSET EM27
          DW OFFSET EM28
          DW OFFSET EM29
          DW OFFSET EM30
          DW OFFSET EM31
          DW OFFSET EM32
          DW OFFSET EM33
          DW OFFSET EM34
EM00      DB 'No error',0
EM01      DB 'Not owner',0
EM02      DB 'No such file or directory',0
EM03      DB 'No such process',0
EM04      DB 'Interrupted system call',0
EM05      DB 'I/O error',0
EM06      DB 'No such device or address',0
EM07      DB 'Argument list too long',0
EM08      DB 'Exec format error',0
EM09      DB 'Bad file number or mode',0
EM10      DB 'No children',0
EM11      DB 'Cannot fork',0
EM12      DB 'Out of storage',0
EM13      DB 'Permission denied',0
EM14      DB 'Bad address',0
EM15      DB 'Block device required',0
EM16      DB 'Device busy',0
EM17      DB 'File already exists',0
EM18      DB 'Cross-device link',0
EM19      DB 'Invalid device access',0
EM20      DB 'Not a directory',0
EM21      DB 'Is a directory',0
EM22      DB 'Invalid argument',0
EM23      DB 'Too many open files in system',0
EM24      DB 'Too many open files in process',0
EM25      DB 'Not a terminal device',0
EM26      DB 'Program file busy',0
EM27      DB 'File too big',0
EM28      DB 'Out of space on device',0
EM29      DB 'Non-seekable device',0
EM30      DB 'Write to read-only file',0
EM31      DB 'Too many links',0
EM32      DB 'Pipe unusable',0
EM33      DB 'Math domain error',0
EM34      DB 'Math range error',0
#endasm

#include "stdio.h"


int array[51];



swap(x,y)
int x,y;
{
int temp;


if(array[x] > array[y])
	{
	temp=array[y];

	array[y]=array[x];

	array[x]=temp;
	}
}







main()
{
int j;
double drand();


puts("Experimental BOSE sort test\nGenerating numbers");

for(j=1 ; j < 51 ; ++j)
	{
	array[j]=(double)(drand()*500);

	printf("%d\t",array[j]);
	}

puts("Sorting");

swap(2,3);
swap(1,3);
swap(1,2);
swap(5,6);
swap(4,6);
swap(4,5);
swap(1,4);
swap(2,5);
swap(3,6);
swap(3,5);
swap(2,4);
swap(3,4);
swap(8,9);
swap(7,9);
swap(7,8);
swap(11,12);
swap(10,12);
swap(10,11);
swap(7,10);
swap(8,11);
swap(9,12);
swap(9,11);
swap(8,10);
swap(9,10);
swap(1,7);
swap(2,8);
swap(3,9);
swap(3,8);
swap(2,7);
swap(3,7);
swap(4,10);
swap(5,11);
swap(6,12);
swap(6,11);
swap(5,10);
swap(6,10);
swap(4,7);
swap(5,8);
swap(6,9);
swap(6,8);
swap(5,7);
swap(6,7);
swap(14,15);
swap(13,15);
swap(13,14);
swap(17,18);
swap(16,18);
swap(16,17);
swap(13,16);
swap(14,17);
swap(15,18);
swap(15,17);
swap(14,16);
swap(15,16);
swap(20,21);
swap(19,21);
swap(19,20);
swap(22,23);
swap(24,25);
swap(22,24);
swap(23,25);
swap(23,24);
swap(19,23);
swap(19,22);
swap(20,24);
swap(21,25);
swap(21,24);
swap(20,22);
swap(21,23);
swap(21,22);
swap(13,20);
swap(13,19);
swap(14,21);
swap(15,22);
swap(15,21);
swap(14,19);
swap(15,20);
swap(15,19);
swap(16,23);
swap(17,24);
swap(18,25);
swap(18,24);
swap(17,23);
swap(18,23);
swap(16,20);
swap(16,19);
swap(17,21);
swap(18,22);
swap(18,21);
swap(17,19);
swap(18,20);
swap(18,19);
swap(1,14);
swap(1,13);
swap(2,15);
swap(3,16);
swap(3,15);
swap(2,13);
swap(3,14);
swap(3,13);
swap(4,17);
swap(5,18);
swap(6,19);
swap(6,18);
swap(5,17);
swap(6,17);
swap(4,14);
swap(4,13);
swap(5,15);
swap(6,16);
swap(6,15);
swap(5,13);
swap(6,14);
swap(6,13);
swap(7,20);
swap(8,21);
swap(9,22);
swap(9,21);
swap(8,20);
swap(9,20);
swap(10,23);
swap(11,24);
swap(12,25);
swap(12,24);
swap(11,23);
swap(12,23);
swap(10,20);
swap(11,21);
swap(12,22);
swap(12,21);
swap(11,20);
swap(12,20);
swap(7,14);
swap(7,13);
swap(8,15);
swap(9,16);
swap(9,15);
swap(8,13);
swap(9,14);
swap(9,13);
swap(10,17);
swap(11,18);
swap(12,19);
swap(12,18);
swap(11,17);
swap(12,17);
swap(10,14);
swap(10,13);
swap(11,15);
swap(12,16);
swap(12,15);
swap(11,13);
swap(12,14);
swap(12,13);
swap(27,28);
swap(26,28);
swap(26,27);
swap(30,31);
swap(29,31);
swap(29,30);
swap(26,29);
swap(27,30);
swap(28,31);
swap(28,30);
swap(27,29);
swap(28,29);
swap(33,34);
swap(32,34);
swap(32,33);
swap(36,37);
swap(35,37);
swap(35,36);
swap(32,35);
swap(33,36);
swap(34,37);
swap(34,36);
swap(33,35);
swap(34,35);
swap(26,32);
swap(27,33);
swap(28,34);
swap(28,33);
swap(27,32);
swap(28,32);
swap(29,35);
swap(30,36);
swap(31,37);
swap(31,36);
swap(30,35);
swap(31,35);
swap(29,32);
swap(30,33);
swap(31,34);
swap(31,33);
swap(30,32);
swap(31,32);
swap(39,40);
swap(38,40);
swap(38,39);
swap(42,43);
swap(41,43);
swap(41,42);
swap(38,41);
swap(39,42);
swap(40,43);
swap(40,42);
swap(39,41);
swap(40,41);
swap(45,46);
swap(44,46);
swap(44,45);
swap(47,48);
swap(49,50);
swap(47,49);
swap(48,50);
swap(48,49);
swap(44,48);
swap(44,47);
swap(45,49);
swap(46,50);
swap(46,49);
swap(45,47);
swap(46,48);
swap(46,47);
swap(38,45);
swap(38,44);
swap(39,46);
swap(40,47);
swap(40,46);
swap(39,44);
swap(40,45);
swap(40,44);
swap(41,48);
swap(42,49);
swap(43,50);
swap(43,49);
swap(42,48);
swap(43,48);
swap(41,45);
swap(41,44);
swap(42,46);
swap(43,47);
swap(43,46);
swap(42,44);
swap(43,45);
swap(43,44);
swap(26,39);
swap(26,38);
swap(27,40);
swap(28,41);
swap(28,40);
swap(27,38);
swap(28,39);
swap(28,38);
swap(29,42);
swap(30,43);
swap(31,44);
swap(31,43);
swap(30,42);
swap(31,42);
swap(29,39);
swap(29,38);
swap(30,40);
swap(31,41);
swap(31,40);
swap(30,38);
swap(31,39);
swap(31,38);
swap(32,45);
swap(33,46);
swap(34,47);
swap(34,46);
swap(33,45);
swap(34,45);
swap(35,48);
swap(36,49);
swap(37,50);
swap(37,49);
swap(36,48);
swap(37,48);
swap(35,45);
swap(36,46);
swap(37,47);
swap(37,46);
swap(36,45);
swap(37,45);
swap(32,39);
swap(32,38);
swap(33,40);
swap(34,41);
swap(34,40);
swap(33,38);
swap(34,39);
swap(34,38);
swap(35,42);
swap(36,43);
swap(37,44);
swap(37,43);
swap(36,42);
swap(37,42);
swap(35,39);
swap(35,38);
swap(36,40);
swap(37,41);
swap(37,40);
swap(36,38);
swap(37,39);
swap(37,38);
swap(1,26);
swap(2,27);
swap(3,28);
swap(3,27);
swap(2,26);
swap(3,26);
swap(4,29);
swap(5,30);
swap(6,31);
swap(6,30);
swap(5,29);
swap(6,29);
swap(4,26);
swap(5,27);
swap(6,28);
swap(6,27);
swap(5,26);
swap(6,26);
swap(7,32);
swap(8,33);
swap(9,34);
swap(9,33);
swap(8,32);
swap(9,32);
swap(10,35);
swap(11,36);
swap(12,37);
swap(12,36);
swap(11,35);
swap(12,35);
swap(10,32);
swap(11,33);
swap(12,34);
swap(12,33);
swap(11,32);
swap(12,32);
swap(7,26);
swap(8,27);
swap(9,28);
swap(9,27);
swap(8,26);
swap(9,26);
swap(10,29);
swap(11,30);
swap(12,31);
swap(12,30);
swap(11,29);
swap(12,29);
swap(10,26);
swap(11,27);
swap(12,28);
swap(12,27);
swap(11,26);
swap(12,26);
swap(13,38);
swap(14,39);
swap(15,40);
swap(15,39);
swap(14,38);
swap(15,38);
swap(16,41);
swap(17,42);
swap(18,43);
swap(18,42);
swap(17,41);
swap(18,41);
swap(16,38);
swap(17,39);
swap(18,40);
swap(18,39);
swap(17,38);
swap(18,38);
swap(19,44);
swap(20,45);
swap(21,46);
swap(21,45);
swap(20,44);
swap(21,44);
swap(22,47);
swap(23,48);
swap(23,47);
swap(24,49);
swap(25,50);
swap(25,49);
swap(24,47);
swap(25,48);
swap(25,47);
swap(22,44);
swap(23,45);
swap(23,44);
swap(24,46);
swap(25,46);
swap(24,44);
swap(25,45);
swap(25,44);
swap(19,38);
swap(20,39);
swap(21,40);
swap(21,39);
swap(20,38);
swap(21,38);
swap(22,41);
swap(23,42);
swap(23,41);
swap(24,43);
swap(25,43);
swap(24,41);
swap(25,42);
swap(25,41);
swap(22,38);
swap(23,39);
swap(23,38);
swap(24,40);
swap(25,40);
swap(24,38);
swap(25,39);
swap(25,38);
swap(13,26);
swap(14,27);
swap(15,28);
swap(15,27);
swap(14,26);
swap(15,26);
swap(16,29);
swap(17,30);
swap(18,31);
swap(18,30);
swap(17,29);
swap(18,29);
swap(16,26);
swap(17,27);
swap(18,28);
swap(18,27);
swap(17,26);
swap(18,26);
swap(19,32);
swap(20,33);
swap(21,34);
swap(21,33);
swap(20,32);
swap(21,32);
swap(22,35);
swap(23,36);
swap(23,35);
swap(24,37);
swap(25,37);
swap(24,35);
swap(25,36);
swap(25,35);
swap(22,32);
swap(23,33);
swap(23,32);
swap(24,34);
swap(25,34);
swap(24,32);
swap(25,33);
swap(25,32);
swap(19,26);
swap(20,27);
swap(21,28);
swap(21,27);
swap(20,26);
swap(21,26);
swap(22,29);
swap(23,30);
swap(23,29);
swap(24,31);
swap(25,31);
swap(24,29);
swap(25,30);
swap(25,29);
swap(22,26);
swap(23,27);
swap(23,26);
swap(24,28);
swap(25,28);
swap(24,26);
swap(25,27);
swap(25,26);

puts("Done");

for(j=1 ; j < 51 ; ++j)
	printf("%d\t",array[j]);
}7);
swap(9,28);
swap(9,27);
swap(8,26);
swap(9,26);
swap(10,29);
swap(11,30);
swap(12,31);
swap(12,30);
swap(11,29);

/*
	TITLE:		VirtualList test driver;
	DESCRIPTION:	"Test stub for C++ Virtual linked-list.
			( Class VirtualList )";

	VERSION:        1.0;
	DATE:           9/21/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:	test driver virtual list;
	FILENAME:	Vlist.cpp;
	SEE-ALSO:	VirtList.cpp, VirtList.hpp;

	AUTHOR:         Michael Kelly
			254 Gold St. Boston, Ma. 02127
			Copyright 1990;

	COPYRIGHT:	This code may not be commercially distributed
			without prior arrangement with the author.  It
			may be used by programmers, without royalty, for
			their personal programs and for "one of a kind"
			or "custom" applications, provided that said
			programmers assume all liability concerning
			same.
*/

#include "virtlist.hpp"

#define BLOCK_SIZE 6200
char buffer[BLOCK_SIZE];

struct phrases  {
    char num_str[16];
    char junk[6000];
};


int main(void)
{
    int i;
    VirtualList v_list;
    VirtualList *virtual_list = new VirtualList;

    char crap[24];
    phrases *a_phrase = new phrases;
    if(! a_phrase)
	err_exit("Cannot allocate a_phrase");

  if(! virtual_list->get_entries())  {

    for(i = 49;i > -1;i--)  {
	sprintf(a_phrase->num_str, "%03d", i);

	if(! virtual_list->add_item(a_phrase, sizeof(phrases)))  {
		printf("\ndied on loop %d\n", 49 - i);
		err_exit("Couldn't add a phrase");
	}

    }
  }

    for(i = 0;i < 50;i++)  {
	sprintf(crap, "%02d", i);
	strcat(crap, " crap");
	if(! v_list.add_item(crap, 24))
	    err_exit("Couldn't add to Crap");
    }

    printf("\n\n\tVirtual List Sorted ...\n\n");
    if(! virtual_list->sort()  ||  virtual_list->error() == NO_MEM)
	printf("\n\tSort Failed!\n");

    do	{

	if(! virtual_list->get_item(buffer))
	    break;
	printf("%s\n", buffer);
    }
    while(virtual_list->next());


    i = virtual_list->get_entries();
    printf("\n\tList of %d items\n\n", i);

    v_list.first();

    while(v_list.remove_item(crap))
	printf("%s\n", crap);

    delete a_phrase;
    delete virtual_list;
    return(EXIT_SUCCESS);
}

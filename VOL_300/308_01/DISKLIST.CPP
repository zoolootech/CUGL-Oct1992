
/*
	TITLE:		Class DiskList;
	DESCRIPTION:	"C++ doubly-linked list with run-time
			data storage on disk";

	VERSION:        1.01;
	DATE:           9/22/90;
	COMPILERS:      Borland Turbo C++ V.1.0;
	KEYWORDS:	linked list disk;
	FILENAME:	DiskList.cpp;
	SEE-ALSO:	DiskList.hpp;

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


#include "disklist.hpp"



int dl_compare(void *c1, void *c2)
{
    return strcmp((const char *)c1, (const char *)c2);
}

int DiskList::compare(void *item1, void *item2)
{
    int 	dif;
    void 	*buf1, *buf2;
    DiskLink 	*d1 = (DiskLink *) item1,
		*d2 = (DiskLink *) item2;

    buf1 = new char[d1->itemsize];
    if(! buf1)  {
	lerror = NO_MEM;
	return 0;
    }
    buf2 = new char[d2->itemsize];
    if(! buf2)  {
	delete buf1;
	lerror = NO_MEM;
	return 0;
    }

    if((fseek(fp, d1->item_offset, SEEK_SET))  ||
       (fread(buf1, d1->itemsize, 1, fp) != 1)	  ||
       (fseek(fp, d2->item_offset, SEEK_SET))  ||
       (fread(buf2, d2->itemsize, 1, fp) != 1))
    {
	delete buf2;
	delete buf1;
	err_exit("Error reading temporary file");
    }

    dif = dl_compare(buf1, buf2);
    delete buf2;
    delete buf1;
    return dif;
}

DiskList::DiskList(void)
{
    fp = tmpfile();
    if(! fp)
	err_exit("Cannot create temporary file");
}

DiskList::~DiskList(void)
{
    if(fp)
	fclose(fp);
}

Boolean DiskList::add_item(void *item, size_t itemsize, Place place)
{
    DiskLink tmplink;

    if(fseek(fp, 0L, SEEK_END))
	return False;
    tmplink.dl_id = DL_ID;
    tmplink.item_offset = ftell(fp);
    tmplink.itemsize = itemsize;
    if(! DoubleList::add_item(&tmplink, sizeof(DiskLink), place))
	return False;
    if(fwrite(item, itemsize, 1, fp) != 1)  {
	DoubleList::delete_item();
	return False;
    }
    fflush(fp);
    return True;
}

Boolean DiskList::get_item(void *itembuf)
{
    DiskLink this_link;

    if(! DoubleList::get_item(&this_link))
	return False;
    if(fseek(fp, this_link.item_offset, SEEK_SET))
	return False;
    if(fread(itembuf, this_link.itemsize, 1, fp) != 1)
	return False;

    return True;
}

size_t DiskList::get_size(void)
{
    DiskLink this_link;

    if(! DoubleList::get_item(&this_link))
	return 0;
    return this_link.itemsize;
}

int DiskList::compare_item(void *item1)
{
    void *item2;
    DiskLink this_link;
    int dif;

    if(! DoubleList::get_item(&this_link))
	return False;
    if(! (item2 = new char[this_link.itemsize]))  {
	lerror = NO_MEM;
	return False;
    }
    if(! get_item(item2))  {
	delete item2;
	return False;
    }
    dif = dl_compare(item1, item2);
    delete item2;
    return dif;
}

Boolean DiskList::remove_item(void *itembuf)
{
    DiskLink *tmplink;
    Boolean on_disk = False;

    if(! entries)
	return False;

    if(Current->entry->itemsize == sizeof(DiskLink))  {
	tmplink = (DiskLink *)Current->entry->item;
	if(tmplink->dl_id == DL_ID)
	    on_disk = True;
    }

    if(on_disk)  {
	DiskList::get_item(itembuf);
	return DoubleList::delete_item();
    }

    return DoubleList::remove_item(itembuf);
}




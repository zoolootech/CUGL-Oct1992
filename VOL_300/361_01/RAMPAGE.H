
/* RamPage.h ---> Transparently Manage a Page of RAM & a File.
 *
 * J.Ekwall                                             11 January 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 18 September 91/EK
 */

#ifndef RAMPAGE

#define RAMPAGE

/* Declare Macros */
#define Close()			(Rampage("Close", NULL, 1))
#define CloseNoSave()		(Rampage("Close", NULL, 2))
#define DeleteLine(x)		(Rampage("Delete", NULL, x))
#define Fetch(x,y)		(Rampage("Fetch", x, y))
#define InsertLine(x,y)		(Rampage("Insert", x, y))
#define Load(x)			(Rampage("Load", x, 1))
#define LoadNoSave(x)		(Rampage("Load", x, 2))
#define Push(x,y)		(Rampage("Push", x, y))
#define Save()			(Rampage("Save", NULL, 1))
#define SaveAs(x)		(Rampage("Save", x, 2))

/* Declare the "Engine" Prototype */
int  RamPage(char *Command, char *Text, int WhereOrFlag);

#endif


#include <dos.h>
#include <dir.h>
#include <string.h>



long calc_tots(char *curr_path)
/* This function will accept a valid path name I.E. c:\dos and will return
   the total number of bytes occupied by all files in the specified directory.
*/
{
long path_tot = 0;
int rc;
char *next_search, *next_path;
struct ffblk *new_dta;
int x;
	next_search=(char *)malloc(80);
	next_path=(char *)malloc(80);
	new_dta=(struct ffblk *)malloc(sizeof(struct ffblk));
	x=strlen(curr_path) - 1;
	if(*(curr_path + x) == '\\')
		*(curr_path + x) = 0x00;
	strcpy(next_search,curr_path);
	strcat(next_search,"\\*.*");
	rc=findfirst(next_search,new_dta,0xff);
	if(rc) goto fin_up;
	if(new_dta->ff_attrib & FA_LABEL) goto do_next;
	if(new_dta->ff_name[0] == '.') goto do_next;
	if(new_dta->ff_attrib & FA_DIREC) {
		strcpy(next_path,curr_path);
		strcat(next_path,"\\");
		strcat(next_path,new_dta->ff_name);
		path_tot += calc_tots(next_path);
	}
	path_tot = path_tot + new_dta->ff_fsize;
do_next:
	rc=findnext(new_dta);	
	if(rc) goto fin_up;
	if(new_dta->ff_attrib & FA_LABEL) goto do_next;
	if(new_dta->ff_name[0] == '.') goto do_next;
	if(new_dta->ff_attrib & FA_DIREC) {
		strcpy(next_path,curr_path);
		strcat(next_path,"\\");
		strcat(next_path,new_dta->ff_name);
		path_tot += calc_tots(next_path);
	}
	path_tot = path_tot + new_dta->ff_fsize;
	goto do_next;
fin_up:
	free(next_search);
	free(next_path);
	free(new_dta);	
	return(path_tot);
}

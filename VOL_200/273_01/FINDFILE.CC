#include <dir.h>
#include <dos.h>
#include <stdlib.h>


find_file(char *dir_str, char *file_spec, char *dir_hit, char *file_hit,char *srch_type)
/* find_file will search a disk looking for a specified file. Parms are:
	char *dir_str = starting disk and directory to begin search.
	char *file_spec = file spec to search for.
	char *srch_type = "e" to search for executables i.e. bat, com, or exe.
					= "n" to search for anything.
					if search type is set to "e" and a file is found
					*srch_type will be set to b,c, or e indicating the
					type of file found. b=batch, c=com, e=exe.
	char *dir_hit   = if file found the directory the file was found in.
	char *file_hit  = if file found the filename.ext of the file.
	RETURN CODE = 1 if file found.
				= 0 if not found.
*/
{
int rc, rc2, rc3, f_rc;
struct ffblk *curr_dta;
char *next_dir;
char *curr_srch;
	curr_dta=(struct ffblk *)malloc(sizeof(struct ffblk));
	next_dir=(char *)malloc(80);
	curr_srch=(char *)malloc(80);
	if(curr_dta == NULL || next_dir == NULL || curr_srch == NULL) return(0);
	strcpy(curr_srch,dir_str);
	strcat(curr_srch,"*.*");
	rc=findfirst(curr_srch,curr_dta,FA_DIREC);
	while(rc == 0) {
		if(curr_dta->ff_attrib & FA_DIREC && curr_dta->ff_name[0] != '.') {
			strcpy(next_dir,dir_str);
			strcat(next_dir,curr_dta->ff_name);
			strcat(next_dir,"\\");
			rc3=find_file(next_dir,file_spec,dir_hit,file_hit,srch_type);
			if(rc3) {
				rc2=rc3;
				goto end_it;
			}
		}
		rc=findnext(curr_dta);
	}

	rc2=get_files(dir_str,file_spec,curr_dta,dir_hit,file_hit,srch_type);
end_it:
	free(curr_dta); free(next_dir); free(curr_srch);
	return(rc2);
}

get_files(char *dir_str, char *file_spec, struct ffblk *curr_dta,
		 char *dir_hit, char *file_hit, char *srch_type)
{
int reg_file = FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH;
int rc;
char search_str[80];
char *exe_types[] = {"BAT","COM","EXE"," "};
int x;
	strupr(srch_type);
	if(*srch_type == 'E') {
		x=0;
		while(*exe_types[x] != ' ') {
			strcpy(search_str,dir_str);
			strcat(search_str,file_spec);
			strcat(search_str,".");
			strcat(search_str,exe_types[x]);
		    rc=findfirst(search_str,curr_dta,reg_file);
			if(rc == 0) {
				strcpy(dir_hit,dir_str);
				strcpy(file_hit,curr_dta->ff_name);
				*srch_type = *exe_types[x];
				return(1);
			}
			x++;
        }
		return(0);
	}

	strcpy(search_str,dir_str);
	strcat(search_str,file_spec);
    rc=findfirst(search_str,curr_dta,reg_file);
	if(rc == 0) {
		strcpy(dir_hit,dir_str);
		strcpy(file_hit,curr_dta->ff_name);
		return(1);
	}
	return(0);
}

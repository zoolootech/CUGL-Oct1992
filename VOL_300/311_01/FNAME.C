#include <stdio.h>

/*
 *      fname_dflts - Merge defaults into a file spec
 */

char *fname_dflts(fname, dflt)
 char *fname, *dflt;
{
        static char new_name[256];
        char node1[10], dev1[50], dir1[100], file1[50], ext1[50], ver1[10];
        char node2[10], dev2[50], dir2[100], file2[50], ext2[50], ver2[10];

        fparse(fname, node1, dev1, dir1, file1, ext1, ver1);
        fparse(dflt , node2, dev2, dir2, file2, ext2, ver2);


        if (*node1) strcpy(new_name, node1); else strcpy(new_name, node2);
        if (*dev1)  strcat(new_name, dev1);  else strcat(new_name, dev2);
        if (*dir1)  strcat(new_name, dir1);  else strcat(new_name, dir2);
        if (*file1) strcat(new_name, file1); else strcat(new_name, file2);

        if (*ext1)
        {       strcat(new_name,".");
                strcat(new_name, ext1);
        }
        else
        if (*ext2)
        {       strcat(new_name,".");
                strcat(new_name, ext2);
        }

        if (*ver1)
        {       strcat(new_name,";");
                strcat(new_name, ver1);
        }
        else
        if (*ver2)
        {       strcat(new_name,";");
                strcat(new_name, ver2);
        }

        return(new_name);
}

/*
 *      fparse - parse a file spec
 */

fparse(fstr, o_node, o_dev, o_dir, o_file, o_ext, o_ver)
 char *fstr, *o_node, *o_dev, *o_dir, *o_file, *o_ext, *o_ver;
{
        char *node, *dev, *dir, *file, *ext, *ver;
        char *s, *d, hold[256], *file_dflt();

        d = node = dev = dir = file = hold;
        ext = ver = NULL;
        s = fstr;

        while (*s)
        {       switch (*d++ = *s++)
                {       case ':':   if (*s == ':')
                                    {       *d++ = *s++;
                                            dev = dir = file = d;
                                            break;
                                    }

                                    dir = file = d;
                                    break;

                        case ']':   ext  = NULL;
                                    file = d;
                                    break;

                        case '/':
                        case '\\':  file = d;
                                    break;

                        case '.':   ext  = d;
                                    break;

                        case ';':   ver = --d;
                                    break;

                        default:    break;
                }
        }         
	*d = 0;

        if (o_ver)
                if (ver) strcpy(o_ver,ver); else strcpy(o_ver,"");

	if (ver) *ver = 0;

        if (o_ext)
                if (ext) strcpy(o_ext,ext); else strcpy(o_ext,"");

	if (ext) *(ext-1) = 0;

        if (o_file)
                if (file) strcpy(o_file,file); else strcpy(o_file,"");
                   
	if (file) *file = 0;

        if (o_dir)
                if (dir) strcpy(o_dir,dir); else strcpy(o_dir,"");
                  
	if (dir) *dir = 0;

        if (o_dev)
                if (dev) strcpy(o_dev,dev); else strcpy(o_dev,"");
                  
	if (dev) *dev = 0;

        if (o_node)
                if (node) strcpy(o_node,node); else strcpy(o_node,"");
}

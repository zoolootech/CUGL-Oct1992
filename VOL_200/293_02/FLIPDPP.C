/*********************** flipdpp.c ****************************

Flip an image

Daniel Geist
Michael Vannier

Mallinckrodt Institute of Radiology
Washington University School of Medicine
510 S. Kingshighway Blvd.
St. Louis, Mo. 63110

*************************************************************/

#include <stdio.h>
#include <ctype.h>

unsigned char inbuf[256],outbuf[256],fnamein[30],fnameout[30];

main(argc,argv)

int argc;
char *argv[];

{
    int i,j,lines;
    FILE *fin,*fout;

    switch (argc)
    {
    case 1:
        printf(" Enter fin: ");
        scanf("%s",fnamein);
        printf("Enter fout: ");
        scanf("%s",fnameout);
        printf("Enter lines: ");
        scanf("%d",&lines);
        break;

    case 2:
        if(isalpha(argv[1][0])){
            strcpy(fnamein,argv[1]);
            printf("Enter lines: ");
            scanf("%d",&lines);
            change_ext(fnamein,fnameout);
        }
        else{
            get_lines_arg(argv[1],&lines);
            printf(" Enter fin: ");
            scanf("%s",fnamein);
            printf("Enter fout: ");
            scanf("%s",fnameout);
        }
        break;

    case 3:
        strcpy(fnamein,argv[1]);
        if(isalpha(argv[2][0])){
            strcpy(fnameout,argv[2]);
            printf("Enter lines: ");
            scanf("%d",&lines);
        }
        else{
            get_lines_arg(argv[2],&lines);
            change_ext(fnamein,fnameout);
        }
        break;

    default:
	strcpy(fnamein,argv[1]);
        strcpy(fnameout,argv[2]);
        get_lines_arg(argv[3],&lines);
    } 
    fin=fopen(fnamein,"rb");
    fout=fopen(fnameout,"wb");
    for(i=lines-1;i>=0;i--){
        fseek(fin,(long)i*256,SEEK_SET);
        fread(inbuf,1,256,fin);
        for(j=0;j<256;j++) outbuf[j]=inbuf[255-j];
        fwrite(outbuf,1,256,fout);
    }
    fclose(fin);
    fclose(fout);
}

get_lines_arg(s,n)

char s[];
int *n;

{
    FILE *f;
    char c;

    if(s[0]=='/') switch(s[1]){
    case 'd':
    case 'D':
        *n=256;
        break;
    case 'p':
    case 'P':
        f=fopen("param.dat","rb");
        fscanf(f,"%c %d",&c,n);
        break;
    default :
        error_exit();
    }
    else if( s[0] >='0' && s[0] <= '9') sscanf(s,"%d",n);
    else error_exit();
}

error_exit()
{
    printf("bad parameters\n");
    exit();
}

change_ext(s1,s2)
char *s1,*s2;
{ 
    int i;
    do{
        *(s2++)=*(s1++);
    }
    while(*(s2-1)!='.');
    strcpy(s2,"flp");
}

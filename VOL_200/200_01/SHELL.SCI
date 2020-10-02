#
# Small C Interpreter command shell
#
char _nr, _nc, _ro, _co, *_cp, *_el, _mhz;

putchar(c){return sys(c,2,1);}
getchar(){return sys(1,2);}
puts(b){return sys(b,2,3);}
putd(n){return printf("%d\n",n);}
putx(n){return printf("%x\n",n);}
puto(n){return printf("%o\n",n);}
gets(b){return sys(b,80,1,4);}
fputc(c,u){return sys(c,u,1);}
fgetc(u){return sys(u,2);}
fputs(b,u){return sys(b,u,3);}
fgets(b,n,u){return sys(b,n,u,4);}
sprintf(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9){sys(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,5);}
printf(a0,a1,a2,a3,a4,a5,a6,a7,a8)
{char b[128];sys(b,a0,a1,a2,a3,a4,a5,a6,a7,a8,5);puts(b);}
sscanf(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9){sys(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,6);}
scanf(a0,a1,a2,a3,a4,a5,a6,a7,a8)
{char b[128];gets(b);sys(b,a0,a1,a2,a3,a4,a5,a6,a7,a8,6);}
atoi(b){int v;sys(b,"%d",&v,6);return v;}
fopen(f,m){return sys(f,m,7);}
fread(s,l,u){return sys(s,l,u,8);}
fwrite(s,l,u){return sys(s,l,u,9);}
fclose(u){return sys(u,10);}
fseek(u,o,w){return sys(u,o,w,11);}
ftell(u){return sys(u,12);}
bdos(a,b){return sys(a,b,13);}
system(s){return sys(s,14);}
exit(){sys(15);}
strcmp(s,t){return sys(s,t,20);}
strncmp(s,t,n){return sys(s,t,n,20);}
strcpy(s,t){return sys(s,t,21);}
strncpy(s,t,n){return sys(s,t,n,21);}
strlen(s){return sys(s,22);}
malloc(n){return sys(n,23);}
free(p){sys(p,24);}
debug(n){sys(n,28);}
check(n){sys(1,n,28);}
dirscan(p,f){return sys(p,f,29);}
int86(i,e,r){return sys(i,e,r,30);}
memleft(){return sys(31);}

entry
main()
{
   int f, t;
   char buf[24];
   char line[81];
   char program[ memleft()-1024 ];

   puts(sys(0));
   puts("\nSCI Shell V1.5 20Oct86 Copyright (C) 1986 Bob Brodt\n");
   *program='Z';
   _mhz=12; 

   _nr=25; _nc=80;
   _ro=_co=1;
   _cp="\033[%d;%dH";
   _el="\033[0K";

   for(;;) {
      puts("shell> ");
      line[5]=0;
      if(gets(line)) {
         if (!strncmp(line,"edit",4))
            sys(atoi(line+4),program,19);
         else if (!strncmp(line,"list",4)) {
            f=1;
            t=32765;
            if(line[4])
               sscanf(line+4,"%d %d",&f,&t);
            sys(program,f,t,27);
         }
         else if (!strncmp(line,"save",4))
            sys(line+5,program,26);
         else if (!strncmp(line,"load",4))
            sys(line+5,program,25);
         else if (!strncmp(line,"exit",4))
            return;
         else if (!strncmp(line,"dir",3)) {
            if ( !line[3] )
               strcpy(line+4,"*.*");
            if ( dirscan(line+4,buf) ) {
               printf("%s\n",buf);
               while(dirscan(0,buf))
                  printf("%s\n",buf);
            }
         }
         else
            printf("\n%d\n",sys(line,program,16));
      }
   }
}

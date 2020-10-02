#
# Sample file copy program for SCI (version 1.4)
#
copy(f,t)
char *f, *t;
{
   int infd, outfd;
   char buf[128],len;
   
   if ( (infd=fopen(f,"r")) < 0 )
      return err(f);
   if ( (outfd=fopen(t,"w")) < 0 )
      return err(t);
   while( (len=fread(buf,128,infd))>0 )
      fwrite(buf,len,outfd);
   fclose(outfd);
   fclose(infd);
}
err(f)
{
   printf("can't open <%s>\n",f);
}


#include<sys/io.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>

void logputs(char *log,char *str)
{
 int f;

 f=open(log,O_APPEND|O_TEXT|O_WRONLY);
 if(f<0)
  f=open(log,O_CREAT|O_TEXT|O_WRONLY,S_IREAD|S_IWRITE);
 if(f>=0)
 {
  write(f,str,strlen(str));
  write(f,"\n",1);
  close (f);
 }//endif
 puts(str);
}//end
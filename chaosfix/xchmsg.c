
#include <sys/io.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <alloc.h>

extern char exepath[80];
char *messages=NULL;
char *xMSG;

int initMSG(char *filename)
{
 int f,i;
 char str[80];

 sprintf(str,"%s%s",exepath,filename);
 f=open(str,"rt");
 if(f<0) return 0;

 if(messages==NULL)messages=(char *)farmalloc(8001);
 if(messages==NULL)return 0;
 if(xMSG==NULL)xMSG=(char *)farmalloc(1024);
 if(xMSG==NULL)return 0;

 i=read(f,messages,8000);
 close(f);
 if(i<0) return 0;
 messages[i]='\0';
 return 1;
}

void getMSG(char *handle)
{
 char *ptr;
 char str[40];

 if(xMSG==NULL||messages==NULL)return;

 sprintf(str,"\n%s \"",handle);
 ptr=strstr(messages,str);
 if(ptr)
 {
  strncpy(xMSG,&ptr[strlen(str)],1023);
  ptr=strstr(xMSG,"\"\n");
  if(ptr)*ptr='\0';else xMSG[1023]='\0';
 }
 else
  sprintf(xMSG,"<unknown message with handle %s>",handle);
}//end
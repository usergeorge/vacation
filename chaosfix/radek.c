#include <string.h>
#include <malloc.h>
#include <sys/io.h>
#include <fcntl.h>
#include "radek.h"

//komentar ';': umazat mezery pred zacatkem radky
void killstrednik(char *buf,int i,int line)
{
 int x;
 buf[i]='\0';
 x=i-1;
 while(x>line && buf[x]==' ')buf[x--]='\0';
}//end sub

void AddLine(struct radek **rad, char *str)
{
 struct radek *pom,*last;

 pom=malloc(sizeof(struct radek));
 if(pom==NULL)memerr();
 strcpy(pom->str,str);
 pom->next=NULL;
 if(*rad==NULL) *rad=pom;
 else
 {
  last=*rad;
  while(last->next!=NULL)last=last->next;
  last->next=pom;
 }//endif
}//end


int LoadList(char *dir_frq, struct radek **ptr,char firstwordonly)
{
 int f,i=0,line=0,j=0,rv=1;
 char *buf;

 buf=malloc(32001);
 if(buf==NULL)memerr(); //memerr();

 f=open(dir_frq,"rt");
 if(f<0)rv=2;
 if(f>=0)j=read(f,buf,32000);
 if(j<0)rv=2;
 if(!eof(f))rv=4;//file too long
 close(f);
 if(rv!=1)goto direrr;
 buf[j]='\0';

 while(i<j)
 {
  if(buf[i]==' ' && firstwordonly) buf[i]='\0';
  if(buf[i]==';')killstrednik(buf,i,line);
  if(buf[i]=='\n')
  {
   buf[i]='\0';
   AddLine(ptr,&buf[line]);
   line=i+1;
  }//endif
  i++;
 }//loop
 direrr:
 free(buf);
 return rv;
}//end sub

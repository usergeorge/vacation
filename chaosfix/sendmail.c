
//new version includes DateTime

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/dir.h>
/* #include <dos.h> */

#include "netmail.h"

char *FidoMonth[]={
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

extern char registered[40];
extern char *VER;
extern char log[80];
extern char netmail[80];
extern char *name;

void sendmail(char *fromuser,struct FidoAdr *from,char *touser,
              struct FidoAdr *to,char *subject,char *file,int mode)
//mode 0=send buffer *file as text / 1=attach file defined in *file & send direct
{
 char str[512];
 int f,max;
 char *ptr;
// char *err="Error writing temporary file !?";
 char *err="Error writing netmail message !";
 char mail[80],pom1[20]="\0",pom2[20]="\0",pom3[20]="\0";
 struct ffblk ff;
 struct Netmail msg;
 struct date d;
 struct time t;

 if(!mode)
 {
  sprintf(str,"Sending mail to %d:%d/%d.%d",to->zone,to->net,to->node,to->point);
  logputs(log,str);
 }

 //binary header reset
 memset(&msg,0,sizeof(struct Netmail));
 memset(msg.Subject,' ',71);

 //text fields
 strcpy(msg.FromUserName,fromuser);
 strcpy(msg.ToUserName,touser);
 strcpy(msg.Subject,subject);

 //date
 gettime(&t);
 getdate(&d);
 if(d.da_year>200)
 {
  if(d.da_year>1999) d.da_year-=2000;
  else d.da_year-=1900;
 }
 sprintf(msg.DateTime,"%02d %s %02d  %02d:%02d:%02d ",
 d.da_day,FidoMonth[(d.da_mon)-1],d.da_year,t.ti_hour,t.ti_min,t.ti_sec);

 //binary fields
 msg.DestNode=to->node;
 msg.DestNet=to->net;
 msg.DestZone=to->zone;
 msg.DestPoint=to->point;
 msg.OrigNode=from->node;
 msg.OrigNet=from->net;
 msg.OrigZone=from->zone;
 msg.OrigPoint=from->point;
 msg.AttributeWord|=0x0180; //Kill/sent 7 Local 8
 if(mode) msg.AttributeWord|=0x0010; //attach 4
 if(mode==1) strcpy(pom3,"FLAGS DIR\n");
 if(mode==2) strcpy(pom3,"FLAGS DIR KFS\n");

 //mail text
 if(from->point!=0)sprintf(pom1,"FMPT %d\n",from->point);
 if(to->point!=0)sprintf(pom2,"TOPT %d\n",to->point);
 sprintf(str,"\
INTL %d:%d/%d %d:%d/%d\n\
%s\
%s\
MSGID %d:%d/%d.%d %lx\n\
%s\
PID %s %s\n",
         to->zone,to->net,to->node,from->zone,from->net,from->node,
         pom1,pom2,
         from->zone,from->net,from->node,from->point,time(NULL),
         pom3,name,VER);

 sprintf(mail,"%s*.MSG",netmail);
 f=findfirst(mail,&ff,0);
 max=0;
 while(!f)
 {
  ptr=strchr(ff.ff_name,'.');
  if (ptr!=NULL) *ptr='\0';
  f=atoi(ff.ff_name);
  if(f>max)max=f;
  f=findnext(&ff);
 }
 sprintf(mail,"%s%d.MSG",netmail,max+1);

 save:
 f=open(mail,O_WRONLY|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
 if(f<0)
 {
  logputs(log,err);
  return;
 }
 write(f,&msg,sizeof(struct Netmail));
 close(f);
 f=open(mail,O_WRONLY|O_TEXT|O_APPEND);
 if(f<0)
 {
  logputs(log,err);
  return;
 }
 write(f,str,strlen(str));
 write(f,file,strlen(file));
 sprintf(str,"\n--- %s %s %s\n",name,VER,registered);
 write(f,str,strlen(str));
 close(f);

/*
 if(mode)
  sprintf(str,"%sxr.exe send %s %d/%d.%d /t $$$$TEXT.TMP /s \"%s\"",exepath,
          subject,to->net,to->node,to->point,touser);
 else
  sprintf(str,"%sxr.exe text \"%s\" %d/%d.%d /t $$$$TEXT.TMP /s \"%s\"",exepath,
          subject,to->net,to->node,to->point,touser);
 system(str);
*/

// unlink("$$$$TEXT.TMP");
// unlink("XROBOT.CTL");
}


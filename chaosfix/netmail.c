//zpracovani jednoho netmailu

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <dir.h>
#include <fcntl.h>
#include <sys\stat.h>

#include "netmail.h"

int readmsg(char *msgname,
            struct FidoAdr *my,struct FidoAdr *to,struct FidoAdr *reply,
             struct Netmail *msg,char *buf, char *robotnames,int *len)

{
 int f,i;
 char *ptr;
 struct FidoAdr *pom=NULL;

 f=open(msgname,O_BINARY|O_RDONLY);
 if (f<0)
 {
  perror(msgname);
  return(2);
 }//endif
 *len=read(f,buf,MSGSIZE);
 close(f);
 if (*len<sizeof(struct Netmail)) return(2); //message is not a valid Fido Netmail

 //je to pro me - zhruba ? jestli ne, koncim.
 memcpy(msg,buf,sizeof(struct Netmail));
 if(strstr(strupr(robotnames),strupr(msg->ToUserName))==NULL) return 0;

 // cilova Fido adresa:
 to->net=msg->DestNet;
 to->node=msg->DestNode;

 //compare net/node with ALL my AKA's jestli ne, tak to uz vazne zabalim
 pom=my;
 while(pom!=NULL)
 {
  if(pom->net==to->net && pom->node==to->node) break;
  pom=pom->next;
 }
 if(pom==NULL)return 5;

 ptr=strstr(&buf[sizeof(struct Netmail)],"TOPT ");
 if(ptr!=NULL)
 {
  i=6;while(i++<11)if(ptr[i]=='\n')ptr[i]='\0';
  to->point=atoi(&ptr[6]);
 }
 else
  to->point=0;

 //je to fuckt pro me ? jestli ne, tak to uz vazne zabalim
 pom=my;
 while(pom!=NULL)
 {
  if(pom->net==to->net && pom->node==to->node && pom->point==to->point) break;
  pom=pom->next;
 }//loop
 if(pom==NULL)return 5;

 //zona je stejna jako zona meho AKA
 to->zone=pom->zone;

 // Fido adresa odesilatele
 reply->zone=to->zone;
 reply->net=msg->OrigNet;
 reply->node=msg->OrigNode;
 ptr=strstr(&buf[sizeof(struct Netmail)],"FMPT ");
 if(ptr!=NULL)
 {
  i=6;while(i++<11)if(ptr[i]=='\n')ptr[i]='\0';
  reply->point=atoi(&ptr[6]);
 }
 else
  reply->point=0;

 return 1;
}//end sub

void fido2str(struct FidoAdr *fido, char *str)
{
 sprintf(str,"%d:%d/%d.%d",fido->zone,fido->net,fido->node,fido->point);
}//end sub

void str2fido(char *str,struct FidoAdr *fido,struct FidoAdr *defaultadr)
{
 char *ptr,*s,wrk[80];

 memcpy(fido,defaultadr,sizeof(struct FidoAdr));
 fido->point=0;
 strcpy(wrk,str);
 s=wrk;
 ptr=strchr(s,',');
 if(ptr==NULL)ptr=strchr(s,' ');
 if(ptr){s=&ptr[1];while(*s==' ')s++;}
 ptr=strchr(wrk,':');
 if(ptr!=NULL)
  {*ptr='\0';fido->zone=atoi(s);s=&ptr[1];}//zone ?
 ptr=strchr(s,'/');
 if(ptr!=NULL)
  {*ptr='\0';fido->net=atoi(s);s=&ptr[1];}//net ?
 ptr=strchr(s,'.');
 if(ptr!=NULL)  //point ?
 {
  *ptr='\0';
  if(s[0])fido->node=atoi(s);
  s=&ptr[1];
  fido->point=atoi(s);
 }
 else
  fido->node=atoi(s);

 fido->next=NULL;
}//end sub
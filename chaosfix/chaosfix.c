// Fido ROBOT "ChaosFix", (c)1996 xChaos software

#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/dir.h>
#include <time.h>
/* #include <dos.h> */
/* #include <conio.h> */
#include <fcntl.h>
#include <sys/stat.h>

#include "netmail.h"
#include "radek.h"
#include "xchmsg.h"

unsigned _stklen=24000u;

char registered[40]="[GNU freeware]";
char *VER="1.11";
char log[80]="chaosfix.log";
char *name="ChaosFix";
char warning[40]="WARNING;";
const char *nulladr="0:0/0.0";

struct radek
 *directory=NULL,*uplink=NULL,*adress=NULL,*friendaka=NULL,
 *freestring=NULL,*echostring=NULL,*transitstring=NULL,*privatestring=NULL;

struct heslo
{
 struct FidoAdr adr;
 char pwd[80];
 struct heslo *next;
} *password=NULL;

struct Tic
{
 char Area[80];         //MDS_GROU.UF
 char Areadesc[80];     // Music - Group musicdisks
 char Origin[80];       // 2:423/66
 char From[80];         // 2:420/100.0
 char To[80];           // Michael Polak, 2:420/85.0
 char File[80];         // HALO01.ZIP
 char Desc[80];         // Halo MusicDisk / Blight Productions
 char Magic[80];
 char Replaces[80];
 struct radek *LDesc;
 unsigned long Crc; // 56C0BC1A
 // Created by....
 struct radek *Path;
 /*
 Path 2:423/66 813672345 Sat Oct 14 13:05:45 1995 UTC+0100
 Path 2:420/100.0 813922690 Tue Oct 17 05:38:10 1995 UTC+0200
 */
 struct radek *SeenBy;
 /*
 Seenby 2:420/100.0
 Seenby 2:421/18.0
 */
 char Pw[80];// MJGOESF2
};

char inbound[80]="IN\\";
char netmail[80]="NETMAIL\\";
char dir_frq[80]="\0";
char update_frq[80]="\0";
char password_lst[80]="\0";
int areastop;
char *underline="-----------------------------------------------------------------------------";
char err[80]="\0";
char change=0;
char newpath[80];
char ticpath[80];
char badpath[80];
char transit[80];
char sysop[80]="Sysop";
char description[80]="LD";
char createpath='N';
char newareatype='E';

struct Area
{
 unsigned char type; //0..new,1..free,2..echo,3..transit,255..private
 char name[80];
 char desc[80];
 char path[80];
 char useAKA[80];
 char exec[80];
 char filesBBS[80];
 char addcomment[80];
 struct radek *rw;
 struct radek *ro;
 struct radek *sleeping;
 struct radek *inactive;
 struct radek *guest;
 struct radek *nomail;
 struct Area *next;
} *first_area=NULL,*area=NULL,*pom_area=NULL;

// prototypes----------------------------------------------------------------

unsigned long filecrc32(char *filename);

void memerr(void)
{
 getMSG("MEMERR");
 logputs(log,xMSG);
 exit(2);
}

void InitTic(struct Tic * tic)
{
 strcpy(tic->Area,"UNDEFINED_AREA");
 tic->Areadesc[0]='\0';
 strcpy(tic->Origin,nulladr);
 strcpy(tic->From,nulladr);
 strcpy(tic->To,adress->str);
 strcpy(tic->File,"NO_FILE");
 strcpy(tic->Desc,"?");
 tic->LDesc=NULL;
 tic->Crc=0ul;
 tic->Path=NULL;
 tic->SeenBy=NULL;
 strcpy(tic->Pw,"?");
 tic->Replaces[0]='\0';
 tic->Magic[0]='\0';
}//end


void ProcessKeyword(char *in, char *keyword, char *out, int linelength)
{
 char kwd[256];
 int k;

 if(linelength>80)linelength=80;
 sprintf(kwd,"%s ",keyword);
 k=strlen(kwd);
 if(strncmpi(in,kwd,k)==0)
 {
  strncpy(out,&in[k],linelength-k);
  out[linelength-k]='\0';
 }//endif
}//end

int LoadTic(struct Tic *tic,char *ticname)
{
 int f,i=0,line=0,j=0,rv=1;
 char *ticbuf;
 char str[80];

 ticbuf=malloc(16001);
 if(ticbuf==NULL)memerr();

 f=open(ticname,"rt");
 if(f<0)rv=2;
 if(f>=0)j=read(f,ticbuf,16000);
 if(j<0)rv=2;
 close(f);
 if(rv!=1)goto ticerr;

 InitTic(tic);
 ticbuf[j]='\0';

 while(i<j)
 {
  if(ticbuf[i]=='\n')
  {
   ticbuf[i]='\0';

   //zpracovani klicovych slov v tic souboru
   ProcessKeyword(&ticbuf[line],"AREA",tic->Area,i-line);
   ProcessKeyword(&ticbuf[line],"AREADESC",tic->Areadesc,i-line);
   ProcessKeyword(&ticbuf[line],"ORIGIN",tic->Origin,i-line);
   ProcessKeyword(&ticbuf[line],"FROM",tic->From,i-line);
   ProcessKeyword(&ticbuf[line],"TO",tic->To,i-line);
   ProcessKeyword(&ticbuf[line],"FILE",tic->File,i-line);
   ProcessKeyword(&ticbuf[line],"DESC",tic->Desc,i-line);
   ProcessKeyword(&ticbuf[line],"MAGIC",tic->Magic,i-line);
   ProcessKeyword(&ticbuf[line],"REPLACES",tic->Replaces,i-line);
   ProcessKeyword(&ticbuf[line],"PW",tic->Pw,i-line);
   str[0]='\0';
   ProcessKeyword(&ticbuf[line],"CRC",str,i-line);
   if(str[0]!='\0')sscanf(str,"%lX",&(tic->Crc));
   str[0]='\0';
   ProcessKeyword(&ticbuf[line],"LDESC",str,i-line);
   if(str[0]!='\0') AddLine(&(tic->LDesc),str);
   str[0]='\0';
   ProcessKeyword(&ticbuf[line],"SEENBY",str,i-line);
   if(str[0]!='\0') AddLine(&(tic->SeenBy),str);
   str[0]='\0';
   ProcessKeyword(&ticbuf[line],"PATH",str,i-line);
   if(str[0]!='\0') AddLine(&(tic->Path),str);

   line=i+1;
  }
  i++;
 }//loop
 ticerr:
 free(ticbuf);
 return rv;
}//end sub


int LoadPasswords(char *name, struct FidoAdr *my)
{
 int f,i=0,line=0,j=0,rv=1;
 char *pwdbuf, *ptr;
 struct heslo *pom,*last=NULL;

 pwdbuf=malloc(32001);
 if(pwdbuf==NULL)memerr();

 f=open(name,"rt");
 if(f<0)rv=2;
 if(f>=0)j=read(f,pwdbuf,32000);
 if(j<0)rv=2;
 if(!eof(f))rv=4;//file too long
 close(f);
 if(rv!=1)goto pwderr;
 pwdbuf[j]='\0';

 while(i<j)
 {
  if(pwdbuf[i]==';')killstrednik(pwdbuf,i,line);
  if(pwdbuf[i]=='\n')
  {
   pwdbuf[i]='\0';
   if(pwdbuf[line])
   {
    pom=malloc(sizeof(struct heslo));
    if(pom==NULL)memerr();
    pom->next=NULL;
    ptr=strchr(&pwdbuf[line],' ');
    if(ptr!=NULL)
    {
     while(*ptr==' ') *ptr++='\0';
     strcpy(pom->pwd,ptr);
    }
    else
     pom->pwd[0]='\0';
    str2fido(&pwdbuf[line],&(pom->adr),my);
    if (password==NULL) password=pom; else if(last!=NULL) last->next=pom;
    last=pom;
   }//endif
   line=i+1;
  }//endif
  i++;
 }//loop
 pwderr:
 free(pwdbuf);
 return rv;
}//end sub

void AllocArea(void)
{
 pom_area=malloc(sizeof(struct Area));
 if(pom_area==NULL)memerr();
 if(first_area==NULL)
  first_area=pom_area;
 else
 {
  area=first_area;
  while(area->next)area=area->next;
  area->next=pom_area;
 }
 area=pom_area;
 area->next=NULL;
 area->useAKA[0]='\0';
 area->nomail=NULL;
 area->guest=NULL;
 area->rw=NULL;
 area->ro=NULL;
 area->sleeping=NULL;
 area->inactive=NULL;
 area->exec[0]='\0';
 area->filesBBS[0]='\0';
 area->addcomment[0]='\0';
}//endif

char *buf0;

int LoadCtl(void)
{
 int f,i=0,line=0,j=0,rv=1,section=0,areatype;
 char *buf;
 char str[80];

 buf=malloc(32001);
 if(buf==NULL)memerr();
 if(buf0==NULL)buf0=malloc(32001);
 if(buf0==NULL)memerr();

 f=open("chaosfix.ctl","rb");
 if(f<0)rv=2;

 if(f>=0) j=read(f,buf,32000);
 if(j<0)rv=2;
 if(!eof(f))rv=4;
 close(f);
 if(rv!=1)goto errctl;

 areastop=0;
 buf[j]='\0';
 memcpy(buf0,buf,j);

 while(i<j)
 {
  if(buf[i]=='\r'){buf[i]='\0';}
  if(buf[i]==';') killstrednik(buf,i,line);
  if(buf[i]=='\n')
  {
   buf[i]='\0';
   str[0]='\0';

   //zpracovani klicovych slov v CTL souboru - 1.cast

   ProcessKeyword(&buf[line],"ADRESS",str,i-line);
   ProcessKeyword(&buf[line],"ADDRESS",str,i-line);
   ProcessKeyword(&buf[line],"AKA",str,i-line);
   if(str[0]) {AddLine(&adress,str);goto section2;}
   ProcessKeyword(&buf[line],"FREESTRING",str,i-line);
   if(str[0]) {AddLine(&freestring,str);goto section2;}
   ProcessKeyword(&buf[line],"ECHOSTRING",str,i-line);
   if(str[0]) {AddLine(&echostring,str);goto section2;}
   ProcessKeyword(&buf[line],"TRANSITSTRING",str,i-line);
   if(str[0]) {AddLine(&transitstring,str);goto section2;}
   ProcessKeyword(&buf[line],"PRIVATESTRING",str,i-line);
   if(str[0]) {AddLine(&privatestring,str);goto section2;}
   ProcessKeyword(&buf[line],"UPLINK",str,i-line);
   if(str[0]) {AddLine(&uplink,str);goto section2;}
   ProcessKeyword(&buf[line],"FRIEND",str,i-line);
   if(str[0]) {AddLine(&friendaka,str);goto section2;}
   ProcessKeyword(&buf[line],"DIRECTORIES",str,i-line);
   if(str[0]) {strcpy(dir_frq,str);goto section2;}
   ProcessKeyword(&buf[line],"LOGFILE",str,i-line);
   if(str[0]) {strcpy(log,str);goto section2;}
   ProcessKeyword(&buf[line],"UPDATEFRQ",str,i-line);
   if(str[0]) {strcpy(update_frq,str);goto section2;}
   ProcessKeyword(&buf[line],"PASSWORDS",str,i-line);
   if(str[0]) {strcpy(password_lst,str);goto section2;}
   ProcessKeyword(&buf[line],"INBOUND",str,i-line);
   if(str[0]) {strcpy(inbound,str);goto section2;}
   ProcessKeyword(&buf[line],"NETMAIL",str,i-line);
   if(str[0]) {strcpy(netmail,str);goto section2;}
   ProcessKeyword(&buf[line],"NEW",str,i-line);
   if(str[0]) {strcpy(newpath,str);goto section2;}
   ProcessKeyword(&buf[line],"SYSOP",str,i-line);
   if(str[0]) {strcpy(sysop,str);goto section2;}
   ProcessKeyword(&buf[line],"COMMENTFILES",str,i-line);
   if(str[0]) {strcpy(description,str);goto section2;}
   ProcessKeyword(&buf[line],"NEWAREATYPE",str,i-line);
   if(str[0]) {newareatype=str[0];goto section2;}
   ProcessKeyword(&buf[line],"CREATEPATH",str,i-line);
   if(str[0]) {createpath=str[0];goto section2;}

   section2:
   if(str[0] && section)
   {
    rv=6;
    strcpy(err,&buf[i-line]);
    break;
   }

   str[0]='\0';
   areatype=0;
   ProcessKeyword(&buf[line],"FREE",str,i-line);
   if(str[0]){areatype=1;goto nextarea;}
   ProcessKeyword(&buf[line],"ECHO",str,i-line);
   if(str[0]){areatype=2;goto nextarea;}
   ProcessKeyword(&buf[line],"TRANSIT",str,i-line);
   if(str[0]){areatype=3;goto nextarea;}
   ProcessKeyword(&buf[line],"PRIVATE",str,i-line);
   if(str[0])areatype=255;
   else goto contarea;
   nextarea:
   if(str[0])
   {
    if(areastop==0)areastop=line;
    section=1;
    //add new fileecho area

    AllocArea();
    area->type=areatype;
    strcpy(area->name,str);
    getMSG("NEWAREA");
    strcpy(area->desc,xMSG);
    strcpy(area->path,newpath);
    area->useAKA[0]='\0';
   }//endif

   contarea:
   str[0]='\0';
   ProcessKeyword(&buf[line],"PATH",str,i-line);
   if(area!=NULL && str[0]){strcpy(area->path,str);goto sect;}
   ProcessKeyword(&buf[line],"DESC",str,i-line);
   if(area!=NULL && str[0]){strcpy(area->desc,str);goto sect;}
   ProcessKeyword(&buf[line],"USEAKA",str,i-line);
   if(area!=NULL && str[0]){strcpy(area->useAKA,str);goto sect;}
   ProcessKeyword(&buf[line],"EXEC",str,i-line);
   if(area!=NULL && str[0]){strcpy(area->exec,str);goto sect;}
   ProcessKeyword(&buf[line],"FILESBBS",str,i-line);
   if(area!=NULL && str[0]){strcpy(area->filesBBS,str);goto sect;}
   ProcessKeyword(&buf[line],"ADDCOMMENT",str,i-line);
   if(area!=NULL && str[0]){strcpy(area->addcomment,str);goto sect;}
   ProcessKeyword(&buf[line],"GUEST",str,i-line);
   if(area!=NULL && str[0]){AddLine(&(area->guest),str);goto sect;}
   ProcessKeyword(&buf[line],"NOMAIL",str,i-line);
   if(area!=NULL && str[0]){AddLine(&(area->nomail),str);goto sect;}
   ProcessKeyword(&buf[line],"RW",str,i-line);
   if(area!=NULL && str[0]){AddLine(&(area->rw),str);goto sect;}
   ProcessKeyword(&buf[line],"RO",str,i-line);
   if(area!=NULL && str[0]){AddLine(&(area->ro),str);goto sect;}
   ProcessKeyword(&buf[line],"SLEEPING",str,i-line);
   if(area!=NULL && str[0]){AddLine(&(area->sleeping),str);goto sect;}
   ProcessKeyword(&buf[line],"INACTIVE",str,i-line);
   if(area!=NULL && str[0])AddLine(&(area->inactive),str);

   sect:
   if(str[0] && (!section || area==NULL))
   {
    rv=8;
    strcpy(err,&buf[line]);
    break;
   }

   line=i+1;
  }
  i++;
 }//loop
 if(areastop==0)areastop=j;
 errctl:
 free(buf);
 return rv;
}//end sub

void timestring(char *ret)
{
 struct date d;
 struct time t;

 gettime(&t);
 getdate(&d);
 sprintf(ret,"%2d:%02d:%02d %d/%d/%d",t.ti_hour,t.ti_min,t.ti_sec,d.da_day,d.da_mon,d.da_year);
}//end sun

int SaveTic(struct Tic * tic,char * ticname)
{
 char str[256];//,pom[64];
 struct radek *r;
 int f;

 f=open(ticname,"wt");
 if(f>=0)
 {
  sprintf(str,"Created by %s %s %s, (c)1996-7 xChaos software\n",name,VER,registered);
  write(f,str,strlen(str));
  sprintf(str,"Area %s\n",tic->Area);
  write(f,str,strlen(str));
  sprintf(str,"Areadesc %s\n",tic->Areadesc);
  write(f,str,strlen(str));
  sprintf(str,"Origin %s\n",tic->Origin);
  write(f,str,strlen(str));
  sprintf(str,"From %s\n",tic->From);
  write(f,str,strlen(str));
  sprintf(str,"To %s\n",tic->To);
  write(f,str,strlen(str));
  sprintf(str,"File %s\n",tic->File);
  write(f,str,strlen(str));
  if(tic->Magic[0])
  {
   sprintf(str,"Magic %s\n",tic->Magic);
   write(f,str,strlen(str));
  }
  if(tic->Replaces[0])
  {
   sprintf(str,"Replaces %s\n",tic->Replaces);
   write(f,str,strlen(str));
  }
  sprintf(str,"Desc %s\n",tic->Desc);
  write(f,str,strlen(str));

  r=tic->LDesc;
  while(r)
  {
   sprintf(str,"LDesc %s\n",r->str);
   if(str[0])write(f,str,strlen(str));
   r=r->next;
  }//loop
  r=tic->Path;
  while(r)
  {
   sprintf(str,"Path %s\n",r->str);
   if(str[0])write(f,str,strlen(str));
   r=r->next;
  }//loop
  r=tic->SeenBy;
  while(r)
  {
   sprintf(str,"SeenBy %s\n",r->str);
   if(str[0])write(f,str,strlen(str));
   r=r->next;
  }//loop

  sprintf(str,"Crc %lX\n",tic->Crc);
  if(str[0])write(f,str,strlen(str));
  if(tic->Pw[0])
  {
   sprintf(str,"Pw %s\n",tic->Pw);
   if(str[0])write(f,str,strlen(str));
  }
  close(f);
  return 1;
 }
 else
  return 2;
}//end sub

int SaveCtl(void)
{
 char str[256],p[64];
 struct radek *pom;
 int f;

 getMSG("WRITINGCTL");
 puts(xMSG);
 f=open("chaosfix.ctl","wb");
 if(f>=0)
 {
  area=first_area;
  write(f,buf0,areastop);
  while(area!=NULL)
  {
   switch(area->type)
   {
    case 1:
    sprintf(str,"Free %s\r\n",area->name);
    break;
    case 2:
    sprintf(str,"Echo %s\r\n",area->name);
    break;
    case 3:
    sprintf(str,"Transit %s\r\n",area->name);
    break;
    default:
    sprintf(str,"Private %s\r\n",area->name);
   }
   write(f,str,strlen(str));
   if(area->desc[0])
   {
    sprintf(str,"Desc %s\r\n",area->desc);
    write(f,str,strlen(str));
   }
   if(area->type!=3)
   {
    sprintf(str,"Path %s\r\n",area->path);
    write(f,str,strlen(str));
   }
   if(area->useAKA[0])
   {
    sprintf(str,"UseAKA %s\r\n",area->useAKA);
    write(f,str,strlen(str));
   }
   if(area->exec[0])
   {
    sprintf(str,"Exec %s\r\n",area->exec);
    write(f,str,strlen(str));
   }
   if(area->filesBBS[0])
   {
    sprintf(str,"FilesBBS %s\r\n",area->filesBBS);
    write(f,str,strlen(str));
   }
   if(area->addcomment[0])
   {
    sprintf(str,"AddComment %s\r\n",area->addcomment);
    write(f,str,strlen(str));
   }
   pom=area->guest;
   while(pom!=NULL)
   {
    sprintf(str,"Guest %s\r\n",pom->str);
    write(f,str,strlen(str));
    pom=pom->next;
   }//loop
   pom=area->nomail;
   while(pom!=NULL)
   {
    sprintf(str,"NoMail %s\r\n",pom->str);
    write(f,str,strlen(str));
    pom=pom->next;
   }//loop
   pom=area->rw;
   while(pom!=NULL)
   {
    sprintf(str,"Rw %s\r\n",pom->str);
    write(f,str,strlen(str));
    pom=pom->next;
   }//loop
   pom=area->ro;
   while(pom!=NULL)
   {
    sprintf(str,"Ro %s\r\n",pom->str);
    write(f,str,strlen(str));
    pom=pom->next;
   }//loop
   pom=area->sleeping;
   while(pom!=NULL)
   {
    sprintf(str,"Sleeping %s\r\n",pom->str);
    write(f,str,strlen(str));
    pom=pom->next;
   }//loop
   pom=area->inactive;
   while(pom!=NULL)
   {
    sprintf(str,"Inactive %s\r\n",pom->str);
    write(f,str,strlen(str));
    pom=pom->next;
   }//loop
   write(f,";",1);
   write(f,underline,strlen(underline));
   write(f,"\r\n",2);
   area=area->next;
  }//loop
  timestring(p);
  sprintf(str,";Areas written by %s version %s %s, (c)1996-7 xChaos software\r\n;Time: %s\r\n",name,VER,registered,p);
  write(f,str,strlen(str));
  close(f);
  getMSG("CTLWROK");
  logputs(log,xMSG);
  return 1;
 }
 getMSG("CTLWRERR");
 logputs(log,xMSG);
 return 2;
}//end sub


void readtpl(char *buf)
{
 int fi,f;
 char str[80];

 sprintf(str,"%s%s.TPL",exepath,name);
 fi=open(str,"rt");
 if(fi>=0)
 {
  f=read(fi,buf,2048);
  close(fi);
 }
 else f=0;
 buf[f]='\0';
}//end sub


int testuser(char *username,struct FidoAdr *adr,struct FidoAdr *my,char *pwd)
{
 struct heslo *pom;
 char buf[MSGSIZE],str[256],pomstr[32];
 int rv=2;

 fido2str(adr,pomstr);
 pom=password;
 while(pom!=NULL)
 {
  if(memcmp(&(pom->adr),adr,8)==0)
  {
   if(strcmpi(pom->pwd,pwd)==0)
    return 1; //ok
   else
   {
    readtpl(buf); //illegal password
    getMSG("ILLEGALPWD");
    sprintf(str,xMSG,username,pomstr,pwd);
    rv=4;
    goto securityviol;
   }//endif
  }//endif
  pom=pom->next;
 }//loop
 readtpl(buf); //not found
 getMSG("SORRYNOTKNOWN");
 sprintf(str,xMSG,username,pomstr,name);
 securityviol:
 strcat(buf,str);
 getMSG("SECVIOLATION");
 sprintf(str,xMSG,name,VER);
 sendmail(name,my,username,adr,str,buf,0);
 return rv;
}//end sub

char is_included_in_list(struct FidoAdr *kdo,struct radek *kde,struct FidoAdr *jako)
{
 struct radek *pom;
 struct FidoAdr a;

 pom=kde;
 while(pom!=NULL)
 {
  str2fido(pom->str,&a,jako);
  if(memcmp(&a,kdo,8)==0)return 1;
  pom=pom->next;
 }//loop
 return 0;
}//end sub

void del_from_list(struct FidoAdr *kdo,struct radek **kde,struct FidoAdr *jako)
{
 struct radek *pom,*last=NULL;
 struct FidoAdr a;

 pom=*kde;
 while(pom!=NULL)
 {
  str2fido(pom->str,&a,jako);
  if(memcmp(&a,kdo,8)==0)
  {
   if(last==NULL)
    *kde=pom->next;
   else
    last->next=pom->next;
   free(pom);
  }
  last=pom;
  pom=pom->next;
 }//loop
}//end sub

void CreateTicName(char *ret,char *kde)
{
 struct ffblk ff;
 int i;

 randomize();
 i=random(32000);
 do
 {
  sprintf(ret,"%sCHFX%X.TIC",kde,i);
  i++;
 }
 while(!findfirst(ret,&ff,0));

}//end sub

void DistributeFile(struct Tic *tic,char *filepath,struct Area *area,struct FidoAdr *aka,struct FidoAdr *komu_ne)
{
 char file[80],text[8050],str[256],flag;
 struct FidoAdr from,to;
 struct radek *komu;
 struct heslo *pw;

 readtpl(text);
 getMSG("MAILTEXT");
 sprintf(str,xMSG,tic->File,tic->Area,tic->Desc,underline);
 strcat(text,str);
 komu=tic->LDesc;
 while(komu)
 {
  strcat(text,komu->str);
  strcat(text,"\n");
  komu=komu->next;
 }//loop

 komu=area->rw;
 if(!komu){komu=area->ro;flag=1;}
 while(komu)
 {
  str2fido(komu->str,&to,aka);
  if(memcmp(komu_ne,&to,8))AddLine(&(tic->SeenBy),komu->str);
  komu=komu->next;
  if(!komu && !flag){komu=area->ro;flag=1;}
 }//loop

 if(area->useAKA[0])
  strcpy(tic->From,area->useAKA);
 else
  strcpy(tic->From,adress->str);
 timestring(file);
 sprintf(str,"%s %ld %s",tic->From,time(NULL),file);
 AddLine(&(tic->Path),str);
 str2fido(tic->From,&from,aka);
 strcpy(tic->Areadesc,area->desc);

 flag=0;
 komu=area->rw;
 if(!komu){komu=area->ro;flag=1;}
 while(komu)
 {
  str2fido(komu->str,&to,aka);
  if(memcmp(komu_ne,&to,8))
  {
   pw=password;
   while(pw)
   {
    if(!memcmp(&(pw->adr),&to,8)){strcpy(tic->Pw,pw->pwd);goto pswok;}
    pw=pw->next;
   }//loop
   tic->Pw[0]='\0';
   pswok:

   strcpy(tic->To,komu->str);
   getMSG("DSTR");
   sprintf(file,xMSG,tic->File,tic->Area,tic->To);
   logputs(log,file);
   strcpy(file,filepath);
   strcat(file,tic->File);
   if(is_included_in_list(&to,area->nomail,&from))
    sendmail(name,&from,"Sysop",&to,file,"",1);
   else
    sendmail(name,&from,"Sysop",&to,file,text,1);
   CreateTicName(file,ticpath);
   SaveTic(tic,file);
   sendmail(name,&from,"Sysop",&to,file,"",2); //tic
  }//endif
  komu=komu->next;
  if(!komu && !flag){komu=area->ro;flag=1;}
 }//loop
}//end sub

void getpath(struct Area *area, char *str)
{
 int c,d;
 struct radek *pomradek;

 if(!area->path[0])strcpy(area->path,newpath);

 if(area->type==3)
  strcpy(str,transit);
 else if(area->path[0]>='0' && area->path[0]<='9')
 {
  //hledani v DIR.FRQ podle cisla
  pomradek=directory;
  d=atoi(area->path);
  c=0;
  strcpy(str,badpath);
  while(pomradek!=NULL)
  {
   c++;
   if(d==c){strcpy(str,pomradek->str);break;}
   pomradek=pomradek->next;
  }//loop
 }
 else if(area->path[0]=='*')
 {
  //hledani v DIR.FRQ podle substringu
  pomradek=directory;
  strcpy(str,badpath);
  strupr(area->path);
  while(pomradek!=NULL)
  {
   if(strstr(strupr(pomradek->str),&(area->path[1])))
    {strcpy(str,pomradek->str);break;}
   pomradek=pomradek->next;
  }//loop
 }
 else
  strcpy(str,area->path);

 if(str[strlen(str)-1]!='\\')strcat(str,"\\");
}//end sub

void Diz2LDesc(char *batch,char *filename,struct radek **ld)
{
 char *ptr;
 char cmd[80],dizbuf[2501];
 int f,i;

 ptr=strchr(filename,'.');
 if(ptr==NULL) return;
 ptr++;
 strupr(ptr);
 sprintf(cmd,"%s%s_ID.BAT %s %s",exepath,batch,filename,ptr);
 system(cmd);
 f=open("FILE_ID.DIZ","rt");
 {
  i=read(f,dizbuf,2500);
  if(i>2500)i=2500;
  dizbuf[i]='\0';
  if(dizbuf[i-1]!='\n')strcat(dizbuf,"\n");
  ptr=dizbuf;
  i=0;
  while(dizbuf[i])
  {
   if(dizbuf[i]=='\n')
   {
    dizbuf[i]='\0';
    AddLine(ld,ptr);
    ptr=&dizbuf[i+1];
   }
   i++;
  }//loop
  close(f);
  unlink("FILE_ID.DIZ");
 }
}//end sub

void UpdateFilesBBS(struct Tic *tic,char *path,struct Area *area)
{
 int f,done=0,desc=0;
 char descrorder[10];
 char descstr[3000];
 char filename[80];
 char filesbbs[80];
 char fBBS[80];
 struct radek *r,*ld=NULL;

 strcpy(filename,path);
 strcat(filename,tic->File);
 sprintf(descstr,"%-12s [00] ",tic->File);

 if(!area->filesBBS[0])
  strcpy(fBBS,"FILES.BBS");
 else
  strcpy(fBBS,area->filesBBS);

 if(!area->addcomment[0])
  strncpy(descrorder,description,9);
 else
  strncpy(descrorder,area->addcomment,9);

 descrorder[9]='\0';
 strupr(descrorder);
 while(!done && desc<strlen(descrorder))
 {
  switch(descrorder[desc])
  {
   case 'L':
   r=tic->LDesc;
   ldesc:
   while(r)
   {
    if(done)strcat(descstr,"\n+");
    done=1;
    strcat(descstr,r->str);
    r=r->next;
   }//loop
   break;
   case 'D':
   strcat(descstr,tic->Desc);
   done=1;
   break;
   case 'F':
   Diz2LDesc("FIX",filename,&ld);
   r=ld;
   goto ldesc;
   case 'N':
   return;
  }
  desc++;
 }//loop

 strcat(descstr,"\n");
 strcpy(filesbbs,path);
 strcat(filesbbs,fBBS);
 f=open(filesbbs,"wt+");
 if(f<0)
  f=open(filesbbs,"wt");
 if(f>0)
 {
  char str[80];
  getMSG("ADDINGDESC");
  sprintf(str,xMSG,filesbbs);
  logputs(log,str);
  write(f,descstr,strlen(descstr));
  close(f);
 }//end if
}//end sub

void NotifySysop(struct FidoAdr *adr,char *text)
{
 char str[80];

 getMSG("NOTIFYSYSOP");
 sprintf(str,xMSG,name,VER);
 sendmail(name,adr,sysop,adr,str,text,0);
}//end sub


// main =====================================================================

char exepath[80];

int main(int argc,char *argv[])
{
 int f,i,j,len,fi,g,c;
 char buf[MSGSIZE],str[1024],pom[256],file[80];
 char *outbuf;
 struct Netmail msg;
 struct FidoAdr *my=NULL,*pomadr,*lastadr=NULL,to,reply,defaultadr;
 struct FidoAdr *a,b;
 char robotname[25]="chaosfix/allfix/filemgr";
 struct Tic *tic;
 struct radek *pomradek=NULL,*report;
 struct ffblk ff,fff;
 char drv[3],pth[65],jm[20],ext[5];
 char adr1[32],adr2[32],event=0;

 fnsplit(argv[0],drv,pth,jm,ext);
 strcpy(exepath,drv);
 strcat(exepath,pth);

 printf("\n\
%8s V%-4s FidoNet ROBOT, fileecho processor, (c)1996-7 xChaos software\n",name,VER);
 printf("ŽŽŽŽŽŽŽŽ\n\
               home of %s is 2:420/85 -> made in Prague.Cz :-)\n",name);

 //key
/*
 strcpy(str,exepath);
 strcat(str,"chaosfix.key");
 if(loadkey(str,registered))
 {
  if(registered[0]!='c' || registered[1]!='F')
  {
   logputs(log,"Illegal registration key !");
   exit(123);
  }//endif
  memmove(registered,&registered[2],32);
 }//endif
*/

 //nactu messages
 puts("\nReading 'Chaosfix.Msg'...");
 if(!initMSG("chaosfix.msg"))
 {
  logputs(log,"Error reading 'Chaosfix.Msg' !");
  exit(2);
 }
 getMSG("WARNING");
 strcpy(warning,xMSG);

 if(argc<2 || argv[1][1]=='?' || argv[1][0]=='?')
 {
  help:
  getMSG("HELP");
  printf(xMSG,name,name,name,name);
  exit(0);
 }//endif

 tic=malloc(sizeof(struct Tic));
 if(tic==NULL)memerr();
 outbuf=farmalloc(65000u);
 if(outbuf==NULL)memerr();

 strcpy(newpath, "?:\\");      /* fill string with form of response: X:\ */
 newpath[0] = 'A' + getdisk();    /* replace X with current drive letter */
 getcurdir(0, newpath+3);  /* fill rest of string with current directory */
 strcpy(ticpath,newpath);
 strcpy(badpath,newpath);
 strcpy(transit,newpath);
 strcat(ticpath,"\\TICS");
 strcat(newpath,"\\NEWAREAS\\");
 strcat(badpath,"\\BADPATH\\");
 strcat(transit,"\\TRANSIT\\");

 //load chaosfix.ctl
 getMSG("CTL");
 puts(xMSG);
 i=LoadCtl();
 switch(i)
 {
  case 2:
  getMSG("ERRCTL");
  logputs(log,xMSG);
  exit(2);
  case 4:
  getMSG("ERRCTLLONG");
  logputs(log,xMSG);
  exit(2);
  case 6:
  getMSG("ERRCTLSETUP");
  sprintf(pom,xMSG,err);
  goto errr;
  case 8:
  getMSG("ERRCTLAREAS");
  sprintf(pom,xMSG,err);
  errr:
  logputs(log,pom);
  getMSG("ERRKEYWORD");
  logputs(log,xMSG);
  exit(2);
 }

 if(inbound[strlen(inbound)-1]!='\\')strcat(inbound,"\\");
 if(netmail[strlen(netmail)-1]!='\\')strcat(netmail,"\\");
 defaultadr.zone=2;
 defaultadr.net=0;
 defaultadr.node=0;
 defaultadr.point=0;
 pomradek=adress;
 while(pomradek!=NULL)
 {
  pomadr=malloc(sizeof(struct FidoAdr));
  if(pomadr==NULL)memerr();
  str2fido(pomradek->str,pomadr,&defaultadr);
  if(my==NULL)
   my=pomadr;
  else
   if(lastadr!=NULL)lastadr->next=pomadr;
  lastadr=pomadr;
  pomradek=pomradek->next;
 }//loop
 if(my==NULL)
 {
  getMSG("ERRADR");
  logputs(log,xMSG);
  exit(2);
 }

 //load password.lst
 getMSG("READING");
 printf(xMSG,password_lst);
 if(password_lst[0])
 {
  i=LoadPasswords(password_lst, my);
  switch(i)
  {
   case 2:
   getMSG("ERRREAD");
   sprintf(pom,xMSG,password_lst);
   logputs(log,pom);
   exit(2);
   case 4:
   getMSG("ERRPWDLONG");
   logputs(log,xMSG);
   exit(2);
  }
 }//endif

 //load dir.frq
 getMSG("READING");
 printf(xMSG,dir_frq);
 if(dir_frq[0])
 {
  i=LoadList(dir_frq,&directory,1);
  switch(i)
  {
   case 2:
   getMSG("ERRREAD");
   sprintf(pom,xMSG,dir_frq);
   logputs(log,pom);
   exit(2);
   case 4:
   getMSG("ERRDIRLONG");
   logputs(log,xMSG);
   exit(2);
  }
 }//endif

/*
 getMSG("SELFCHECK");
 puts(xMSG);

 getMSG("SELFTEST");
 i=atoi(xMSG)-207;
 if(i!=(int)((filecrc32(argv[0]))%19731))
 {
  getMSG("ERRCHECK");
  logputs(log,xMSG);
  exit(2);
 }//endif
*/

 timestring(pom);
 sprintf(str,"\nþ %s %s %s session, %s",name,VER,registered,pom);
 logputs(log,str);

 switch(argv[1][0])
 {
  case 'e': //Event ---------------------------------------------------------
  case 'E':
  printf("\n");
  getMSG("EVENT");
  logputs(log,xMSG);
  event=1;
  goto mgr;
  case 'm': //Mgr -----------------------------------------------------------
  case 'M':
  printf("\n");
  getMSG("MGR");
  logputs(log,xMSG);
  mgr:
  getMSG("NETMAIL");
  puts(xMSG);
  sprintf(pom,"%s*.MSG",netmail);
  f=findfirst(pom,&ff,0);
  while(!f)
  {
   sprintf(pom,"%s%s",netmail,ff.ff_name);
   i=readmsg(pom,my,&to,&reply,&msg,buf,robotname,&len);
   switch(i)
   {
    char mailsw=0;

    case 1:
    fido2str(&reply,adr1);
    fido2str(&to,adr2);
    getMSG("REQUEST");
    sprintf(str,xMSG,pom,msg.FromUserName,adr1,msg.ToUserName,adr2);
    logputs(log,str);
    buf[len]='\0';
    j=sizeof(struct Netmail)-5;
    report=NULL;
    while(j<len)
    {
     switch (buf[j])
     {
      case '%':
      j++;
      mailsw=0;
      switch(buf[j])
      {
       case 'H'://help
       case 'h':
       logputs(log," Cmd: %HELP");

       sprintf(str,"%s%s.HLP",exepath,name);
       fi=open(str,"rt");
       if(fi>=0)
       {
        f=read(fi,outbuf,32000);
        close(fi);
       }
       else f=0;
       outbuf[f]='\0';           //example...
       sprintf(str,"\n\
From: %s (%s)\n  To: %s (%s)\nSubj: <your password>\n%s\n%%LIST\n+ALL\n%%QUERY\n",
       msg.FromUserName,adr1,msg.ToUserName,adr2,underline);
       strcat(outbuf,str);

       sprintf(str,"%s %s help",name,VER);
       sendmail(name,&to,msg.FromUserName,&reply,str,outbuf,0);
       break;


       case 'Q'://query
       case 'q':
       logputs(log," Cmd: %QUERY");
       readtpl(outbuf);
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;

       fi=0;
       g=0;
       f=0;
       area=first_area;
       while(area!=NULL)
       {
        g+=is_included_in_list(&reply,area->guest,&to);
        c=0;
        if(is_included_in_list(&reply,area->rw,&to))c=1;
        if(is_included_in_list(&reply,area->ro,&to))c=2;
        if(is_included_in_list(&reply,area->sleeping,&to))c=3;
        if(is_included_in_list(&reply,area->inactive,&to))c=4;

        if(c)
        {
         if(!fi)
         {
          getMSG("QUERY");
          sprintf(str,xMSG,msg.FromUserName,adr1,underline);
          strcat(outbuf,str);
         }
         if(c<3)
          strcat(outbuf,"*");
         else
         {
          strcat(outbuf,"+");
          f++;
         }
         sprintf(str,"%-16s",area->name);
         if(c==1) strcat(str,"Read/Write ");
         if(c==2) strcat(str,"Read Only  ");
         if(c==3) strcat(str,"Sleeping   ");
         if(c==4) strcat(str,"Inactive   ");
         strcat(outbuf,str);
         strcat(outbuf,area->desc);
         if(is_included_in_list(&reply,area->nomail,&to))strcat(outbuf," !");
         strcat(outbuf,"\n");
         fi++;
        }//enndif
        area=area->next;
       }//loop
       if(fi>0)strcat(outbuf,underline);
       getMSG("CONNECTED");
       sprintf(str,xMSG,fi,f);
       strcat(outbuf,str);

       f=is_included_in_list(&reply,friendaka,&to);
       goto send;

       case 'L'://list
       case 'l':
       logputs(log," Cmd: %LIST");
       readtpl(outbuf);
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;

       f=is_included_in_list(&reply,friendaka,&to);

       area=first_area;
       fi=0;
       g=0;
       while(area!=NULL)
       {
        g+=is_included_in_list(&reply,area->guest,&to);
        if(area->type!=255 || f || g)
        {
         if(!fi)
         {
          getMSG("LIST");
          sprintf(str,xMSG,msg.FromUserName,adr1,underline);
          strcat(outbuf,str);
         }//endif

         fi++;
         if(is_included_in_list(&reply,area->rw,&to) ||
            is_included_in_list(&reply,area->ro,&to) ||
            is_included_in_list(&reply,area->sleeping,&to) ||
            is_included_in_list(&reply,area->inactive,&to))
          strcat(outbuf,"*");
         else
          strcat(outbuf,"+");
         sprintf(str,"%-16s",area->name);
         if(area->type==1) strcat(str,"Free     ");
         if(area->type==2) strcat(str,"Echo     ");
         if(area->type==3) strcat(str,"Transit  ");
         if(area->type==255) strcat(str,"Private  ");
         strcat(outbuf,str);
         strcat(outbuf,area->desc);
         if(is_included_in_list(&reply,area->nomail,&to))strcat(outbuf," !");
         strcat(outbuf,"\n");
        }
        area=area->next;
       }//loop
       if(fi>0)strcat(outbuf,underline);
       getMSG("ACCESS");
       sprintf(str,xMSG,fi);
       strcat(outbuf,str);

       send:
       getMSG("FRIEND");
       if(f)strcat(outbuf,xMSG);
       else
       if(g>0)
       {
        getMSG("GUEST");
        sprintf(str,xMSG,g);
        strcat(outbuf,str);
       }//endif
       if(is_included_in_list(&reply,uplink,&to))
       {
        getMSG("UPLINK");
        sprintf(str,xMSG,adr2);
        strcat(outbuf,str);
       }//endif

       sendmail(name,&to,msg.FromUserName,&reply,msg.Subject,outbuf,0);
       break;


       case 'A'://active
       case 'a':
       logputs(log," Cmd: %ACTIVE");
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;
       area=first_area;
       fi=0;
       while(area!=NULL)
       {
        if(is_included_in_list(&reply,area->sleeping,&to))
        {
         AddLine(&(area->rw),adr1);
         del_from_list(&reply,&(area->ro),&to);
        }
        else if(is_included_in_list(&reply,area->inactive,&to))
        {
         AddLine(&(area->ro),adr1);
         del_from_list(&reply,&(area->rw),&to);
        }//endif
        if(is_included_in_list(&reply,area->rw,&to) ||
           is_included_in_list(&reply,area->ro,&to) ) fi++;
        del_from_list(&reply,&(area->inactive),&to);
        del_from_list(&reply,&(area->sleeping),&to);
        change=1;
        area=area->next;
       }//loop
       getMSG("ACTIVE");
       sprintf(str,xMSG,fi);
       AddLine(&report,str);
       break;


       case 'P'://passive
       case 'p':
       passive:
       logputs(log," Cmd: %PASSIVE");
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;
       area=first_area;
       fi=0;
       while(area!=NULL)
       {
        if(is_included_in_list(&reply,area->rw,&to))
        {
         AddLine(&(area->sleeping),adr1);
         del_from_list(&reply,&(area->inactive),&to);
        }
        else if(is_included_in_list(&reply,area->ro,&to))
        {
         AddLine(&(area->inactive),adr1);
         del_from_list(&reply,&(area->sleeping),&to);
        }//endif
        if(is_included_in_list(&reply,area->inactive,&to) ||
           is_included_in_list(&reply,area->sleeping,&to) ) fi++;
        del_from_list(&reply,&(area->rw),&to);
        del_from_list(&reply,&(area->ro),&to);
        change=1;
        area=area->next;
       }//loop
       getMSG("PASSIVE");
       sprintf(str,xMSG,fi);
       AddLine(&report,str);
       break;


       case '+'://+ALL
       connect:
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;
       if(!mailsw)logputs(log," Cmd: %+ALL");
       f=is_included_in_list(&reply,friendaka,&to);
       fi=0;
       c=0;
       area=first_area;
       while(area!=NULL)
       {
        if(mailsw)
        {
         del_from_list(&reply,&(area->nomail),&to);
         c++;
         change=1;
        }
        else
        {
         if(area->type!=255 || f || is_included_in_list(&reply,area->guest,&to))
         {
          del_from_list(&reply,&(area->rw),&to);
          del_from_list(&reply,&(area->ro),&to);
          if((area->type==1 ||
              is_included_in_list(&reply,uplink,&to) ||
              is_included_in_list(&reply,area->sleeping,&to)) &&
             !is_included_in_list(&reply,area->inactive,&to))
          {
           AddLine(&(area->rw),adr1);
           c++;
          }
          else
           AddLine(&(area->ro),adr1);
          del_from_list(&reply,&(area->sleeping),&to);
          del_from_list(&reply,&(area->inactive),&to);
          fi++;
          change=1;
         }
        }//endif
        area=area->next;
       }//loop
       if(mailsw)
       {
        getMSG("MAIL+ALL");
        sprintf(str,xMSG,c);
       }
       else
       {
        getMSG("ALL");
        sprintf(str,xMSG,fi,c);
       }
       AddLine(&report,str);
       mailsw=0;
       break;


       case '-': //-ALL
       disconnect:
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;
       if(!mailsw)logputs(log," Cmd: %-ALL");
       area=first_area;
       while(area!=NULL)
       {
        if(mailsw)
        {
         if(!is_included_in_list(&reply,area->nomail,&to))
          AddLine(&(area->nomail),adr1);
         change=1;
        }
        else
        {
         del_from_list(&reply,&(area->rw),&to);
         del_from_list(&reply,&(area->ro),&to);
         del_from_list(&reply,&(area->sleeping),&to);
         del_from_list(&reply,&(area->inactive),&to);
         change=1;
        }//endif
        area=area->next;
       }//loop
       if(mailsw)
        getMSG("MAIL-ALL");
       else
        getMSG("NONE");
       AddLine(&report,xMSG);
       mailsw=0;
       break;


       case 'M'://mail prefix
       case 'm':
       mailsw=1;
      }//end switch
      break;

      case '+':
      if(strncmpi(&buf[j+1],"ALL",3)==0) goto connect;
      case '\n': //add,activate
      area=first_area;
      f=is_included_in_list(&reply,friendaka,&to);
      while(area!=NULL)
      {
       fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
       if(fi!=1) goto out;
       if(strncmpi(&buf[j+1],area->name,strlen(area->name))==0)
       {
        if(mailsw)
        {
         del_from_list(&reply,&(area->nomail),&to);
         getMSG("MAIL+");
         sprintf(str,xMSG,area->name);
         AddLine(&report,str);
         change=1;
        }
        else
        {
         //oblast
         sprintf(str," Cmd: +%s",area->name);
         logputs(log,str);
         if(area->type!=255 || f || is_included_in_list(&reply,area->guest,&to))
         {
          del_from_list(&reply,&(area->rw),&to);
          del_from_list(&reply,&(area->ro),&to);
          if((area->type==1 ||
              is_included_in_list(&reply,uplink,&to) ||
              is_included_in_list(&reply,area->sleeping,&to)) &&
             !is_included_in_list(&reply,area->inactive,&to))
          {
           AddLine(&(area->rw),adr1);
           getMSG("RDWR");
           sprintf(str,xMSG,area->name);
           AddLine(&report,str);
          }
          else
          {
           AddLine(&(area->ro),adr1);
           getMSG("RDONLY");
           sprintf(str,xMSG,area->name);
           AddLine(&report,str);
          }
          del_from_list(&reply,&(area->sleeping),&to);
          del_from_list(&reply,&(area->inactive),&to);
          change=1;
         }
         else
         {
          getMSG("NOTCON");
          AddLine(&report,xMSG);
         }
         break;
        }
       }//endif
       area=area->next;
      }//loop
      mailsw=0;
      break;

      case '-': //disconnect
      if(strncmpi(&buf[j+1],"ALL",3)==0) goto disconnect;
      area=first_area;
      while(area!=NULL)
      {
       if(strncmpi(&buf[j+1],area->name,strlen(area->name))==0)
       {
        if(mailsw)
        {
         if(!is_included_in_list(&reply,area->nomail,&to))
          AddLine(&(area->nomail),adr1);
         getMSG("MAIL-");
         sprintf(str,xMSG,area->name);
         AddLine(&report,str);
         change=1;
        }
        else
        {
         //oblast
         fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
         if(fi!=1) goto out;
         sprintf(str," Cmd: -%s",area->name);
         logputs(log,str);
         del_from_list(&reply,&(area->rw),&to);
         del_from_list(&reply,&(area->ro),&to);
         del_from_list(&reply,&(area->sleeping),&to);
         del_from_list(&reply,&(area->inactive),&to);
         getMSG("DISCON");
         sprintf(str,xMSG,area->name);
         AddLine(&report,str);
         change=1;
        }//endif
       }//endif
       area=area->next;
      }//loop
      mailsw=0;
      break;

      case '*': //passive
      if(strncmpi(&buf[j+1],"ALL",3)==0) goto passive;
      fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
      if(fi!=1) goto out;
      area=first_area;
      while(area!=NULL)
      {
       if(strncmpi(&buf[j+1],area->name,strlen(area->name))==0)
       {
        fi=testuser(msg.FromUserName,&reply,&to,msg.Subject);
        if(fi!=1) goto out;
        sprintf(str," Cmd: *%s",area->name);
        logputs(log,str);
        if(is_included_in_list(&reply,area->rw,&to))
        {
         AddLine(&(area->sleeping),adr1);
         del_from_list(&reply,&(area->inactive),&to);
         fi++;
        }
        else if(is_included_in_list(&reply,area->ro,&to))
        {
         AddLine(&(area->inactive),adr1);
         del_from_list(&reply,&(area->sleeping),&to);
         fi++;
        }
        else break;
        del_from_list(&reply,&(area->rw),&to);
        del_from_list(&reply,&(area->ro),&to);
        getMSG("PASS1");
        sprintf(str,xMSG,area->name);
        AddLine(&report,str);
        change=1;
        break;
       }//endif
       area=area->next;
      }//loop
      break;
     }//endswitch
     j++;
    }//loop

    if(report!=NULL)// mail o zmenach..
    {
     struct radek *pom;

     readtpl(outbuf);
     pom=report;
     while(pom!=NULL)
     {
      strcat(outbuf,pom->str);
      report=pom;
      pom=pom->next;
      free(report);
     }//loop
     sendmail(name,&to,msg.FromUserName,&reply,msg.Subject,outbuf,0);
    }//endif
    out:
    unlink(pom);
    break;

    case 2:
    getMSG("ERRNETMAIL");
    sprintf(str,xMSG,pom);
    logputs(log,str);
    break;
    case 5:
    getMSG("JINEJFIX");
    sprintf(str,xMSG,to.net,to.node,to.point);
    logputs(log,str);
   }//end switch
   f=findnext(&ff);
  }//loop
  if(event)goto fix;
  break;

  case 'f': //Fix ----------------------------------------------------------
  case 'F':

  printf("\n");
  getMSG("FIX");
  logputs(log,xMSG);
  fix:
  mkdir(ticpath);
  strcat(ticpath,"\\");
  getMSG("INBOUND");
  puts(xMSG);
  sprintf(pom,"%s*.TIC",inbound);
  f=findfirst(pom,&ff,0);
  while(!f)
  {
   sprintf(pom,"%s%s",inbound,ff.ff_name);

   InitTic(tic);
   i=LoadTic(tic,pom);
   switch(i)
   {
    case 1: //ok
    area=first_area;
    while(area!=NULL)
    {
     if(strcmpi(tic->Area,area->name)==0)
     {
      a=my;
      while(a!=NULL)
      {
       str2fido(tic->To,&b,a);
       if(!strncmpi(ff.ff_name,"HATCH",5))goto processtic;
       if(memcmp(a,&b,8)==0)
       {
        //nalezeno spravne AKA
        str2fido(tic->From,&b,a); //b...komu ne
        if(!is_included_in_list(&b,area->rw,&to) &&
           !is_included_in_list(&b,area->sleeping,&to) ||
            testuser("Sysop",&b,a,tic->Pw)!=1)
        {
         getMSG("NOTALLOWED");
         sprintf(str,xMSG,tic->From,area->name);
         goto badtic;
        }
        processtic:
        sprintf(str,"%s%s",inbound,tic->File);
        if(!findfirst(str,&fff,0)) //soubor...
        {

         //!! Crc
         if(tic->Crc!=0)
          if(filecrc32(str)!=tic->Crc)
          {
           getMSG("CRC?");
           sprintf(str,xMSG,warning,tic->File);
           logputs(log,str);
           if(!strncmpi(pom,"CHFX",4))
           {
            getMSG("CRC!!");
            sprintf(str,xMSG,warning,tic->File);
            logputs(log,str);
            NotifySysop(my,pom);
           }//endif
          }//endif

         getpath(area,str);
         unlink(pom); //smazat tic
         strcpy(pom,str);
         pom[strlen(pom)-1]='\0';      
         mkdir(pom);
         getMSG("RCVD");
         sprintf(pom,xMSG,tic->File,tic->Area,tic->Origin,tic->From,tic->To);
         logputs(log,pom);
         if(area->exec[0])
         {
          getMSG("EXEC");
          sprintf(pom,xMSG,area->exec,inbound,tic->File);
          logputs(log,pom);
          sprintf(pom,"%s %s%s",area->exec,inbound,tic->File);
          system(pom);
         }

         //!! replace
         if(tic->Replaces[0])
         {
          if(is_included_in_list(&b,uplink,&to))
          {
           sprintf(pom,"%s%s",str,tic->Replaces);
           if(!findfirst(pom,&fff,0))
           {
            char maz[80];

            if(!strcmpi(fff.ff_name,area->filesBBS) || !strcmpi(fff.ff_name,area->filesBBS))
            {
             getMSG("CANNOT");
             sprintf(pom,xMSG,warning,fff.ff_name);
             goto illegalrepl;
            }

            strcpy(maz,str);
            strcat(maz,fff.ff_name);
            if(!findnext(&fff))
            {
             getMSG("REPLACES>1");
             sprintf(pom,xMSG,warning,tic->Replaces);
             illegalrepl:
             CreateTicName(file,"BADTICS\\");
             getMSG("SAVING");
             strcat(pom,xMSG);
             strcat(pom,file);
             logputs(log,pom);
             NotifySysop(my,pom);
             mkdir("BADTICS");
             SaveTic(tic,file);
            }
            else
             unlink(maz);
           }//endif
          }
          else
          {
           getMSG("NOTUPLINK");
           sprintf(pom,xMSG,warning,tic->From,tic->Replaces);
           goto illegalrepl;
          }
         }//endif

         sprintf(pom,"move %s%s %s%s>NUL",inbound,tic->File,str,tic->File);
         system(pom);

         DistributeFile(tic,str,area,a,&b); //b=komu ne

         //update files bbs
         if(area->type!=3) UpdateFilesBBS(tic,str,area);
        }
        else
        {
         getMSG("NOTFOUND");
         sprintf(str,xMSG,warning,tic->File,ff.ff_name);
         logputs(log,str);
        }
        goto nexttic;
       }//endif
       a=a->next;
      }//loop
      badaka:
      getMSG("BADAKA");
      sprintf(str,xMSG,ff.ff_name,tic->To);
      goto badtic;
     }//endif
    area=area->next;
    }//loop
    getMSG("BADAREA");
    sprintf(str,xMSG,tic->Area,ff.ff_name);
    str2fido(tic->From,&b,my);
    if(is_included_in_list(&b,uplink,my))
    {
     getMSG("CREATING");
     strcat(str,xMSG);
     NotifySysop(my,str);
     logputs(log,str);
     a=my;
     while(a!=NULL)
     {
      str2fido(tic->To,&b,a);
      if(memcmp(a,&b,8)==0)
      {
       char newdir[80]="\0";
       int i;
       // zalozeni nove oblasti:
       AllocArea();
       getMSG("AUTOAREA");
       strcpy(area->desc,xMSG);
       switch(newareatype)
       {
        case 'F':
        case 'f':
        area->type=1;
        break;
        case 'T':
        case 't':
        area->type=3;
        break;
        case 'P':
        case 'p':
        area->type=255;
        break;
        default:
        area->type=2;
       }//endswitch
       strupr(tic->Area);
       pomradek=privatestring;  //pvt oblast ?
       while(pomradek!=NULL)
       {
        if(strstr(tic->Area,strupr(pomradek->str)))
        {
         area->type=255; //pvt
         goto ook;
        }
        pomradek=pomradek->next;
       }//loop
       pomradek=transitstring;  //transit oblast ?
       while(pomradek!=NULL)
       {
        if(strstr(tic->Area,strupr(pomradek->str)))
        {
         area->type=3; //transit
         goto ook;
        }
        pomradek=pomradek->next;
       }//loop
       pomradek=echostring;  //echo oblast ?
       while(pomradek!=NULL)
       {
        if(strstr(tic->Area,strupr(pomradek->str)))
        {
         area->type=2; //echo
         goto ook;
        }
        pomradek=pomradek->next;
       }//loop
       pomradek=freestring;  //volna oblast ?
       while(pomradek!=NULL)
       {
        if(strstr(tic->Area,strupr(pomradek->str)))
        {
         area->type=1; //free
         goto ook;
        }
        pomradek=pomradek->next;
       }//loop
       ook:
       strcpy(area->name,tic->Area);
       if(tic->Areadesc[0])strcpy(area->desc,tic->Areadesc);
       if(newpath[strlen(newpath)-1]!='\\')strcat(newpath,"\\");  
       if(area->type!=3)
       {
        char pom[80];
        //adresar pro netransitni oblasti
        strcpy(area->path,newpath);
        if(newpath[0])
        {
         //zalozeni newareas cesty
         strcpy(pom,newpath);
         pom[strlen(pom)-1]='\0';
         mkdir(pom);
        }//endif
        strcpy(newdir,area->name);
        switch(createpath)
        {
         case 'a':
         case 'A':
         {
          char *ptr;
          newdir[13]='\0';
          ptr=strchr(newdir,'.');
          if(ptr)
          {
           if(strlen(ptr)>3 || ptr-newdir>8)
            memmove(ptr,&ptr[1],12);
           else
            goto jednatecka;
          }
          if(strlen(newdir)>8)
           newdir[8]='.';
          jednatecka:
          newdir[12]='\0';
         }
         strcat(area->path,newdir);
         break;
         case 'c':
         case 'C':
         {
          char *ptr;
          ptr=strstr(newdir,pomradek->str);
          if(ptr)
          {
           if(ptr==newdir)memmove(newdir,&newdir[strlen(pomradek->str)],
                                  strlen(newdir)-strlen(pomradek->str)+1);
           else
            *ptr='\0';
          }
         }//continue...
         case 't':
         case 'T':
         newdir[8]='\0';
         i=0;
         while(newdir[i])
         {
          if((newdir[i]<'A' || newdir[i]>'Z') &&
             (newdir[i]<'0' || newdir[i]>'9') &&
              !strchr("!-$#~",newdir[i]))
            newdir[i]='_';
          i++;
         }//loop
         strcat(area->path,newdir);
         if(createpath=='c' || createpath=='C')
         {
          struct Area *pa;
          i=0;
          pa=first_area;
          while(pa)
          {
           i++;
           if(!strcmpi(pa->path,area->path) && area!=pa)
           {
            char str[5];
            sprintf(str,".%d",i);
            strcat(area->path,str);
            break;
           }
           pa=pa->next;
          }//loop
         }//endif
        }//endswitch
        if(update_frq[0])logputs(update_frq,area->path);
       }//endif
       if(a!=my)fido2str(a,area->useAKA);
       AddLine(&(area->rw),tic->From);
       change=1;
       str2fido(tic->From,&b,a);
       goto processtic;
      }
      a=a->next;
     }
     goto badaka;
    }
    else
    {
     getMSG("A_UZEL");
     strcat(str,xMSG);
     strcat(str,tic->From);
     getMSG("NENI..");
     strcat(str,xMSG);
    }
    badtic:
    getMSG("BADTIC");
    strcat(str,xMSG);
    NotifySysop(my,str);
    logputs(log,str);
    mkdir("BADTICS");
    CreateTicName(file,"BADTICS\\");
    sprintf(str,"move %s %s>NUL",pom,file);
    system(str);
    break;

    case 2:
    getMSG("ERRTIC");
    sprintf(str,xMSG,pom);
    logputs(log,str);
   }//end switch

   nexttic:
   f=findnext(&ff);
  }//loop
  //proces transit files
  sprintf(pom,"%s*.*",transit);
  f=findfirst(pom,&ff,0);
  getMSG("TRANSIT");
  if(!f) puts(xMSG);
  while(!f)
  {
   sprintf(str,"%s*.TIC",ticpath);
   f=findfirst(str,&fff,0);
   while(!f)
   {
    InitTic(tic);
    sprintf(str,"%s%s",ticpath,fff.ff_name);
    if(LoadTic(tic,str)==1) if(!strcmpi(tic->File,ff.ff_name))goto keepfile;
    f=findnext(&fff);
   }//loop
   getMSG("DELETING");
   sprintf(pom,xMSG,ff.ff_name);
   logputs(log,pom);
   sprintf(pom,"%s%s",transit,ff.ff_name);
   unlink(pom);
   keepfile:
   f=findnext(&ff);
  }//loop
  break;

  case 'h': //Hatch --------------------------------------------------------
  case 'H':
  getMSG("HATCH");
  logputs(log,xMSG);
  InitTic(tic);
  getMSG("NAME");
  printf(xMSG);

  if(argc==2)
   gets(file);
  else
   strcpy(file,argv[2]);

  strupr(file);
  if(argc>2)puts(file);
  if(findfirst(file,&ff,0))
  {
   getMSG("FNOTFOUND");
   printf(xMSG,file);
   exit(2);
  }
  fnsplit(file,drv,pth,jm,ext);
  strcat(jm,ext);
  strcpy(tic->File,jm);
  getMSG("AREA");
  printf(xMSG);
  gets(tic->Area);
  strupr(tic->Area);

  area=first_area;
  while(area!=NULL)
  {
   if(!strcmpi(tic->Area,area->name))
   {
    getpath(area,str);
    goto conthatch;
   }
   area=area->next;
  }//loop
  getMSG("ANOTFOUND");
  printf(xMSG,tic->Area);
  exit(2);

  conthatch:
  getMSG("APATH");
  printf(xMSG,str);
  getMSG("ADESC");
  printf(xMSG,area->desc);
  getMSG("DESC");
  printf(xMSG);
  gets(tic->Desc);
  getMSG("RPLC");
  printf(xMSG);
  gets(tic->Replaces);

  getMSG("CRC");
  puts(xMSG);
  tic->Crc=filecrc32(file);
  sprintf(pom,"%s: Crc=%lX",file,tic->Crc);
  logputs(log,pom);
  if(area->useAKA[0])
   strcpy(tic->Origin,area->useAKA);
  else
   strcpy(tic->Origin,adress->str);
  strcpy(tic->Areadesc,area->desc);

  getMSG("EXDIZ?");
  printf(xMSG);
  i=getch();
  putch(i);

  if(i!='n' && i!='N') Diz2LDesc("HATCH",file,&(tic->LDesc));

  printf("\n");

  if(area->rw || area->ro)
  {
   getMSG("FIXNOW?");
   printf(xMSG);
   i=getch();
   putch(i);
   printf("\n");
  }
  else
  {
   getMSG("NONODES");
   printf(xMSG,area->name);
  }
  if(i=='n' || i=='N' || !area->rw && !area->ro)
  {
   stophatch:
   getMSG("HATCHTIC");
   sprintf(pom,xMSG,file,inbound);
   logputs(log,pom);
   SaveTic(tic,"HATCH.TIC");
   exit(0);
  }//endif

  sprintf(pom,"%s%s",str,tic->Replaces);
  if(!findfirst(pom,&fff,0))
  {
   char p[80];
   if(!findnext(&fff))
   {
    getMSG("MATCH>1!");
    sprintf(p,xMSG,warning,tic->Replaces);
    logputs(log,p);
   }

   getMSG("DELETE?");
   printf(xMSG,pom);
   i=getch();
   putch(i);
   printf("\n");
   if(i!='y' && i!='Y' && i!='a' && i!='A') goto nemazat;
   sprintf(p,"DEL %s",pom);
   system(p);
  }//endif
  nemazat:

  strcpy(pom,drv);
  strcat(pom,pth);
  if(strcmpi(pom,str)) //presun fajlu
  {
   char *c="move";

   getMSG("MOVEDEST?");
   printf(xMSG,file,str);
   i=getch();
   putch(i);
   printf("\n");
   if(i=='n' || i=='N')
   {
    getMSG("COPYDEST?");
    printf(xMSG,file,str);
    i=getch();
    putch(i);
    printf("\n");
    if(i=='n' || i=='N')
    {
     getMSG("NOTDEST!");
     puts(xMSG);
     goto stophatch;
    }
    c="copy";
   }//endif

   strcpy(pom,str);
   pom[strlen(pom)-1]='\0'; 
   mkdir(pom);

   sprintf(pom,"%s %s %s%s",c,file,str,tic->File);
   system(pom);
  }//endif
  mkdir(ticpath);
  strcat(ticpath,"\\");

  getMSG("HATCHING");
  sprintf(pom,xMSG,tic->File,tic->Desc);
  logputs(log,pom);
  str2fido(tic->Origin,&b,my);
  DistributeFile(tic,str,area,&b,&b); //b=komu ne

  if(!area->filesBBS[0])
   strcpy(pom,"FILES.BBS");
  else
   strcpy(pom,area->filesBBS);

  if(area->type!=3) //mimo transit
  {
   getMSG("FILESBBS?");
   printf(xMSG,str,pom);
   i=getch();
   putch(i);
   printf("\n");
   if(i!='n' && i!='N') UpdateFilesBBS(tic,str,area);
  }
  break;
  default:
  goto help;
 }//end switch
 if(change)SaveCtl();
 return 0;
}//end


struct Netmail
{
  char FromUserName[36];
  char ToUserName[36];
  char Subject[72];
  char DateTime[20];
  unsigned TimesRead;
  unsigned DestNode;
  unsigned OrigNode;
  unsigned Cost;
  unsigned OrigNet;
  unsigned DestNet;
  unsigned DestZone;
  unsigned OrigZone;
  unsigned DestPoint;
  unsigned OrigPoint;
  unsigned ReplyTo;
  unsigned AttributeWord;
  unsigned NextReply;
//  char * Msgchar;
//  char Attr2;
};

struct FidoAdr
{
 int zone,net,node,point;
 struct FidoAdr *next;
};

#define MSGSIZE 16000

int readmsg(char *msgname, //jmeno, napr 1.MSG
            struct FidoAdr *my,struct FidoAdr *to,struct FidoAdr *reply,
            //my = moje adresa, to = to pole v netmailu, reply = from pole
             struct Netmail *msg,char *buf, char *robotnames,int *len);
            //Netmail - hlavicka netmailu, buf = bufer pro text, max. MSGSIZE,
            //robotname string ve kterem se bude hledat to, napr "Allfix/Filemgr"

void logputs(char *log,char *str);
void fido2str(struct FidoAdr *fido, char *str);
void str2fido(char *str,struct FidoAdr *fido,struct FidoAdr *defaultadr);

extern char exepath[80];
void sendmail(char *fromuser,struct FidoAdr *from,char *touser,
              struct FidoAdr *to,char *subject,char *file,int mode);
//mode 0=send file as text / 1=attach file & send direct

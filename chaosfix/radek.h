struct radek
{
 char str[128];
 struct radek *next;
};
void killstrednik(char *buf,int i,int line);
void AddLine(struct radek **rad, char *str);
int LoadList(char *dir_frq, struct radek **ptr,char firstwordonly);
void memerr(void);

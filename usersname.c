#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <gdbm.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include <features.h>
#include "vacation.h"

struct passwd *pw;

main()
{
//        ALIAS *cur;
        
//  if (!(cur = malloc((u_int)sizeof(ALIAS))))
//    exit(1);
//  cur->name = pw->pw_name;
//  cur->next = names;
//  names = cur;
    printf("%s %s\n",pw->pw_name,pw->pw_gecos);
}

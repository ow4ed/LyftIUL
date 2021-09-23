#include <string.h>
#define myfgets(s, size, file){ fgets(s,size,file); s[strlen(s)-1]='\0';}


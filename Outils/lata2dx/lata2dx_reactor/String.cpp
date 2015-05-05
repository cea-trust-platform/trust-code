#include "String.h"


String* String::substring(int i0, int i1)
{
char* ss = new char(i1-i0+1);
for (int i=i0;i<i1;i++)
	ss[i]=s[i];

String* str=new String(ss);
delete ss;
return str;
}


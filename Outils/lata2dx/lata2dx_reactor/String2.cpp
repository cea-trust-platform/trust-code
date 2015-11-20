#include "String2.h"


String2* String2::substring(int i0, int i1)
{
char* ss = new char(i1-i0+1);
for (int i=i0;i<i1;i++)
	ss[i]=s[i];

String2* str=new String2(ss);
delete ss;
return str;
}


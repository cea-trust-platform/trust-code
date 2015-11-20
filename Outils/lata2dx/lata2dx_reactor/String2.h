
#ifndef String2_inclus
#define String2_inclus

#include <iostream>
#include <string.h>
using std::ostream;

class String2
{

public :
friend class StringTokenizer;
String2()
	{
	s = new char[1];
	s[0]='0';
	}



String2(char *ss)
	{
	//cout << s << endl;
	s = new char[strlen(ss)+1];
	strcpy(s,ss);
	}



String2(String2& ss)
	{//cout << "copie" << endl;
	s = new char[strlen(ss.s)+1];
	strcpy(s,ss.s);
	}
	
String2(String2* ss)
	{
	s = new char[strlen(ss->s)+1];
	strcpy(s,ss->s);
	}
	
~String2()
	{
	delete[] s;
	}
	
	
	
inline int length() { return strlen(s);}


inline char* toChar()	{ return s; }
inline int compare(const String2& ss) const { return strcmp(s,ss.s); }
inline int compare(const String2* ss) const { return strcmp(s,ss->s); }
inline int compare(const char * ss) const { return strcmp(s,ss); }
String2* substring(int i0, int i1) ;

inline friend ostream& operator<<(ostream&  os,const String2& c) ;

private :
char *s;


};

ostream& operator<<(ostream &  os,const String2& c) 
{ 
return os << c.s; 
}


#endif


#ifndef String_inclus
#define String_inclus

#include <iostream>
#include <string.h>
using std::ostream;

class String
{

public :
friend class StringTokenizer;
String()
	{
	s = new char[1];
	s[0]='0';
	}



String(char *ss)
	{
	//cout << s << endl;
	s = new char[strlen(ss)+1];
	strcpy(s,ss);
	}



String(String& ss)
	{//cout << "copie" << endl;
	s = new char[strlen(ss.s)+1];
	strcpy(s,ss.s);
	}
	
String(String* ss)
	{
	s = new char[strlen(ss->s)+1];
	strcpy(s,ss->s);
	}
	
~String()
	{
	delete[] s;
	}
	
	
	
inline int length() { return strlen(s);}


inline char* toChar()	{ return s; }
inline int compare(const String& ss) const { return strcmp(s,ss.s); }
inline int compare(const String* ss) const { return strcmp(s,ss->s); }
inline int compare(const char * ss) const { return strcmp(s,ss); }
String* substring(int i0, int i1) ;

inline friend ostream& operator<<(ostream&  os,const String& c) ;

private :
char *s;


};

ostream& operator<<(ostream &  os,const String& c) 
{ 
return os << c.s; 
}


#endif

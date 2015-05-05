#ifndef StringTokenizer_inclus
#define StringTokenizer_inclus

#include "String.h"


class StringTokenizer
{

public :

static const int NUMBER;
static const int STRING;
static const int EOS;

int type;

StringTokenizer();
StringTokenizer(String& );
StringTokenizer(String,String);
~StringTokenizer();

int nextToken(void);

inline double getNValue() { return nval; }
inline const String& getSValue() { return *sval; }



private :

double nval;
String* sval;
String* str;
char* token, *reste;
char* separateur;
};




#endif

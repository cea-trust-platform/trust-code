#ifndef StringTokenizer_inclus
#define StringTokenizer_inclus

#include "String2.h"


class StringTokenizer
{

public :

static const int NUMBER;
static const int STRING;
static const int EOS;

int type;

StringTokenizer();
StringTokenizer(String2& );
StringTokenizer(String2,String2);
~StringTokenizer();

int nextToken(void);

inline double getNValue() { return nval; }
inline const String2& getSValue() { return *sval; }



private :

double nval;
String2* sval;
String2* str;
char* token, *reste;
char* separateur;
};




#endif

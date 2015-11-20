#include "StringTokenizer.h"
#include <iostream>
#include "String2.h"
#include <sstream>
#include <string.h>
#include <stdlib.h>
using namespace std;

const int StringTokenizer::NUMBER=-2;
const int StringTokenizer::STRING=-3;
const int StringTokenizer::EOS=-1;

StringTokenizer::StringTokenizer()
{
//cerr << "bonjour" << endl;
sval =NULL;
str = new String2((char*)"0");
separateur=new char[strlen("+-*/()^<>")+1];
strcpy(separateur,"+-*/()^<>");
reste = str->s;
}

StringTokenizer::StringTokenizer(String2& s)
{
sval =NULL;
//cerr << "bonjour" << sval << endl;
str = new String2(s);
separateur=new char[strlen("+-*/()^<>")+1];
strcpy(separateur,"+-*/()^<>");
reste = str->s;
}

StringTokenizer::StringTokenizer(String2 s, String2 sep)
{
sval =NULL;
str = new String2(s);
separateur = new char[sep.length()];
strcpy(separateur,sep.s);
reste = str->s;
}

StringTokenizer::~StringTokenizer()
{
delete str;
delete[] separateur;
if ( sval != NULL) delete sval;
}


int StringTokenizer::nextToken(void)
{

char *tmp;	
tmp=strpbrk(reste,separateur);
if (tmp == NULL)
	{
	//cerr << "Fin ? " << reste << endl;
	if (reste[0] == '\0') 
		{
		//cerr << "Oui Fin !! " << endl;
		type = EOS;
//		return EOS;
		}
	else
		{
		//cerr << "Non pas Fin !!! "  << reste <<  endl;
		if ((reste[0] >= '0') && ( reste[0] <= '9') )
			{
			//cerr << "c'est un nb !!" << endl;
			type = NUMBER;
			stringstream stream;
			stream << reste << endl;
			stream >> nval;
			//cerr << "le nb est " << nval << endl;
			}
		else
			{
			//cerr << "c'est une chaine !!" << endl;
			type=STRING;
			if ( sval != NULL) delete sval;
			sval = new String2(reste);
			}
		while ((*reste++) != '\0') ;
		reste--;
		}
	}
else if (tmp==reste)
	{
	//cerr << tmp[0] << endl;
	reste++;
	type = tmp[0];
//	return tmp[0];
	}
else
	{
	char* tok = new char[tmp-reste+1];
	int j=0;
	for (int i=0;i<tmp-reste;i++)
		{
		if ( reste[i] != ' ' )	tok[j++] = reste[i];
		}
	tok[j] = '\0';
	//cerr << tok << endl;
	if ((tok[0] >= '0') && ( tok[0] <= '9') )
		{
		//cerr << "c'est un nb !!" << endl;
		type = NUMBER;
		stringstream stream;
		stream << tok << endl;
		stream >> nval;
		//cerr << "le nb est " << nval << endl;
		}
	else
		{
		//cerr << "c'est une chaine !!" << tok << endl;
		type=STRING;
		if ( sval != NULL) 
			{
			//cerr << " delete sval !! " << sval << endl;
			delete sval;
			}
		sval = new String2(tok);
		}
	reste=tmp;
	delete [] tok;
//	return type;
	}
//cerr << "reste = " << reste << endl;
	
return type;		
}


/*
int main()
{
//String2 s("23+34*12-13+COS ( 12 )* 2^3");
String2 s("2+3");
StringTokenizer tk(s);
cerr << StringTokenizer::NUMBER << endl;
cerr << StringTokenizer::EOS << endl;
cerr << StringTokenizer::STRING << endl;

while (tk.nextToken()!=StringTokenizer::EOS) 
	{
	if (tk.type == StringTokenizer::STRING)
		{
		cerr << "Chaine = " << tk.getSValue() << endl; 
		}
	else if (tk.type == StringTokenizer::NUMBER)
		{
		cerr << "Valeur = " << tk.getNValue() << endl; 
		}
	else 
		{
		cerr << "Operateur = " << (char) tk.type << endl; 
		}
	}
	

}
*/

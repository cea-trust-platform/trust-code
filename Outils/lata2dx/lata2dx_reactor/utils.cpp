#ifndef _UTILS_CPP_
#define _UTILS_CPP_

#include <sstream>
#include <assert.h>
#include <vector>
#include <string.h>
using namespace std;

/* Functions that help code developpement */
string IntToString(int i)
{
  stringstream s;
  s << i;
  return s.str();
}

char* c_string(const string& s)
{
  char *p = new char[s.length()+1];
  s.copy(p,string::npos);
  p[s.length()]=0; //add final character
  return p;
}

string extract_label(const string& component_label)
{
  string label;
  const int place = component_label.find_last_of("_");
  
  return label=component_label.substr(place+1);
}

string delete_label(const string& component_label)
{
  const int place = component_label.find_last_of("_");

  assert(place>0);
  return component_label.substr(0,place);
}

char* to_upper(const char* s)
{
  const unsigned int size = strlen(s)+1;
  char *ss = new char[size];

  for (unsigned int i=0; i<size; i++)
    ss[i]=toupper(s[i]);

  return ss;
}

char* to_lower(const char* s)
{
  const unsigned int size = strlen(s)+1;
  char *ss = new char[size];

  for (unsigned int i=0; i<size; i++)
    ss[i]=tolower(s[i]);

  return ss;
}

string& to_upper(string& s)
{
  const unsigned int size = s.length();

  for (unsigned int i=0; i<size; i++)
    s[i]=(char) toupper(s[i]);

  return s;
}

string& to_lower(string& s)
{
  const unsigned int size = s.length();

  for (unsigned int i=0; i<size; i++)
    s[i]=(char) tolower(s[i]);

  return s;
}

char next_character(istream& is)
{
  char character;

  is >> character;//to skip white spaces
  is.unget();//to rewind is

  return character;
}

double max(const vector<double>& vect)
{
  double max=0;
  for (unsigned int i=0; i<vect.size(); i++)
    max = (vect[i]>max) ? vect[i] : max;

  return max;
}

double min(const vector<double>& vect)
{
  double min=0;
  for (unsigned int i=0; i<vect.size(); i++)
    min = (vect[i]<min) ? vect[i] : min;

  return min;
}
#endif

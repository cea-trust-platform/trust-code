//////////////////////////////////////////////////////////////////////////////
//
// File:        Exceptions.cpp
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// Exceptions.cpp
// version 1.2 10/05/2010

#include <Exceptions.h>
#include <sstream>
#include <string.h>

using namespace std;

using namespace ICoCo;

WrongContext::WrongContext(const string& prob, const string& method, const string& precondition) :
  prob(prob), method(method), precondition(precondition) { }

const char *WrongContext::what() const throw()
{
  ostringstream s;
  s << "WrongContext in Problem instance of name: " << prob<< endl;
  s << " in method " << method << " : " << precondition << endl;
  return strdup(s.str().c_str());
}

WrongArgument::WrongArgument(const string& prob, const string& method, const string& arg, const string& condition) :
  prob(prob), method(method), arg(arg), condition(condition) { }

const char *WrongArgument::what() const throw()
{
  ostringstream s;
  s << "WrongContext in Problem instance of name: " << prob<< endl;
  s << " in method " << method << ", argument " << arg << " : " << condition << endl;
  return strdup(s.str().c_str());
}

NotImplemented::NotImplemented(const string& prob, const string& method) :
  prob(prob), method(method) { }

const char *NotImplemented::what() const throw()
{
  ostringstream s;
  s << "WrongContext in Problem instance of name: " << prob<< endl;
  s << ", method " << method << endl;
  return strdup(s.str().c_str());
}


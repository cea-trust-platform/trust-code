// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#include <ICoCoExceptions.hxx>
#include <sstream>
#include <string.h>  // strdup

namespace ICoCo
{
WrongContext::WrongContext(const std::string& aprob, const std::string& amethod, const std::string& aprecondition)
    : prob(aprob)
    , method(amethod)
    , precondition(aprecondition)
{
}

const char* WrongContext::what() const throw()
{
  std::ostringstream s;
  s << "WrongContext in Problem instance with name: " << prob << std::endl;
  s << " in method " << method << " : " << precondition << std::endl;
  return strdup(s.str().c_str());
}

WrongArgument::WrongArgument(const std::string& aprob, const std::string& amethod, const std::string& aarg,
                             const std::string& acondition)
    : prob(aprob)
    , method(amethod)
    , arg(aarg)
    , condition(acondition)
{
}

const char* WrongArgument::what() const throw()
{
  std::ostringstream s;
  s << "WrongArgument in Problem instance with name: " << prob << std::endl;
  s << " in method " << method << ", argument " << arg << " : " << condition << std::endl;
  return strdup(s.str().c_str());
}

NotImplemented::NotImplemented(const std::string& aprob, const std::string& amethod)
    : prob(aprob)
    , method(amethod)
{
}

const char* NotImplemented::what() const throw()
{
  std::ostringstream s;
  s << "NotImplemented in Problem instance with name: " << prob << std::endl;
  s << ", method " << method << std::endl;
  return strdup(s.str().c_str());
}

}  // end namespace ICoCo

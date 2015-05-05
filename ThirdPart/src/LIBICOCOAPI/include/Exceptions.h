//////////////////////////////////////////////////////////////////////////////
//
// File:        Exceptions.h
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// Exceptions.h
// version 1.2 10/05/2010

#include <exception>
#include <string>


#ifndef _Exceptions_included_
#define _Exceptions_included_

namespace ICoCo
{
class WrongContext : public std::exception
{
public:
  WrongContext(const std::string& prob, const std::string& method, const std::string& precondition);
  ~WrongContext() throw() { }
  virtual const char* what() const throw();
private:
  std::string prob;         // Problem in which exception occured
  std::string method;       // method in which exception occured
  std::string precondition; // precondition which was not met by the Problem state
};

class WrongArgument : public std::exception
{
public:
  WrongArgument(const std::string& prob, const std::string& method, const std::string& arg, const std::string& condition);
  ~WrongArgument() throw() { }
  virtual const char* what() const throw();
private:
  std::string prob;         // Problem in which exception occured
  std::string method;       // method in which exception occured
  std::string arg;          // argument for which exception occured
  std::string condition;    // condition which was not met by the argument
};

class NotImplemented : public std::exception
{
public:
  NotImplemented(const std::string& prob, const std::string& method);
  ~NotImplemented() throw() { }
  virtual const char* what() const throw();
private:
  std::string prob;         // Problem in which exception occured
  std::string method;       // method in which exception occured
};

}

#endif


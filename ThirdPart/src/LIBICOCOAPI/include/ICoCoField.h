//////////////////////////////////////////////////////////////////////////////
//
// File:        ICoCoField.h
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// ICoCoField.h
// version 1.2 10/05/2010

#ifndef _ICoCoField_included_
#define _ICoCoField_included_
#include <string>


namespace ICoCo
{

class Field
{
public:
  Field();
  virtual ~Field();
  void setName(const std::string& name);
  const std::string& getName() const;
  const char* getCharName() const;

private:
  std::string* _name;
};
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
// File:        ICoCoField.cpp
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// ICoCoField.cpp
// version 1.2 10/05/2010

#include <ICoCoField.h>
#include <string>

using namespace ICoCo;
using std::string;

Field::Field()
{
  _name=new string;
}

Field::~Field()
{
  delete _name;
}

void Field::setName(const string& name)
{
  *_name=name;
}

const string& Field::getName() const
{
  return *_name;
}

const char* Field::getCharName() const
{
  return _name->c_str();
}


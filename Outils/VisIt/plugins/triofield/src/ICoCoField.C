//////////////////////////////////////////////////////////////////////////////
//
// File:        ICoCoField.cpp
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/fauchet_162/fauchet_163/1
//
//////////////////////////////////////////////////////////////////////////////

#include <ICoCoField.h>
#include <string>
#include <stdlib.h>

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

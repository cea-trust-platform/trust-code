// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#include "ICoCoField.hxx"

#include <string>

namespace ICoCo
{
Field::Field()
{
  _name = new std::string;
}

Field::~Field()
{
  delete _name;
}

void Field::setName(const std::string& name)
{
  *_name = name;
}

const std::string& Field::getName() const
{
  return *_name;
}

const char* Field::getCharName() const
{
  return _name->c_str();
}

}  // end namespace ICoCo

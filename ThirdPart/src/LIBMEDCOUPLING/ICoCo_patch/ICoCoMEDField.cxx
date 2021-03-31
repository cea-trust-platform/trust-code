// Copyright (C) 2007-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ICoCoMEDField.hxx"
#include "MEDCouplingFieldDouble.hxx"

namespace ICoCo
{

  MEDField::MEDField() : _field(0) {}


  /*! Constructor directly attaching a MEDCouplingFieldDouble
    the object does not take the control the objects pointed by 
    \a field.
  */
    
  MEDField::MEDField(MEDCoupling::MEDCouplingFieldDouble *field):_field(field)
  {
    if(_field)
      _field->incrRef();
  }
 MEDField::MEDField(const MEDField& field):_field(field.getMCField())
  {
    if(_field)
      _field->incrRef();
  }

  MEDField::~MEDField()
  {
    if(_field)
      _field->decrRef();
  }


  MEDField& MEDField::operator=(const MEDField& field)
  {
    if (_field)
      _field->decrRef();

    _field=field.getMCField();
    if(_field)
      _field->incrRef();
    return *this;
  }

  MEDCoupling::MEDCouplingFieldDouble *MEDField::getMCField() const
  {
    return _field;
  }

  void MEDField::setMCField(MEDCoupling::MEDCouplingFieldDouble * f)
  {
    if(_field)
      _field->decrRef();
    _field = f;
    if(f)
      _field->incrRef();
  }

}

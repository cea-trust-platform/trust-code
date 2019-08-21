// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#ifndef ICoCoMEDIntField_included
#define ICoCoMEDIntField_included

#include "ICoCoField.hxx"

namespace MEDCoupling
{
class MEDCouplingFieldInt32;
}

namespace ICoCo
{
class MEDIntField : public ICoCo::Field
{
 public:
  MEDIntField();
  MEDIntField(MEDCoupling::MEDCouplingFieldInt32* field);
  MEDIntField(const MEDIntField& field);
  MEDIntField& operator=(const MEDIntField& field);
  virtual ~MEDIntField();
  MEDCoupling::MEDCouplingFieldInt32* getMCField() const;
  void setMCField(MEDCoupling::MEDCouplingFieldInt32* f);

 private:
  MEDCoupling::MEDCouplingFieldInt32* _field;
};
}  // namespace ICoCo

#endif

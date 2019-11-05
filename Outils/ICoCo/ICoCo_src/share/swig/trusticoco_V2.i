%module trusticoco_V2

%{
#define SWIG_FILE_WITH_INIT
#include <iostream>
#include "ICoCoTrioField.h"
#include "ProblemTrio.h"
#include "Exceptions.h"
%}

#ifdef MEDCOUPLING
%{
  #include "ICoCoMEDField.hxx"
%}
#endif 


#pragma SWIG nowarn=401

%include "typemaps.i"
%include "std_string.i"
%include "exception.i"
#ifdef MEDCOUPLING
   %include "MEDCoupling.i"
#else    // MEDCoupling already performs this:
   %include "std_vector.i"
   %template(VecString) std::vector<std::string>;
#endif

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  } 
}

// Map certain input parameters to output:
%apply bool &OUTPUT { bool& stop };
%apply bool &OUTPUT { bool& converged };
%apply bool &OUTPUT { bool& converged };

// Wrap TrioField:
%include ICoCoField.h
#ifdef MEDCOUPLING
%include "ICoCoMEDField.hxx"
#endif
%include ICoCoTrioField.h

#ifdef MEDCOUPLING
// What functions will be creating new object which memory is to be managed by Python:
%newobject ICoCo::ProblemTrio::getInputMEDFieldTemplate;
%newobject ICoCo::ProblemTrio::getOutputMEDField;

// [ABN] f*** SWIG: it doesn't let me rename a method based on its **full** signature, meaning
// I have to use %ignore and %extend ... :

//%rename(getInputMEDFieldTemplateAsMF) "getInputFieldTemplate(const std::string& name, MEDField& afield)"
//%rename(setInputMEDFieldAsMF)         "setInputField(const std::string& name, const MEDField& afield)"
//%rename(getOutputMEDFieldAsMF)        "getOutputField(const std::string& name, MEDField& afield)"

%extend ICoCo::ProblemTrio {

  //
  // Methods using TrioField
  //  
  void getInputFieldTemplate(const std::string& name, TrioField& afield) const
  {   self->getInputFieldTemplate(name, afield);    }
  
  void setInputField(const std::string& name, const TrioField& afield)
  {   self-> setInputField(name, afield); }
  
  void getOutputField(const std::string& name, TrioField& afield) const
  {   self-> getOutputField(name, afield); }

  //
  // "...MED...AsMF()" methods using MEDField
  //
  void getInputMEDFieldTemplateAsMF(const std::string& name, MEDField& afield) const
  {   self->getInputFieldTemplate(name, afield);    }
  
  void setInputMEDFieldAsMF(const std::string& name, const MEDField& afield)
  {   self-> setInputField(name, afield); }
  
  void getOutputMEDFieldAsMF(const std::string& name, MEDField& afield) const
  {   self-> getOutputField(name, afield); }


  //
  // Methods returning MEDCoupling objects:
  //
  MEDCoupling::MEDCouplingFieldDouble* getInputMEDFieldTemplate(const std::string& name) const
  {
    ICoCo::MEDField *f = new ICoCo::MEDField();
    self->getInputFieldTemplate(name, *f);
    return f->getField();
  }

  MEDCoupling::MEDCouplingFieldDouble* getOutputMEDField(const std::string& name) const
  {
    ICoCo::MEDField *f = new ICoCo::MEDField();
    self->getOutputField(name, *f);
    return f->getField();
  }
}
#endif

%ignore getInputFieldTemplate;
%ignore setInputField;
%ignore getOutputField;

%include "ProblemTrio.h"
%include "Problem.h"

%typemap(out) ICoCo::TrioField* {
  $result = $1;
 }

#ifdef MEDCOUPLING
// Typemap here. If put before, doesn't compile ... don't ask why??
%typemap(out) ICoCo::MEDField* {
  $result = $1;
}
#endif

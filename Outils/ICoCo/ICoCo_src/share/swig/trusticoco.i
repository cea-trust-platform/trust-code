%module trusticoco

// Include necessary files for C++ compilation:
%{
#define SWIG_FILE_WITH_INIT
#include "ICoCoTrioField.hxx"
#include "ProblemTrio.h"
#include "ICoCoExceptions.hxx"
%}

#ifdef MEDCOUPLING
%{
  #include "ICoCoMEDDoubleField.hxx"
%}
#endif 

// Deacrivate  SWIGWARN_TYPE_UNDEFINED_CLASS (because of Problem.h, which we actually don't want to SwIG)
#pragma SWIG nowarn=401

// Load usual SWIG typemaps for std::string, std::exception and others
%include "typemaps.i"
%include "std_string.i"
%include "exception.i"
#ifdef MEDCOUPLING
   // Load SWIG definitions for ICoCo::MEDDoubleField and ICoCo::MEDIntField
   %include "ICoCoMEDField.i"
#endif

// Turn vector of strings into Python list of strings
%include "std_vector.i"
%template(VecString) std::vector<std::string>;

// Propagate C++ exceptions as Python exceptions
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  } 
}

// Map boolean [out] parameters to Python output (i.e. left hand side of the equal sign).
// Used for computeTimeStep() and iterateTimeStep().
%apply bool &OUTPUT { bool& stop };
%apply bool &OUTPUT { bool& converged };

// Wrap TrioField!
%include ICoCoField.hxx
#ifdef MEDCOUPLING
  %include "ICoCoMEDDoubleField.hxx"
#endif
// Wrap MEDDoubleField and MEDIntField! Warning those are renamed into ICoCoMEDDoubleField and ICoCoMEDIntField
%include ICoCoTrioField.hxx

//
// Main part of the wrapping:
//
%include "ICoCoProblem.hxx"
%include "ProblemTrio.h"

// In Python, define extra functions: 
//   - getInputMEDFieldTemplate()
//   - getOutputMEDField()
//   - setInputMEDField()
// so that they behave like the old version of the ICoCo interface (the one using MEDCouplingFieldDouble return values)
// but relying internally on the new "*MEDDoubleField*" functions. 
// Rationale: users who already had wrapped the old ICoCo API in Python won't have to change their scripts.
//
#ifdef MEDCOUPLING
  // Functions that will be creating new object which memory is to be managed by Python:
  %newobject ICoCo::ProblemTrio::getInputMEDFieldTemplate;
  %newobject ICoCo::ProblemTrio::getOutputMEDField;
#endif

%extend ICoCo::ProblemTrio {

  MEDCoupling::MEDCouplingFieldDouble* getInputMEDFieldTemplate(const std::string& name) const
  {
    ICoCo::MEDDoubleField *f = new ICoCo::MEDDoubleField();
    self->getInputMEDDoubleFieldTemplate(name, *f);
    return f->getMCField();
  }

  void setInputMEDField(const std::string& name, MEDCoupling::MEDCouplingFieldDouble* field)
  {
    ICoCo::MEDDoubleField *f = new ICoCo::MEDDoubleField();
    f->setMCField(field);
    self->setInputMEDDoubleField(name, *f);
  }

  MEDCoupling::MEDCouplingFieldDouble* getOutputMEDField(const std::string& name) const
  {
    ICoCo::MEDDoubleField *f = new ICoCo::MEDDoubleField();
    self->getOutputMEDDoubleField(name, *f);
    return f->getMCField();
  }
}

// Renaming ValueType enum into ICoCoValueType because Python wrapping discards the namespace "ICoCo"
// Also handle the funny wrapping of SWIG with "_" (this does not happen in PyBind11)
%pythoncode {
class ICoCoValueType(object):
    Double = ValueType_Double
    Int = ValueType_Int
    String = ValueType_String
}

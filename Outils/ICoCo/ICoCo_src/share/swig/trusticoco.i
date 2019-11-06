%module trusticoco

%{
#define SWIG_FILE_WITH_INIT
#include <iostream>
#include "ICoCoTrioField.h"
#include "ProblemTrio.h"
#include "Exceptions.h"
%}

#if defined(MEDCOUPLING) && defined(OLD_MEDCOUPLING)
%{
   #include "ICoCoMEDField.hxx"
%}
#endif


#pragma SWIG nowarn=401

%include "typemaps.i"
%include "std_string.i"
%include "exception.i"

#if defined(MEDCOUPLING) && defined(OLD_MEDCOUPLING)
  %include "MEDCoupling.i"
#else
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

// Wrap MED and TrioField:
%include "ICoCoField.hxx"
#if defined(MEDCOUPLING) && defined(OLD_MEDCOUPLING)
%include "ICoCoMEDField.hxx"
#endif
%include "ICoCoTrioField.h"

// Wrap ProblemTrio:
// Certain functions are only supported in the old version of MEDCoupling:
#if defined(MEDCOUPLING) && defined(OLD_MEDCOUPLING)
%ignore setInputMEDField();
%ignore getInputMEDFieldTemplate();
%ignore getOutputMEDField();
#endif

%include "ProblemTrio.h"
%include "Problem.h"

%typemap(out) ICoCo::TrioField* {
  $result = $1;
 }

%extend ICoCo::ProblemTrio {
  ICoCo::TrioField * getOutputField(const std::string& name) {
    ICoCo::TrioField *f = new ICoCo::TrioField;
    self->getOutputField(name, *f);
    return f;
  }
 }

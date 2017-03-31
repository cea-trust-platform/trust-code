%module LataLoader

%include std_vector.i
%include std_string.i


%{
#include "LataLoader.h"
using namespace MEDCoupling;

%}

#if SWIG_VERSION >= 0x010329
%template()  std::vector<std::string>;
#endif


%include "MEDCoupling.i"
%include "LataLoader.h"



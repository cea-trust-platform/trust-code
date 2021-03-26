# ICoCo API - Version 2 (02/2021)

**WARNING: This API should not be changed!!**

ICoCo stands for **Interface for Code Coupling**. This is a norm that a code may choose
to implement to facilitate its coupling with other ICoCo-compliant codes.

Practically ICoCo is provided as a main abstract C++ class (ICoCo::Problem) that a code has
to derive to implement the norm. This can be only a partial implementation as some methods
are not relevant to all the codes (especially the I/O methods of the API).

The API rely on ICoCo::MEDDoubleField and ICoCo::MEDIntField for some of the field exchange
methods. An implementation for those is provided by the SALOME MEDCoupling library 
(version 9.7+)

The full documentation of the API can be generated with Doxygen using the provided 
file <code>doc/Doxyfile_icoco</code>.

Finally this API can be wrapped in Python: an example of a SWIG wrapping can be found in the
TRUST platform sources or in the swig subfolder for a dummy code named <code>your_code</code>.

/////////////////////////////////////////////////////////////////////////////
//
// File      : my_first_class.h
// Directory : basic        # name of the project    (required) basic        # name of the project    (required)/src
//
/////////////////////////////////////////////////////////////////////////////

#ifndef my_first_class_included
#define my_first_class_included

#include <Objet_U.h>
#include <Interprete_geometrique_base.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : my_first_class
// .HEADER      : basic        # name of the project    (required) basic        # name of the project    (required)/src
// .LIBRARY     : libbasic        # name of the project    (required)
// .FILE        : my_first_class.h
// .FILE        : my_first_class.cpp
// .DESCRIPTION : class my_first_class
//
// <Description of class my_first_class>
//
/////////////////////////////////////////////////////////////////////////////

class my_first_class : public Interprete_geometrique_base {

Declare_instanciable(my_first_class) ;

public :
virtual Entree& interpreter_(Entree&);
protected :

};

#endif /* my_first_class_included */

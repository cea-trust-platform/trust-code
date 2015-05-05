//////////////////////////////////////////////////////////////////////////////
//
// File:        Testeur2.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Testeur2_inclus
#define Testeur2_inclus

#include <Interprete.h>

//
// AJOUTER ICI 

class Testeur2 : public Interprete
{

  Declare_instanciable(Testeur2);

public :

  Entree& interpreter(Entree&);
 
};


#endif




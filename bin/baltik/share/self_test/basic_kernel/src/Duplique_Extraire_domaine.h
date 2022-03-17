//////////////////////////////////////////////////////////////////////////////
//
// File:        Duplique_Extraire_domaine.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////





#ifndef Duplique_Extraire_domaine_inclus
#define Duplique_Extraire_domaine_inclus

#include <Interprete.h>




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Duplique_Extraire_domaine
//    sans interet, mais teste si l atelier logiciel se comporte correctement
//////////////////////////////////////////////////////////////////////////////

class Duplique_Extraire_domaine : public Interprete
{
  Declare_instanciable(Duplique_Extraire_domaine);
public :
  Entree& interpreter(Entree&) override;
};

#endif

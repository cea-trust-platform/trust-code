//////////////////////////////////////////////////////////////////////////////
//
// File:        Duplique_Extraire_domaine.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Duplique_Extraire_domaine.h>


Implemente_instanciable(Duplique_Extraire_domaine,"Extraire_Domaine",Interprete);

Sortie& Duplique_Extraire_domaine::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}
Entree& Duplique_Extraire_domaine::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Duplique_Extraire_domaine::interpreter(Entree& is)
{

  return is;

}

////////////////////////////////////////////////////////////
//
// File:	Ecrire_fic_lml.h
// Directory:	$TRUST_ROOT/Geometrie/Decoupeur
//
////////////////////////////////////////////////////////////


#ifndef Ecrire_fic_lml_included
#define Ecrire_fic_lml_included

#include <Interprete.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Ecrire_fic_lml
//    Lecture d'un fichier
// .SECTION voir aussi
//    Interprete
//////////////////////////////////////////////////////////////////////////////
class Ecrire_fic_lml : public Interprete
{
  Declare_instanciable(Ecrire_fic_lml);
public :
  Entree& interpreter(Entree&) override;
protected:
  Nom toto; //Modif pour test_atelier
};
#endif

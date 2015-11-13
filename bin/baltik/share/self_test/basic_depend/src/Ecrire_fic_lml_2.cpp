////////////////////////////////////////////////////////////
//
// File:	Ecrire_fic_lml_2.cpp
// Directory:	$TRUST_ROOT/Geometrie/Decoupeur
//
////////////////////////////////////////////////////////////

#include <Ecrire_fic_lml_2.h>
#include <Domaine.h>
#include <EcrFicPartage.h>

Implemente_instanciable(Ecrire_fic_lml_2,"Ecrire_fic_lml_2",Interprete);

// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Ecrire_fic_lml_2::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Ecrire_fic_lml_2::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Ecrire_fic_lml_2::interpreter(Entree& is)
{
  toto="essai";
  Ecrire_fic_lml::interpreter(is);
  return is;
}



/****************************************************************************
* Copyright (c) 2015, CEA
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Entree_fluide_temperature_imposee_H.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Entree_fluide_temperature_imposee_H.h>
#include <Fluide_Quasi_Compressible.h>
#include <Equation_base.h>
#include <Motcle.h>

Implemente_instanciable(Entree_fluide_temperature_imposee_H,"Entree_temperature_imposee_H",Entree_fluide_temperature_imposee);

// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
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
Sortie& Entree_fluide_temperature_imposee_H::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Simple appel a: Cond_lim_base::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Entree_fluide_temperature_imposee_H::readOn(Entree& s)
{
  return Entree_fluide_temperature_imposee::readOn(s);
}

// Description:
//    Complete les conditions aux limites.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Entree_fluide_temperature_imposee_H::completer()
{
  le_fluide = ref_cast(Fluide_Quasi_Compressible,ma_zone_cl_dis->equation().milieu());
  modifier_val_imp = 1;
}

// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Entree_fluide_temperature_imposee sont compatibles
//    avec une equation dont le domaine est la Thermique
//    ou bien indetermine.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec l'equation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Entree_fluide_temperature_imposee_H::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Thermique="Thermique_H";
  if ( (dom_app==Thermique))
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

// Description:
//    Renvoie la valeur imposee sur la i-eme composante
//    du champ a la frontiere.
// Precondition:
// Parametre: int i
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes:
// Exception: deuxieme dimension du champ de frontiere superieur a 1
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Entree_fluide_temperature_imposee_H::val_imp(int i) const
{
  if (le_champ_front.valeurs().size()==1)
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_front(0,0));
      else
        return le_champ_front(0,0);

    }
  else if (le_champ_front.valeurs().dimension(1)==1)
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_front(i,0));
      else
        return le_champ_front(i,0);

    }
  else
    Cerr << "Temperature_imposee_paroi_H::val_imp erreur" << finl;

  abort();
  return 0.;
}


// Description:
//    Renvoie la valeur imposee sur la (i,j)-eme composante
//    du champ a la frontiere.
// Precondition:
// Parametre: int i
//    Signification: indice suivant la premiere dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int j
//    Signification: indice suivant la deuxieme dimension du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: double
//    Signification: la valeur imposee sur la composante du champ specifiee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Entree_fluide_temperature_imposee_H::val_imp(int i, int j) const
{
  if (le_champ_front.valeurs().dimension(0)==1)
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_front(0,j));
      else
        return le_champ_front(0,j);

    }
  else
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_front(i,j));
      else
        return le_champ_front(i,j);
    }
}

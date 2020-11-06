/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Scalaire_impose_paroi.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Scalaire_impose_paroi.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Discretisation_base.h>

Implemente_instanciable(Scalaire_impose_paroi,"Scalaire_impose_paroi",Dirichlet);
// XD scalaire_impose_paroi dirichlet scalaire_impose_paroi 0 Imposed temperature condition at the wall called bord (edge).
// XD   attr ch front_field_base ch 0 Boundary field type.

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
Sortie& Scalaire_impose_paroi::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Simple appel a: Dirichlet::readOn(Entree& )
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
Entree& Scalaire_impose_paroi::readOn(Entree& s )
{
  return Dirichlet::readOn(s) ;
}


// Description:
//    Verifie que les conditions aux limites sont
//    compatiblea avec la discretisation specifiees en parametre.
//    Des CL de type Scalaire_imposee_paroi sont compatibles
//    avec une discretisation de type VEF.
// Precondition:
// Parametre: Discretisation_base& discr
//    Signification: la discretisation avec laquelle il faut verifier la compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: valeur booleenne,
//                   1 si les CL sont compatibles avec la discretisation
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Scalaire_impose_paroi::compatible_avec_discr(const Discretisation_base& discr) const
{
  Nom type_discr=discr.que_suis_je();
  if (type_discr == "VEF")
    return 1;
  else if (type_discr == "EF")
    return 1;
  else if (type_discr == "EF_axi")
    return 1;
  else if (type_discr == "VEF_P1_P1")
    return 1;
  else if (type_discr == "VEFPreP1B")
    return 1;
  else if (type_discr == "VEFPreP1B_Front_Tracking")
    return 1;
  else if (type_discr == "PolyMAC")
    return 1;
  else if (type_discr == "CoviMAC")
    return 1;
  else
    {
      err_pas_compatible(discr);
      return 0;
    }
}


// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Scalaire_imposee_paroi sont compatibles
//    avec une equation dont le domaine est la Thermique, la Concentration
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
int Scalaire_impose_paroi::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Thermique="Thermique";
  Motcle Concentration="Concentration";
  Motcle indetermine="indetermine";

  if ( (dom_app==Thermique) || (dom_app==Concentration) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

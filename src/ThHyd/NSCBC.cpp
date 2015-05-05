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
// File:        NSCBC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <NSCBC.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Front_VF.h>

Implemente_instanciable(NSCBC,"NSCBC",Neumann);


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
Sortie& NSCBC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}


// Description:
//    Type le_champ_front en "Champ_front_uniforme".
//    Lit les valeurs du champ exterieur si les conditions
//    aux limites sont specifiees: "T_ext", "C_ext" ou "K_Eps_ext"
//    Produit une erreur sinon.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: type de champ exterieur non reconnu,
//            les types reconnus sont: "T_ext", "C_ext" ou "K_Eps_ext"
// Effets de bord:
// Postcondition:
Entree& NSCBC::readOn(Entree& s )
{
  return s;
}


void NSCBC::mettre_a_jour(double temps)
{
  Neumann::mettre_a_jour(temps);
  calculer_contribution_valeurs_propres(temps);
}

void NSCBC::calculer_contribution_valeurs_propres(double temps)
{
  Cerr<<"la methode NSCBC::calculer_contribution_valeurs_propres "<<finl;
  Cerr<<"n'est pas codee"<<finl;
  Cerr<<"on ne devrait pas passer par la classe NSCBC mais une de ses"<<finl;
  Cerr<<"classes filles : NSCBC_SG_parfaitement_absorbante, ..."<<finl;
  exit();
}


// Description:
//    Renvoie un booleen indiquant la compatibilite des conditions
//    aux limites avec l'equation specifiee en parametre.
//    Des CL de type Sortie_libre_pression_imposee sont compatibles
//    avec une equation dont le domaine est l'hydraulique (Navier_Stokes)
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
int NSCBC::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Hydraulique="Hydraulique";
  Motcle indetermine="indetermine";
  if ( (dom_app==Hydraulique) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

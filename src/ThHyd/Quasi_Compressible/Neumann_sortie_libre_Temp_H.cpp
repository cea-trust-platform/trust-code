/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Neumann_sortie_libre_Temp_H.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Neumann_sortie_libre_Temp_H.h>
#include <Fluide_Quasi_Compressible.h>
#include <Equation_base.h>
#include <Motcle.h>

Implemente_instanciable(Neumann_sortie_libre_Temp_H,"Sortie_libre_temperature_imposee_H",Neumann_sortie_libre);

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
Sortie& Neumann_sortie_libre_Temp_H::printOn(Sortie& s ) const
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
Entree& Neumann_sortie_libre_Temp_H::readOn(Entree& s )
{
  s >> le_champ_ext;

  le_champ_front = le_champ_ext;

  return s;
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
void Neumann_sortie_libre_Temp_H::completer()
{
  le_fluide = ref_cast(Fluide_Quasi_Compressible,ma_zone_cl_dis->equation().milieu());
  modifier_val_imp = 1;
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
int Neumann_sortie_libre_Temp_H::compatible_avec_eqn(const Equation_base& eqn) const
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
//    Renvoie la valeur de la i-eme composante
//    du champ impose a l'exterieur de la frontiere.
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
double Neumann_sortie_libre_Temp_H::val_ext(int i) const
{
  if (le_champ_ext.valeurs().size()==1)
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_ext(0,0));
      else
        return le_champ_ext(0,0);

    }
  else if (le_champ_ext.valeurs().dimension(1)==1)
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_ext(i,0));
      else
        return le_champ_ext(i,0);

    }
  else
    {
      Cerr << "Neumann_sortie_libre_Temp_H::val_ext" << finl;
      Cerr<<le_champ_ext<<finl;
    }

  abort();
  return 0.;
}


// Description:
//    Renvoie la valeur de la (i,j)-eme composante
//    du champ impose a l'exterieur de la frontiere.
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
double Neumann_sortie_libre_Temp_H::val_ext(int i,int j) const
{
  if (le_champ_ext.valeurs().dimension(0)==1)
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_ext(0,j));
      else
        return le_champ_ext(0,j);

    }
  else
    {
      if (modifier_val_imp==1)
        return le_fluide->calculer_H(le_champ_ext(i,j));
      else
        return le_champ_ext(i,j);
    }
}

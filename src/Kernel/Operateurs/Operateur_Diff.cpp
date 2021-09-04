/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Operateur_Diff.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Operateur_Diff.h>
#include <Operateur_Diff_base.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Discretisation_base.h>
#include <stat_counters.h>
#include <Champ_base.h>
Implemente_deriv(Operateur_Diff_base);
Implemente_instanciable(Operateur_Diff,"Operateur_Diff",DERIV(Operateur_Diff_base));


// Description:
//    Simple appel a Operateur::ecrire(Sortie&)
//    Ecrit l'operateur sur un flot de sortie.
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
Sortie& Operateur_Diff::printOn(Sortie& os) const
{
  return Operateur::ecrire(os);
}


// Description:
//    Simple appel a Operateur::lire(Entree&)
//    Lit l'operateur a partir d'un flot d'entree.
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
Entree& Operateur_Diff::readOn(Entree& is)
{
  return Operateur::lire(is);
}

// Description:
//    Type l'operateur:
//     se type "Op_Diff_"+discretisation()+
//     ("_"ou"_Multi_inco_")+("const_"ou"var_")
//     + inconnue().suffix
//    Associe la diffusivite a l'operateur base.
// Precondition: si l'operateur n'est pas negligeable une
//               diffusivite doit avoir ete associee a l'operateur.
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
// Postcondition: l'operateur est type
void Operateur_Diff::typer()
{
  Cerr << "Operateur_Diff::typer("<<typ<<")" << finl;
  if (Motcle(typ)==Motcle("negligeable"))
    DERIV(Operateur_Diff_base)::typer("Op_Diff_negligeable");
  else
    {
      assert(la_diffusivite.non_nul());
      Equation_base& eqn=mon_equation.valeur();
      Nom nom_type= eqn.discretisation().get_name_of_type_for(que_suis_je(),typ,eqn,diffusivite());
      DERIV(Operateur_Diff_base)::typer(nom_type);
      valeur().associer_diffusivite(diffusivite());
    }
  Cerr << valeur().que_suis_je() << finl;
}

// Description:
//    Renvoie l'objet sous-jacent upcaste en Operateur_base
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur_base&
//    Signification: l'objet sous-jacent upcaste en Operateur_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Operateur_base& Operateur_Diff::l_op_base()
{
  return valeur();
}
// Description:
//    Renvoie l'objet sous-jacent upcaste en Operateur_base
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur_base&
//    Signification: l'objet sous-jacent upcaste en Operateur_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Operateur_base& Operateur_Diff::l_op_base() const
{
  return valeur();
}

// Description:
//    Appel a l'objet sous-jacent.
//    Ajoute la contribution de l'operateur au tableau
//    passe en parametre
// Precondition:
// Parametre: DoubleTab& donnee
//    Signification: tableau contenant les donnees sur lesquelles on applique
//                   l'operateur.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: tableau auquel on ajoute la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau contenant le resultat
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Operateur_Diff::ajouter(const DoubleTab& donnee,
                                   DoubleTab& resu) const
{
  statistiques().begin_count(diffusion_counter_);
  DoubleTab& tmp = valeur().ajouter(donnee, resu);
  statistiques().end_count(diffusion_counter_);
  return tmp;
}

// Description:
//    Appel a l'objet sous-jacent.
//    Initialise le tableau passe en parametre avec la contribution
//    de l'operateur.
// Precondition:
// Parametre: DoubleTab& donnee
//    Signification: tableau contenant les donnees sur lesquelles on applique
//                   l'operateur.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: tableau dans lequel stocke la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes: l'ancien contenu est ecrase
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau contenant le resultat
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Operateur_Diff::calculer(const DoubleTab& donnee,
                                    DoubleTab& resu) const
{
  statistiques().begin_count(diffusion_counter_);
  DoubleTab& tmp = valeur().calculer(donnee, resu);
  statistiques().end_count(diffusion_counter_);
  return tmp;
}


// Description:
//    Renvoie le champ representant la diffusivite.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ representant la diffusivite
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_base& Operateur_Diff::diffusivite() const
{
  return la_diffusivite.valeur();
}


// Description:
//    Associe la diffusivite a l'operateur.
// Precondition:
// Parametre: Champ_Don& nu
//    Signification: le champ representant la diffusivite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_Don&
//    Signification: le champ representant la diffusivite
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Operateur_Diff::associer_diffusivite(const Champ_base& nu)
{
  la_diffusivite=nu;
}

void  Operateur_Diff::associer_diffusivite_pour_pas_de_temps(const Champ_base& nu)
{
  valeur().associer_diffusivite_pour_pas_de_temps(nu);
}

// Description:
//    Type l'operateur.
// Precondition:
// Parametre: Nom& typ
//    Signification: le nom representant le type de l'operateur
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Operateur_Diff::typer(const Nom& un_type)
{
  DERIV(Operateur_Diff_base)::typer(un_type);
}

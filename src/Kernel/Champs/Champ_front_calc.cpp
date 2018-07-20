/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Champ_front_calc.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_calc.h>
#include <Interprete.h>
#include <Equation_base.h>
#include <Probleme_Couple.h>
#include <Frontiere_dis_base.h>
#include <Probleme_base.h>
#include <Raccord_distant_homogene.h>
#include <Domaine.h>
#include <Debog.h>
#include <Zone_VF.h>

Implemente_instanciable(Champ_front_calc,"Champ_front_calc",Ch_front_var_instationnaire_dep);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_front_calc::printOn(Sortie& os) const
{
  return os;
}

// Description:
//    Lit le nom d'un probleme, le nom d'un bord et
//    le nom d'un champ inconnue a partir d'un flot d'entree.
//    Cree ensuite le champ de frontiere correspondant.
//    Format:
//      Champ_front_calc nom_probleme nom_bord nom_champ
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entre
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_front_calc::readOn(Entree& is)
{
  Motcle nom_inco;
  is >> nom_autre_pb_ >> nom_autre_bord_ >> nom_inco;
  creer(nom_autre_pb_, nom_autre_bord_, nom_inco);
  return is;
}

// Description:
//    Cree l'objet Champ_front_calc representant la trace d'un champ
//    inconnue sur une frontiere a partir des noms:
//         - du probleme portant l'inconnue
//         - du bord concerne (la frontiere)
//         - de l'inconnue
// Precondition:
// Parametre: Nom& nom_pb
//    Signification: le nom du probleme auquel appartent l'inconnue dont
//                   on veut prendre la trace
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Nom& nom
//    Signification: le nom de la frontiere sur laquelle on prend la
//                   trace de l'inconnue
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Motcle& nom_inco
//    Signification: le nom de l'inconnue dont on veut prendre la trace
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: pas de probleme du nom specifie
// Exception: le probleme n'a pas de champ du nom specifie
// Effets de bord:
// Postcondition:
void Champ_front_calc::creer(const Nom& nom_pb,
                             const Nom& nom_bord,
                             const Motcle& nom_inco)
{
  distant_=1;
  nom_autre_pb_ = nom_pb;
  nom_autre_bord_ = nom_bord;
  REF(Probleme_base) autre_pb;
  Objet_U& ob=Interprete::objet(nom_autre_pb_);
  if(sub_type(Probleme_base, ob))
    {
      autre_pb = ref_cast(Probleme_base, ob);
    }
  else
    {
      Cerr << "We did not find problem with name " << nom_pb << finl;
      exit();
    }
  REF(Champ_base) rch;
  rch = autre_pb->get_champ(nom_inco);
  if (sub_type(Champ_Inc_base,rch.valeur()))
    {
      l_inconnue=ref_cast(Champ_Inc_base, rch.valeur()) ;
      fixer_nb_comp(rch.valeur().nb_comp());
    }
  else
    {
      Cerr << autre_pb->le_nom() << " did not have unknown field with name " << nom_inco << finl;
      exit();
    }
}

void Champ_front_calc::completer()
{
  // Check/initialize Raccord boundaries in parallel:
  if (nproc()>1)
    {
      const Zone_dis_base& zone_dis_opposee = front_dis().zone_dis();
      const Zone_dis_base& zone_dis_locale = frontiere_dis().zone_dis();
      const Frontiere& frontiere_opposee = front_dis().frontiere();
      const Frontiere& frontiere_locale = frontiere_dis().frontiere();
      if (!sub_type(Raccord_distant_homogene, frontiere_opposee))
        {
          const Nom& nom_domaine_oppose = zone_dis_opposee.zone().domaine().le_nom();
          Cerr << "Error, the boundary " << frontiere_opposee.le_nom() << " should be a Raccord." << finl;
          Cerr << "Add in your data file between the definition and the partition of the domain " << nom_domaine_oppose << " :" << finl;
          Cerr << "Modif_bord_to_raccord " << nom_domaine_oppose << " " << frontiere_opposee.le_nom() << finl;
          exit();
        }
      Raccord_distant_homogene& raccord_distant = ref_cast_non_const(Raccord_distant_homogene, frontiere_opposee);
      if (distant_==1)
        {
          if (!raccord_distant.est_initialise())
            raccord_distant.initialise(frontiere_locale, zone_dis_locale, zone_dis_opposee);
        }
    }
}

int Champ_front_calc::initialiser(double temps, const Champ_Inc_base& inco)
{
  Ch_front_var_instationnaire_dep::initialiser(temps, inco);
  return 1;
}

// Description:
//    Associe le champ inconnue a l'objet
// Precondition:
// Parametre: Champ_Inc_base& inco
//    Signification: le champ inconnue dont on prendra la trace
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'objet a un champ inconnue associe
void Champ_front_calc::associer_ch_inc_base(const Champ_Inc_base& inco)
{
  l_inconnue = inco;
}


// Description:
//    Non code
// Precondition:
// Parametre: Champ_front_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_front_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_front_base& Champ_front_calc::affecter_(const Champ_front_base& ch)
{
  return *this;
}

// Description:
//    Mise a jour en temps du champ
//    On prend juste la trace du champ inconnue au pas de
//    temps auquel il se situe.
//    WEC : verifier qu'on prend l'inconnue au bon temps !

// Precondition: le nom du bord doit etre different de "??"
// Parametre: double
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
void Champ_front_calc::mettre_a_jour(double temps)
{
  assert (nom_autre_bord_ != "??") ;
  DoubleTab& tab=valeurs_au_temps(temps);
  const Frontiere_dis_base& frontiere_dis_opposee = zone_dis().frontiere_dis(nom_bord_oppose());
  l_inconnue->trace(frontiere_dis_opposee,tab,temps,distant_ /* distant */);


}

// Description:
//    Renvoie le champ inconnue associe
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc_base&
//    Signification: le champ inconnue associe
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Inc_base& Champ_front_calc::inconnue() const
{
  return l_inconnue.valeur();
}

// Description:
//    Renvoie le nom du bord sur lequel on calcule la trace.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du bord sur lequel on calcule la trace
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Champ_front_calc::nom_bord_oppose() const
{
  return nom_autre_bord_;
}

// Description:
//    Renvoie l'equation associee a l'inconnue dont on prend
//    la trace.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation associee a l'inconnue dont on prend
//                   la trace
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Equation_base& Champ_front_calc::equation() const
{
  if (l_inconnue.non_nul()==0)
    {
      Cerr << "\nError in Champ_front_calc::equation() : not able to return the equation !" << finl;
      Process::exit();
    }
  return inconnue().equation();
}

// Description:
//    Renvoie le milieu associe a l'equation qui porte
//    le champ inconnue dont on prend la trace.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le milieu associe a l'equation qui porte
//                    le champ inconnue dont on prend la trace
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Champ_front_calc::milieu() const
{
  return equation().milieu();
}

// Description:
//    Renvoie la zone discretisee associee a l'equation
//    qui porte le champ inconnue dont on prend la trace.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_dis_base&
//    Signification: la zone discretisee associee a l'equation
//                   qui porte le champ inconnue dont on prend la trace
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Zone_dis_base& Champ_front_calc::zone_dis() const
{
  return inconnue().zone_dis_base();
}

// Description:
//    Renvoie la zone des conditions au limites discretisees
//    portee par l'equation qui porte le champ inconnue
//    dont on prend la trace
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_Cl_dis_base&
//    Signification: la zone des conditions au limites discretisees
//                   portee par l'equation qui porte le champ inconnue
//                   dont on prend la trace
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Zone_Cl_dis_base& Champ_front_calc::zone_Cl_dis() const
{
  return equation().zone_Cl_dis().valeur();
}

// Description:
//    Renvoie la frontiere discretisee correspondante
//    au domaine sur lequel prend la trace.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: frontiere discretisee correspondante
//                   au domaine sur lequel prend la trace
//    Contraintes: reference constante
// Exception: frontiere du nom specifie introuvable
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Frontiere_dis_base& Champ_front_calc::front_dis() const
{
  return zone_dis().frontiere_dis(nom_autre_bord_);
}


void Champ_front_calc::verifier(const Cond_lim_base& la_cl) const
{
}

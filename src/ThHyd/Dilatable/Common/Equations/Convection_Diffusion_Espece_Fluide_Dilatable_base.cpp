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
// File:        Convection_Diffusion_Espece_Fluide_Dilatable_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Espece_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <EcritureLectureSpecial.h>
#include <Neumann_sortie_libre.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Avanc.h>

Implemente_base(Convection_Diffusion_Espece_Fluide_Dilatable_base,"Convection_Diffusion_Espece_Fluide_Dilatable_base",Convection_Diffusion_std);

Sortie& Convection_Diffusion_Espece_Fluide_Dilatable_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

Entree& Convection_Diffusion_Espece_Fluide_Dilatable_base::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul() && le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);
  return is;
}

void Convection_Diffusion_Espece_Fluide_Dilatable_base::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
}

int Convection_Diffusion_Espece_Fluide_Dilatable_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Fluide_Dilatable_base
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Fluide_Quasi_Compressible"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: les proprietes physiques du fluide ne sont pas toutes specifiees
// Effets de bord:
// Postcondition:
void Convection_Diffusion_Espece_Fluide_Dilatable_base::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_Dilatable_base& un_fluideQC = ref_cast(Fluide_Dilatable_base,un_milieu);
  associer_fluide(un_fluideQC);
}

// Description:
//    Associe un fluide de type Fluide_Dilatable_base a l'equation.
// Precondition:
// Parametre: Fluide_Dilatable_base& un_fluide
//    Signification: le milieu a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu associe
void Convection_Diffusion_Espece_Fluide_Dilatable_base::associer_fluide(const Fluide_Dilatable_base& un_fluide)
{
  le_fluide = un_fluide;
}

const Champ_Don& Convection_Diffusion_Espece_Fluide_Dilatable_base::diffusivite_pour_transport()
{
  return le_fluide->mu_sur_Schmidt();  // rho * D
}

// Description:
//    Tout ce qui ne depend pas des autres problemes eventuels.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Espece_Fluide_Dilatable_base::preparer_calcul()
{
  Convection_Diffusion_std::preparer_calcul();

  // remplissage de la zone cl modifiee avec 1 partout au bord...
  zcl_modif_=(zone_Cl_dis());

  Conds_lim& condlims=zcl_modif_.valeur().les_conditions_limites();
  int nb=condlims.size();
  for (int i=0; i<nb; i++)
    {
      // pour chaque condlim on recupere le champ_front et on met 1
      // meme si la cond lim est un flux (dans ce cas la convection restera nullle.)
      DoubleTab& T=condlims[i].valeur().champ_front().valeurs();
      T=1.;
      if (sub_type(Neumann_sortie_libre,condlims[i].valeur()))
        ref_cast(Neumann_sortie_libre,condlims[i].valeur()).tab_ext()=1;
    }
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);

  return 1;
}

// Description:
//    Associe l inconnue de l equation a la loi d etat,
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
void Convection_Diffusion_Espece_Fluide_Dilatable_base::completer()
{
  Convection_Diffusion_std::completer();
}

// Description:
//    Discretise l'equation.
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
// Postcondition: l'equation est discretisee
void Convection_Diffusion_Espece_Fluide_Dilatable_base::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Massic fraction equation discretization ..." << finl;
  //On utilise temperature pour la directive car discretisation identique
  dis.discretiser_champ("temperature",zone_dis(),"fraction_massique","sans_dimension",
                        1 /* nb_composantes */,nb_valeurs_temp,temps,l_inco_ch);
  Convection_Diffusion_std::discretiser();
  Cerr << "Convection_Diffusion_Espece_Fluide_Dilatable_base::discretiser() ok" << finl;
}

// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Dilatable_base upcaste en Milieu_base)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Dilatable_base upcaste en Milieu_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Convection_Diffusion_Espece_Fluide_Dilatable_base::milieu() const
{
  return fluide();
}

// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Dilatable_base upcaste en Milieu_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Dilatable_base upcaste en Milieu_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Convection_Diffusion_Espece_Fluide_Dilatable_base::milieu()
{
  return fluide();
}

// Description:
//    Renvoie le fluide dilatable associe a l'equation.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Fluide_Dilatable_base&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes: reference constante
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Fluide_Dilatable_base& Convection_Diffusion_Espece_Fluide_Dilatable_base::fluide() const
{
  if (!le_fluide.non_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}

// Description:
//    Renvoie le fluide dilatable associe a l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Fluide_Dilatable_base&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes:
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition:
Fluide_Dilatable_base& Convection_Diffusion_Espece_Fluide_Dilatable_base::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
}

int Convection_Diffusion_Espece_Fluide_Dilatable_base::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Equation_base::sauvegarder(os);
  // en mode ecriture special seul le maitre ecrit
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  return bytes;
}

// Description:
//     Effectue une reprise a partir d'un flot d'entree.
//     Appelle Equation_base::reprendre()
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: la reprise a echoue
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Espece_Fluide_Dilatable_base::reprendre(Entree& is)
{
  Convection_Diffusion_std::reprendre(is);
  return 1;
}

// Description:
//    Impression des flux sur les bords sur un flot de sortie.
//    Appelle Equation_base::impr(Sortie&)
// Precondition: Sortie&
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Convection_Diffusion_Espece_Fluide_Dilatable_base::impr(Sortie& os) const
{
  return Convection_Diffusion_std::impr(os);
}

// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Thermique".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Thermique"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Convection_Diffusion_Espece_Fluide_Dilatable_base::domaine_application() const
{
  static Motcle domaine ="Fraction_massique";
  return domaine;
}

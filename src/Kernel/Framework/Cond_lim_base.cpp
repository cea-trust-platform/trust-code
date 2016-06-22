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
// File:        Cond_lim_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Cond_lim_base.h>
#include <Equation_base.h>
#include <Discretisation_base.h>

Implemente_base(Cond_lim_base,"Cond_lim_base",Objet_U);



// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime la condition au limite sur un flot de sortie.
//    Seul le champ impose a la frontiere est imprime
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
Sortie& Cond_lim_base::printOn(Sortie& s ) const
{
  return s << le_champ_front ;
}


// Description:
//    Lecture d'une condition aux limites a partir d'un flot d'entree
//    Format:
//      lire un Champ_front
// Precondition:
// Parametre: Entree& s
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
Entree& Cond_lim_base::readOn(Entree& s )
{
  return s >> le_champ_front ;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
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
void Cond_lim_base::completer()
{
  champ_front()->completer();
}

// Description:
//    Change le i-eme temps futur de la CL.
void Cond_lim_base::changer_temps_futur(double temps,int i)
{
  champ_front()->changer_temps_futur(temps,i);
}

// Description:
//    Tourne la roue de la CL
int Cond_lim_base::avancer(double temps)
{
  return champ_front()->avancer(temps);
}

// Description:
//    Tourne la roue de la CL
int Cond_lim_base::reculer(double temps)
{
  return champ_front()->reculer(temps);
}

// Description:
//    Initialisation en debut de calcul.
//    A appeler avant tout calculer_coeffs_echange ou mettre_a_jour
//    Contrairementaux methodes mettre_a_jour, les methodes
//    initialiser des CLs ne peuvent pas dependre de l'exterieur
//    (lui-meme peut ne pas etre initialise)
// Precondition: appel a completer sur cette condition et les objets
// auxquels elle pourrait etre liee.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: 0 en cas d'erreur, 1 sinon.
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Cond_lim_base::initialiser(double temps)
{
  return le_champ_front->initialiser(temps,zone_Cl_dis().inconnue());
}


// Description:
//    Effectue une mise a jour en temps de la condition aux limites.
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Cond_lim_base::mettre_a_jour(double temps)
{
  le_champ_front.mettre_a_jour(temps);
}


// Description:
//    Cette methode indique si cette condition aux limites doit
//  etre mise a jour dans des sous pas de temps d'un schema en temps tel que RK
//  Par defaut elle renvoie 0 pour indiquer qu'aucune mise a jour
//  n'est necessaire ; il faut la surcharger pour renvoyer 1 au besoin
//  (exemple Echange_impose_base)
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Cond_lim_base::a_mettre_a_jour_ss_pas_dt()
{
  return 0;
}


// Description:
//    Calcul des coefficient d'echange pour un couplage par
//    Champ_front_contact_VEF.
//    Ces calculs sont locaux au probleme et dependant uniquement de
//    l'inconnue. Ils doivent donc etre faits chaque fois que
//    l'inconnue est modifiee. Ils sont disponibles pour l'exterieur et
//    stockes dans les CLs.
//    WEC : Les Champ_front_contact_VEF devraient disparaitre et cette
//    methode avec !!!
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Cond_lim_base::calculer_coeffs_echange(double temps)
{
  le_champ_front.calculer_coeffs_echange(temps);
}


// Description:
// Appel la verification du champ lu par l intermediaire
// de l equation pour laquelle on considere la condition limite
// La methode est surchargee dans les cas ou l utilisateur doit
// specifier le champ frontiere
// Precondition:
// Parametre:
// Exception:
// Effets de bord:
// Postcondition:
void Cond_lim_base::verifie_ch_init_nb_comp()
{

}

// Description:
//    Associe la frontiere a l'objet.
//    L'objet Frontiere_dis_base est en fait associe au membre
//    Champ_front de l'objet Cond_lim_base qui represente le champ des conditions
//    aux limites imposees a la frontiere.
// Precondition: le membre Cond_lim_base::le_champ_front doit etre non nul
// Parametre: Frontiere_dis_base& fr
//    Signification: la frontiere sur laquelle on impose les conditions aux limites
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Cond_lim_base::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  assert(le_champ_front.non_nul());
  le_champ_front.associer_fr_dis_base(fr);
  modifier_val_imp = 0;
}


// Description:
//    Associe la Zone_Cl_dis_base (Zone des conditions aux limites discretisees)
//    a l'objet. Cette Zone_Cl_dis_base stocke (reference) toutes les conditions
//    aux limites relative a une Zone geometrique.
// Precondition:
// Parametre: Zone_Cl_dis_base& zcl
//    Signification: une Zone des conditions aux limites discretisees a laquelle
//                   l'objet Cond_lim_base se rapporte
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Cond_lim_base::associer_zone_cl_dis_base(const Zone_Cl_dis_base& zcl)
{
  ma_zone_cl_dis=zcl;
  le_champ_front.valeur().verifier(*this);
}


// Description:
//    Renvoie 1 si la condition aux limites est compatible
//    avec la discretisation passee en parametre.
//    NE FAIT RIEN
//    A Surcharger dans les classes derivees
// Precondition:
// Parametre: Discretisation_base&
//    Signification: la discretisation avec laquelle on veut verifier la
//                   compatibilite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Cond_lim_base::compatible_avec_discr(const Discretisation_base& ) const
{
  return 1;
}


// Description:
//    Cette methode est appelee quand la condition aux limites
//    n'est pas compatible avec l'equation sur laquelle on essaye
//    de l'appliquer.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation avec laquelle la condition aux limites est incompatible
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Cond_lim_base::err_pas_compatible(const Equation_base& eqn) const
{
  Cerr << "The boundary condition " << que_suis_je() << " can't apply to " << finl
       << "the equation of kind " << eqn.que_suis_je()
       << finl;
  exit();
}


// Description:
//    Cette methode est appelee quand la condition aux limites
//    n'est pas compatible avec la discretisation sur laquelle on essaye
//    de l'appliquer.
// Precondition:
// Parametre: Discretisation_base& discr
//    Signification:  la discretisation avec laquelle la condition aux limites est
//                    incompatible
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Cond_lim_base::err_pas_compatible(const Discretisation_base& discr) const
{
  Cerr << "The boundary condition " << que_suis_je() << " can't be used with " << finl
       << "the discretization of kind " << discr.que_suis_je()
       << finl;
  exit();
}

void Cond_lim_base::champ_front(int face, DoubleVect& var) const
{
  le_champ_front->valeurs_face(face,var);
}

void Cond_lim_base::injecter_dans_champ_inc(const Champ_Inc& ) const
{
  Cerr << "Cond_lim_base::injecter_dans_champ_inc()" << finl;
  Cerr << "this method does nothing and must be overloaded " << finl;
  Cerr << "Contact TRUST support." << finl;
  exit();
}
// Description:
//    Change le i-eme temps futur de la cl.
void Cond_lim_base::set_temps_defaut(double temps)
{
  champ_front()->set_temps_defaut(temps);
}
// Description:
//     Appele par Conds_lim::completer
//   Appel cha_front_base::fixer_nb_valeurs_temporelles
void Cond_lim_base::fixer_nb_valeurs_temporelles(int nb_cases)
{
  champ_front()->fixer_nb_valeurs_temporelles(nb_cases);

}

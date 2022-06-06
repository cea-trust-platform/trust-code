/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Cond_lim_base_included
#define Cond_lim_base_included

#include <Champ_front.h>
#include <Ref_Zone_Cl_dis_base.h>
#include <Champ_Inc.h>

class Equation_base;
class Discretisation_base;
class Champ_Inc;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Cond_lim_base
//     Classe de base pour la hierarchie des classes qui representent
//     les differentes conditions aux limites (Dirichlet, Neumann ...).
//     Un objet condition aux limite sert a definir, pour une equation
//     donnee, les conditions aux limites a appliquer sur une frontiere
//     d'un domaine.
//     Chaque objet Cond_lim_base contient une reference vers l'objet
//     Zone_Cl_dis_base dont il fait partie.
//     Chaque objet contient egalement un objet Champ_front contenant les
//     valeurs a imposer sur la frontiere.
// .SECTION voir aussi
//     Cond_lim Zone_Cl_dis_base Frontiere_dis_base
//     Classe abstraite dont toutes les objets representant des conditions
//     aux limites doivent deriver.
//     Methode abstraite:
//       int compatible_avec_eqn(const Equation_base&) const
//////////////////////////////////////////////////////////////////////////////
class Cond_lim_base : public Objet_U
{

  Declare_base(Cond_lim_base);

public:

  virtual void completer();
  virtual int initialiser(double temps);
  virtual void mettre_a_jour(double temps);
  virtual void calculer_coeffs_echange(double temps);
  virtual void verifie_ch_init_nb_comp() const;
  virtual inline Frontiere_dis_base& frontiere_dis();
  virtual inline const Frontiere_dis_base& frontiere_dis() const;
  virtual void associer_fr_dis_base(const Frontiere_dis_base& ) ;
  inline Zone_Cl_dis_base& zone_Cl_dis();
  inline const Zone_Cl_dis_base& zone_Cl_dis() const;
  // Je transforme cette methode en une methode virtuelle
  // pour pouvoir la redefinir dans une classe fille
  virtual void associer_zone_cl_dis_base(const Zone_Cl_dis_base&);
  inline Champ_front& champ_front();
  inline const Champ_front& champ_front() const;

  // ajout de methode pour nre pas operer directement su lr champ_front
  virtual void set_temps_defaut(double temps);
  virtual void fixer_nb_valeurs_temporelles(int nb_cases);

  //ajout : renvoie les valeurs imposees pour une face
  virtual void champ_front(int, DoubleVect&) const;
  virtual int compatible_avec_eqn(const Equation_base&) const=0;
  virtual int compatible_avec_discr(const Discretisation_base&) const;
  virtual void injecter_dans_champ_inc(const Champ_Inc& ) const;

  virtual int a_mettre_a_jour_ss_pas_dt();

  //methode pour positionner le drapeau modifier_val_impl
  inline void set_modifier_val_imp(int);
  virtual void changer_temps_futur(double temps,int i);
  virtual int avancer(double temps);
  virtual int reculer(double temps);

protected:

  Champ_front le_champ_front;
  REF(Zone_Cl_dis_base) ma_zone_cl_dis;
  void err_pas_compatible(const Equation_base& ) const;
  void err_pas_compatible(const Discretisation_base& ) const;

  //ajout drapeau pour modifier ou pas la valeur imposee sur la condition limite
  int modifier_val_imp;
};


// Description:
//    Renvoie la frontiere discretisee a laquelle les conditions
//    aux limites s'appliquent.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee a laquelle les conditions
//                   aux limites sont associees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Frontiere_dis_base& Cond_lim_base::frontiere_dis()
{
  return le_champ_front.frontiere_dis();
}


// Description:
//    Renvoie la frontiere discretisee a laquelle les conditions
//    aux limites s'appliquent.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee a laquelle les conditions
//                   aux limites sont associees
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Frontiere_dis_base& Cond_lim_base::frontiere_dis() const
{
  return le_champ_front.frontiere_dis();
}


// Description:
//    Renvoie la zone des conditions aux limites discretisee dont
//    l'objet fait partie.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_Cl_dis_base&
//    Signification: la zone des conditions aux limites discretisee dont
//                   l'objet fait partie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Zone_Cl_dis_base& Cond_lim_base::zone_Cl_dis()
{
  return ma_zone_cl_dis.valeur();
}


// Description:
//    Renvoie la zone des conditions aux limites discretisee dont
//    l'objet fait partie.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_Cl_dis_base&
//    Signification: la zone des conditions aux limites discretisee dont
//                   l'objet fait partie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Zone_Cl_dis_base& Cond_lim_base::zone_Cl_dis() const
{
  return ma_zone_cl_dis.valeur();
}

inline Champ_front& Cond_lim_base::champ_front()
{
  return le_champ_front;
}

inline const Champ_front& Cond_lim_base::champ_front() const
{
  return le_champ_front;
}

// Description:
//    Positionne le drapeau modifier_val_imp a la valeur donnee :
//    - si drap == 1 : modifier_val_imp=1
//    - sinon        : modifier_val_imp=0
//    Ce drapeau permet a la CL de savoir si elle doit renvoyer la valeur stockee
//    telle quelle, ou si elle doit la traduire pour l'objet appelant
//    A la CL de savoir ensuite quelle traduction faire.
//    Voir application dans Temperature_imposee_paroi_H, ou la CL renvoie l'enthalpie
//    par defaut (modifier_val_imp=1), ou la temperature sinon (cas de l'operateur de diffusion)
//    Le drapeau est positionne dans le equation.derivee_en_temps_inco
//    selon l'operateur qui va etre appele
// Precondition:
// Parametre: drap
//    Signification: valeur a donner au drapeau
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Cond_lim_base::set_modifier_val_imp(int drap)
{
  //positionne le drapeau a 0 ou 1
  modifier_val_imp = (drap==1);
}
#endif

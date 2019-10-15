/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Echange_impose_base.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_impose_base_included
#define Echange_impose_base_included



#include <Cond_lim_base.h>




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Echange_impose_base:
//    Cette condition limite sert uniquement pour l'equation d'energie.
//    Elle correspond a imposer un echange de chaleur avec l'exterieur
//    du domaine en imposant une temperature exterieure T_ext et un
//    coefficient d'echange h_imp.
//    Le terme de flux calcule a partir du couple(h_imp,T_ext) s'ecrit :
//                           h_t(T_ext - T_entier)*Surf
//                          avec h_t : coefficient d'echange global.
//    Il figure au second membre de l'equation d'energie.
//    Soit l'utilisateur donne un coefficient d'echange qui correspond
//    uniquement a la paroi, auquel cas le programme calculera la diffusion
//    sur la demi-maille pres de la paroi, soit il donne un coefficient
//    d'echange global qui tient compte directement des deux precedents.
//    Les deux classes derivees Echange_externe_impose et Echange_global_impose
//    representent ces deux possibilites.
// .SECTION voir aussi
//    Cond_lim_base Echange_externe_impose Echange_global_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_impose_base  : public Cond_lim_base
{

  Declare_base_sans_constructeur(Echange_impose_base);

public:

  double h_imp(int num) const;
  double h_imp(int num,int k) const;
  virtual double T_ext(int num) const;
  virtual double T_ext(int num,int k) const;
  virtual void mettre_a_jour(double );
  virtual int initialiser(double temps);
  virtual int a_mettre_a_jour_ss_pas_dt()
  {
    return 1;
  };


  // ajout de methode pour ne pas operer directement sur champ_front
  virtual void set_temps_defaut(double temps);
  virtual void fixer_nb_valeurs_temporelles(int nb_cases);
  //
  virtual void changer_temps_futur(double temps,int i);
  virtual int avancer(double temps);
  virtual int reculer(double temps);
  virtual void associer_fr_dis_base(const Frontiere_dis_base& ) ;

  inline Champ_front& T_ext();
  inline const Champ_front& T_ext() const;
  inline Champ_front& h_imp();
  inline const Champ_front& h_imp() const;

  inline Echange_impose_base();
  virtual int compatible_avec_eqn(const Equation_base&) const;
  virtual int compatible_avec_discr(const Discretisation_base& ) const;
  virtual void completer();
protected :

  Champ_front h_imp_;
  int division_par_rhoCp_;
};


// Description:
//    Renvoie le champ T_ext de temperature imposee a la frontiere.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_front&
//    Signification: le champ T_ext de temperature imposee a la frontiere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_front& Echange_impose_base::T_ext()
{
  return le_champ_front;
}

// Description:
//    Renvoie le champ T_ext de temperature imposee a la frontiere.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_front&
//    Signification: le champ T_ext de temperature imposee a la frontiere
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_front& Echange_impose_base::T_ext() const
{
  return le_champ_front;
}

// Description:
//    Constructeur par defaut de la classe Echange_impose_base.
//    Ne construit rien, initialise le champ init a 0.
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
inline Echange_impose_base::Echange_impose_base()
{
  division_par_rhoCp_=-1;
}

inline Champ_front& Echange_impose_base::h_imp()
{
  return h_imp_;
}

inline const Champ_front& Echange_impose_base::h_imp() const
{
  return h_imp_;
}


#endif

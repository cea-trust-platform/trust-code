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
// File:        Reaction.h
// Directory:   $TRUST_ROOT/src/ThHyd/Chimie
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Reaction_included
#define Reaction_included

#include <List.h>
#include <ArrOfDouble.h>
#include <Champ_Fonc.h>
#include <Ref_Champ_Inc_base.h>
class Motcles;
class Champ_base;
class Probleme_base;

Declare_vect(REF(Champ_Inc_base));

class Reaction: public Objet_U
{
  Declare_instanciable(Reaction);
public:
  friend class Chimie;
  void completer(const Motcles& list_var,const ArrOfDouble& masse_molaire);

  void reagir(VECT(REF(Champ_Inc_base))& liste_c,const double deltat) const;
  double calcul_proportion_implicite(ArrOfDouble& C_temp,const ArrOfDouble& C0,double deltat, double seuil, double& poroportion_directe ) const;
  void extract_coef(ArrOfDouble& coeff_recactifs,ArrOfDouble& coeff_produits,const Motcles& list_var,const ArrOfDouble& masse_molaire) const;
  double calculer_pas_de_temps() const;
  int lire_motcle_non_standard(const Motcle& motlu, Entree& is);
  inline const Champ_base& get_omega() const
  {
    return omega_.valeur();
  };
  void discretiser_omega(const Probleme_base& pb,const Nom&);
  inline  const int& nb_sous_pas_de_temps_reaction() const
  {
    return nb_sous_pas_de_temps_reaction_ ;
  };
protected:
  Nom reactifs_,produits_,activite_;

  double constante_taux_reaction_;
  double enthalpie_reaction_; // en J/mol
  double contre_reaction_;     // prefacteur
  double c_r_Ea_;              // en J/mol
  /* double c0_D_G_;             // en Pa.m2/s         |
     double c1_D_G_;             // en Pa.m2/s/K       |
     double c2_D_G_;             // en Pa.m2/s/K2      |      pour D_G=1/p*(c0+c1*T+c2*T^2+c32*T^(3/2))
     double d_D_G_;              // en Pa.m2/s/K^(3/2) |

     double c_Re05Sc033_;        // constante de la loi de type Ranz-Marschall pour D_G (e.g. 0.3)
     double d_min_, d_max_;      // en m
  */
  double Sc_t_;               // Nombre de Schmidt turbulent (e.g. 0.8)
  double Ea_;                 // en J/mol
  double beta_/*,exp_comb_,exp_oxy_*/;
  ArrOfDouble coeff_Y_,coeff_stoechio_;
  ArrOfDouble coeff_activite_;
  Motcles save_alias_; //pour verifier que les Yi arrivent dans le meme ordre que lors de l'interpretation de la reaction
  mutable Champ_Fonc omega_;

  mutable double proportion_max_sur_delta_t_;
  double proportion_max_admissible_;
  int nb_sous_pas_de_temps_reaction_;

  /* int    nb_iter_impl_contre_reaction_;      // iteration du point fixe en cas de contre-reaction
     double sous_relax__impl_contre_reaction_;  // iteration du point fixe en cas de contre-reaction
  */
};

Declare_liste(Reaction);
#endif

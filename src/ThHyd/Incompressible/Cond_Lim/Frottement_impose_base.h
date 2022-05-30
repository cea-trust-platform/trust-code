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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Frottement_impose_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Cond_Lim
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Frottement_impose_base_included
#define Frottement_impose_base_included

#include <Navier.h>
#include <Param.h>
#include <TRUSTTab.h>
#include <Frontiere_dis_base.h>
#include <Ref_Frontiere_dis_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Frottement_impose_base
//    Classe de base pour des conditions aux limites de type Navier (v.n nul, v_t par contrainte
//    imposee) dans laquelle la contrainte imposee suit un coefficient de frottement :
//    (force subie) = - coefficient_frottement * (vitesse tangentielle)
//    La vitesse tangentielle peut etre soit prise directement a chaque face (classe Frottement_externe_impose),
//    soit etre prise en l'element voisin de la face (classe Frottement_global_impose)
// .SECTION voir aussi
//    Navier
//////////////////////////////////////////////////////////////////////////////
class Frottement_impose_base : public Navier
{

  Declare_base(Frottement_impose_base);

public:

  virtual void liste_faces_loi_paroi(IntTab&) =0;
  void associer_fr_dis_base(const Frontiere_dis_base& fr) override {la_frontiere_dis=fr;};
  int initialiser(double temps) override =0 ;
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& zcl) override { ma_zone_cl_dis=zcl;};
  virtual double coefficient_frottement(int i) const=0;
  virtual double coefficient_frottement(int i,int j) const=0;
  virtual void is_grad_v() {is_calc_qdm = 0;};

  // fonctions de cond_lim_base qui necessitent le champ_front qu'on met a zero car on fait abstraction du champ_front
  void completer() override {};
  void fixer_nb_valeurs_temporelles(int nb_cases) override {};
  inline Frontiere_dis_base& frontiere_dis() override {return la_frontiere_dis;};
  inline const Frontiere_dis_base& frontiere_dis() const override {return la_frontiere_dis;};
  void changer_temps_futur(double temps,int i) override {};
  void set_temps_defaut(double temps) override {};
  void calculer_coeffs_echange(double temps) override {};
  void verifie_ch_init_nb_comp() const override {};

protected:
  REF(Frontiere_dis_base) la_frontiere_dis;
  double mon_temps = -1e10;
  int is_calc_qdm = 1 ;
};

#endif

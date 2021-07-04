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
// File:        Fluide_Dilatable.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_Dilatable_included
#define Fluide_Dilatable_included

#include <Fluide_Incompressible.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Dilatable
//    Cette classe represente un d'un fluide dilatable,
//    heritant de l'incompressible
// .SECTION voir aussi
//     Milieu_base Fluide_Incompressible
//
//////////////////////////////////////////////////////////////////////////////

class Fluide_Dilatable : public Fluide_Incompressible
{
  Declare_instanciable_sans_constructeur(Fluide_Dilatable);

public :
  Fluide_Dilatable();
  void verifier_coherence_champs(int& err,Nom& message);
  void set_Cp(double);
  void update_rho_cp(double temps);
  virtual void checkTraitementPth(const Zone_Cl_dis&);
  inline const Champ_Inc& inco_chaleur() const { return inco_chaleur_.valeur(); }
  inline Champ_Inc& inco_chaleur() { return inco_chaleur_.valeur(); }
  inline const Champ_Inc& vitesse() const { return vitesse_.valeur(); }
  inline const Champ_Don& pression_tot() const { return pression_tot_; }
  inline Champ_Don& pression_tot() { return pression_tot_; }
  inline const Champ_Don& mu_sur_Schmidt() const { return mu_sur_Sc; }
  inline Champ_Don& mu_sur_Schmidt() { return mu_sur_Sc; }
  inline const Champ_Don& nu_sur_Schmidt() const { return nu_sur_Sc; }
  inline Champ_Don& nu_sur_Schmidt() { return nu_sur_Sc;  }
  inline double pression_th() const { return Pth_;  }
  inline double pression_thn() const { return Pth_n; }
  inline double pression_th1() const { return Pth1; }
  inline void set_pression_th(double Pth) { Pth_n = Pth_ = Pth; }

protected :
  int traitement_PTh; // flag pour le traitement de la pression thermo
  double Pth_;  //Pression thermodynamique
  double Pth_n;  //Pression thermodynamique a l'etape precedente
  double Pth1; //Pression thermodynamique calculee pour conserver la masse
  REF(Champ_Inc) inco_chaleur_;
  REF(Champ_Inc) vitesse_;
  REF(Champ_Inc) pression_;
  Champ_Don pression_tot_,mu_sur_Sc,nu_sur_Sc,rho_gaz,rho_comme_v;
  mutable DoubleTab tab_W_old_;

};

#endif /* Fluide_Dilatable_included */

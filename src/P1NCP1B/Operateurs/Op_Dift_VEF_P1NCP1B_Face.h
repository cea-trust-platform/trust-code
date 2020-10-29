/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Op_Dift_VEF_P1NCP1B_Face.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Dift_VEF_P1NCP1B_Face_included
#define Op_Dift_VEF_P1NCP1B_Face_included

#include <Op_Dift_VEF_base.h>
#include <Ref_Champ_P1NC.h>
#include <Ref_Zone_VEF_PreP1b.h>
#include <Ref_Zone_Cl_VEFP1B.h>
#include <Ref_Champ_Uniforme.h>
#include <Ref_Turbulence_paroi_base.h>
#include <Ref_Mod_turb_hyd_base.h>
#include <Ref_Modele_turbulence_scal_base.h>
#include <Matrice_Morse.h>
#include <Zone_VEF_PreP1b.h>
#include <Zone_Cl_VEFP1B.h>
#include <Matrice_Morse_Sym.h>
#include <SolveurSys.h>
//
// .DESCRIPTION class Op_Dift_VEF_P1NCP1B_Face
//  Cette classe represente l'operateur de diffusion
//  La discretisation est VEF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite est uniforme
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_VEF_P1NCP1B_Face : public Op_Dift_VEF_base
{
  Declare_instanciable(Op_Dift_VEF_P1NCP1B_Face);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  void associer_diffusivite(const Champ_base& );
  inline const Champ_base& diffusivite() const
  {
    return diffusivite_;
  };

  void associer_modele_turbulence(const Mod_turb_hyd_base& );

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  void verifier() const;

  void contribue_au_second_membre() const;
  void ajouter_contribution() const;

  double calculer_dt_stab() const;
  inline void remplir_nu(DoubleTab& ) const
  {
    Cerr<<__FILE__<<":"<<(int)__LINE__<<finl;
    exit();
  } ;

protected :
  REF(Champ_base) diffusivite_;

  DoubleTab& calculer_gradient_elem(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& calculer_gradient_som(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& calculer_divergence_elem(double, const DoubleTab&,
                                      const DoubleTab&, DoubleTab& ) const;
  DoubleTab& calculer_divergence_som(double, const DoubleTab&,
                                     const DoubleTab&, DoubleTab& ) const;
  DoubleTab& corriger_div_pour_Cl(DoubleTab& ) const;
  REF(Mod_turb_hyd_base) le_modele_turbulence;
  REF(Zone_VEF_PreP1b) la_zone_vef;
  REF(Zone_Cl_VEFP1B) la_zcl_vef;
  SolveurSys  solveur;
  int trans;
  double lambda;
  int filtre;
  Matrice_Morse_Sym masse;
  DoubleTab savgrad;
};
#endif

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
// File:        Convection_Diffusion_Fluide_Dilatable_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Convection_Diffusion_Fluide_Dilatable_base_included
#define Convection_Diffusion_Fluide_Dilatable_base_included

#include <Convection_Diffusion_Fluide_Dilatable_Proto.h>
#include <Convection_Diffusion_std.h>
#include <Ref_Fluide_Dilatable_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Fluide_Dilatable_base pour un fluide dilatable
//
// .SECTION voir aussi
//     Conv_Diffusion_std Convection_Diffusion_Temperature
//////////////////////////////////////////////////////////////////////////////

class Convection_Diffusion_Fluide_Dilatable_base : public Convection_Diffusion_std,
  public Convection_Diffusion_Fluide_Dilatable_Proto
{
  Declare_base(Convection_Diffusion_Fluide_Dilatable_base);

public :
  void associer_fluide(const Fluide_Dilatable_base& );
  void associer_milieu_base(const Milieu_base& ) override;
  int impr(Sortie& os) const override;
  const Champ_Don& diffusivite_pour_transport() const override;
  const Fluide_Dilatable_base& fluide() const;
  Fluide_Dilatable_base& fluide();
  const Milieu_base& milieu() const override;
  Milieu_base& milieu() override;

  void set_param(Param& titi) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  // je sais, je sais ... mais pas maintenant !
  void calculer_div_u_ou_div_rhou(DoubleTab& res) const override = 0;
  bool is_thermal() const override = 0;
  bool is_generic() const override = 0;

  // Methodes inlines
  inline const Champ_Inc& inconnue() const override { return l_inco_ch; }
  inline Champ_Inc& inconnue() override { return l_inco_ch; }
  inline const Zone_Cl_dis& zone_cl_modif() const { return zcl_modif_ ;}
  inline Zone_Cl_dis& zone_cl_modif() { return zcl_modif_ ;}

protected:
  Champ_Inc l_inco_ch;
  Zone_Cl_dis zcl_modif_;
  REF(Fluide_Dilatable_base) le_fluide;
};

#endif /* Convection_Diffusion_Fluide_Dilatable_base_included */

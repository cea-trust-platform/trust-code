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
// File:        Op_Diff_CoviMAC_base.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Op_Diff_CoviMAC_base_included
#define Op_Diff_CoviMAC_base_included

#include <Operateur_Diff_base.h>
#include <Op_Diff_Turbulent_base.h>
#include <Ref_Zone_CoviMAC.h>
#include <Ref_Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <SFichier.h>
class Champ_Fonc;


//
// .DESCRIPTION class Op_Diff_CoviMAC_base
//
// Classe de base des operateurs de diffusion CoviMAC

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_CoviMAC_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_CoviMAC_base : public Operateur_Diff_base
{


  Declare_base(Op_Diff_CoviMAC_base);

public:
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& ) override;

  void associer_diffusivite(const Champ_base& ) override;
  void completer() override;

  mutable IntTab pe_ext; // tableau aux faces de bord : (indice dans op_ext, indice d'element) pour les faces de type Echange_contact

  const Champ_base& diffusivite() const override;
  void mettre_a_jour(double t) override;

  /* methodes surchargeables dans des classes derivees pour modifier nu avant de calculer les gradients dans update_nu_xwh */
  virtual int dimension_min_nu() const /* dimension minimale de nu / nu_bord par composante */
  {
    return 1;
  };
  virtual void modifier_nu(DoubleTab& ) const { };

  /* diffusivite / conductivite. Attension : stockage nu(element, composante[, dim 1[, dim 2]]) */
  inline const DoubleTab& nu() const /* aux elements */
  {
    if (!nu_a_jour_) update_nu();
    return nu_;
  }

  /* flux aux faces (hors Echange_contact): cf. Zone_CoviMAC::fgrad */
  void update_phif(int full_stencil = 0) const;
  //indices : elems locaux dans phif_e([phif_d(f), phif_d(f + 1)[)
  mutable IntTab phif_d, phif_e; //stencils
  mutable DoubleTab phif_c; //coefficients

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  int impr(Sortie& os) const override;

protected:
  REF(Zone_CoviMAC) la_zone_poly_;
  REF(Zone_Cl_CoviMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;

  double t_last_maj_ = -1e10; //pour detecter quand on doit recalculer nu, xh, wh et fgrad

  /* diffusivite aux elems */
  void update_nu() const; //mise a jour
  mutable DoubleTab nu_;

  /* liste de sommets traites directement par l'operateur et non par Zone_CoviMAC::fgrad() (cf. Op_Diff_CoviMAC_Elem) */
  mutable IntTab som_ext;

  mutable int nu_constant_, nu_a_jour_ = 0, s_dist_init_ = 0, som_ext_init_ = 0, phif_a_jour_ = 0; //nu constant / nu a jour / phif a jour

  mutable SFichier Flux, Flux_moment, Flux_sum;
};
#endif

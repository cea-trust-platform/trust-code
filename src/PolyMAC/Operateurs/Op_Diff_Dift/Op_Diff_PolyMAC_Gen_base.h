/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Op_Diff_PolyMAC_Gen_base_included
#define Op_Diff_PolyMAC_Gen_base_included

#include <Operateur_Diff_base.h>
#include <TRUST_Ref.h>
#include <SFichier.h>

class Domaine_Cl_PolyMAC;
class Domaine_PolyMAC;

/*! @brief class Op_Diff_PolyMAC_Gen_base
 *
 *  Classe de base generique pour la diffusion PolyMAC
 *
 */
class Op_Diff_PolyMAC_Gen_base: public Operateur_Diff_base
{
  Declare_base(Op_Diff_PolyMAC_Gen_base);
public:
  void associer(const Domaine_dis_base&, const Domaine_Cl_dis_base&, const Champ_Inc_base&) override;
  void associer_diffusivite(const Champ_base& diffu) override { diffusivite_ = diffu; }
  const Champ_base& diffusivite() const override { return diffusivite_.valeur(); }

  DoubleTab& calculer(const DoubleTab&, DoubleTab&) const override;
  virtual void modifier_mu(DoubleTab&) const { }

  /* methodes surchargeables dans des classes derivees pour modifier nu avant de calculer les gradients dans update_nu_xwh */
  virtual int dimension_min_nu() const /* dimension minimale de nu / nu_bord par composante */
  {
    return 1;
  }

  /* diffusivite / conductivite. Attension : stockage nu(element, composante[, dim 1[, dim 2]]) */
  inline const DoubleTab& nu() const /* aux elements */
  {
    if (!nu_a_jour_) update_nu();
    return nu_;
  }

  virtual void update_nu() const = 0; //mise a jour

  int impr(Sortie& os) const override;

protected:
  mutable SFichier Flux, Flux_moment, Flux_sum; // Fichiers .out
  mutable DoubleTab nu_; // diffusivite aux elements
  mutable int nu_a_jour_ = 0, nu_constant_ = 0 /* Elie : pour valgrind */; // si on doit mettre a jour nu, nu constant

  OBS_PTR(Domaine_PolyMAC) le_dom_poly_;
  OBS_PTR(Domaine_Cl_PolyMAC) la_zcl_poly_;
  OBS_PTR(Champ_base) diffusivite_;
};

#endif /* Op_Diff_PolyMAC_Gen_base_included */


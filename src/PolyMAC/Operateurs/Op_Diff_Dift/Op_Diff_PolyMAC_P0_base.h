/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Op_Diff_PolyMAC_P0_base_included
#define Op_Diff_PolyMAC_P0_base_included

#include <Operateur_Diff_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC_P0.h>
#include <TRUST_Ref.h>
#include <SFichier.h>

/*! @brief class Op_Diff_PolyMAC_P0_base
 *
 *  Classe de base des operateurs de diffusion PolyMAC_P0
 *
 */

class Op_Diff_PolyMAC_P0_base: public Operateur_Diff_base
{
  Declare_base(Op_Diff_PolyMAC_P0_base);
public:
  void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Champ_Inc&) override;

  void associer_diffusivite(const Champ_base& diffu) override { diffusivite_ = diffu; }
  void completer() override;

  mutable IntTab pe_ext; // tableau aux faces de bord : (indice dans op_ext, indice d'element) pour les faces de type Echange_contact

  const Champ_base& diffusivite() const override { return diffusivite_.valeur(); }
  void mettre_a_jour(double t) override;

  /* methodes surchargeables dans des classes derivees pour modifier nu avant de calculer les gradients dans update_nu_xwh */
  virtual int dimension_min_nu() const /* dimension minimale de nu / nu_bord par composante */
  {
    return 1;
  }

  virtual void modifier_mu(DoubleTab&) const { }

  /* diffusivite / conductivite. Attension : stockage nu(element, composante[, dim 1[, dim 2]]) */
  inline const DoubleTab& nu() const /* aux elements */
  {
    if (!nu_a_jour_) update_nu();
    return nu_;
  }

  /* flux aux faces (hors Echange_contact): cf. Domaine_PolyMAC_P0::fgrad */
  void update_phif(int full_stencil = 0) const;

  //indices : elems locaux dans phif_e([phif_d(f), phif_d(f + 1)[)
  mutable IntTab phif_d, phif_e; //stencils
  mutable DoubleTab phif_c; //coefficients

  DoubleTab& calculer(const DoubleTab&, DoubleTab&) const override;
  int impr(Sortie& os) const override;

protected:
  REF(Domaine_PolyMAC_P0) le_dom_poly_;
  REF(Domaine_Cl_PolyMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;

  double t_last_maj_ = -1e10; //pour detecter quand on doit recalculer nu, xh, wh et fgrad

  /* diffusivite aux elems */
  void update_nu() const; //mise a jour
  mutable DoubleTab nu_;

  /* liste de sommets traites directement par l'operateur et non par Domaine_PolyMAC_P0::fgrad() (cf. Op_Diff_PolyMAC_P0_Elem) */
  mutable IntTab som_ext;

  mutable int nu_constant_ = 0 /* Elie : pour valgrind */, nu_a_jour_ = 0, s_dist_init_ = 0, som_ext_init_ = 0, phif_a_jour_ = 0; //nu constant / nu a jour / phif a jour

  mutable SFichier Flux, Flux_moment, Flux_sum;
};

#endif

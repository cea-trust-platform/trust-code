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

#include <Champ_Elem_PolyMAC_P0.h>
#include <Connectivite_som_elem.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Champ_Elem_PolyMAC_P0,"Champ_Elem_PolyMAC_P0",Champ_Elem_PolyMAC_P0P1NC);

Sortie& Champ_Elem_PolyMAC_P0::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_PolyMAC_P0::readOn(Entree& s) { return Champ_Elem_PolyMAC_P0P1NC::readOn(s) ; }

const Domaine_PolyMAC_P0& Champ_Elem_PolyMAC_P0::domaine_PolyMAC_P0() const
{
  return ref_cast(Domaine_PolyMAC_P0, le_dom_VF.valeur());
}

inline void Champ_Elem_PolyMAC_P0::mettre_a_jour(double tps)
{
  if (tps_last_calc_grad_ != tps) grad_a_jour = 0;
  Champ_Inc_P0_base::mettre_a_jour(tps);
}

int Champ_Elem_PolyMAC_P0::reprendre(Entree& fich)
{
  if (! via_ch_fonc_reprise()) return Champ_Inc_base::reprendre(fich); /* ie: resume last time ! */

  const Pb_Multiphase * pbm = mon_equation_non_nul() ? (sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr) : nullptr;
  if (pbm) return Champ_Inc_base::reprendre(fich);

  // sinon on fait ca ...
  const Domaine_PolyMAC_P0* domaine = le_dom_VF.non_nul() ? &ref_cast( Domaine_PolyMAC_P0,le_dom_VF.valeur()) : nullptr;
  valeurs().set_md_vector(MD_Vector()); //on enleve le MD_Vector...
  valeurs().resize(0);
  int ret = Champ_Inc_base::reprendre(fich);
  //et on remet le bon si on peut
  if (domaine) valeurs().set_md_vector(valeurs().dimension_tot(0) > domaine->nb_elem_tot() ? domaine->mdv_elems_faces : domaine->domaine().md_vector_elements());
  return ret;
}

void Champ_Elem_PolyMAC_P0::init_grad(int full_stencil) const
{
  if (fgrad_d.size()) return;
  const IntTab& f_cl = fcl();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_VF.valeur());
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites(); // CAL du champ à deriver
  domaine.fgrad(1, 0, 0, cls, f_cl, nullptr, nullptr, 1, full_stencil, fgrad_d, fgrad_e, fgrad_w);
}

void Champ_Elem_PolyMAC_P0::calc_grad(int full_stencil) const
{
  if (grad_a_jour) return;
  const IntTab& f_cl = fcl();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_VF.valeur());
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites(); // CAL du champ à deriver
  domaine.fgrad(1, 0, 0, cls, f_cl, nullptr, nullptr, 1, full_stencil, fgrad_d, fgrad_e, fgrad_w);
  grad_a_jour = 1;
  tps_last_calc_grad_ = temps();
}

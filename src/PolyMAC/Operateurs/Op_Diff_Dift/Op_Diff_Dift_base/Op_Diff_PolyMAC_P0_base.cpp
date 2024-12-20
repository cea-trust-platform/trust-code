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

#include <Echange_contact_PolyMAC_P0.h>
#include <Op_Diff_PolyMAC_P0_base.h>
#include <Frottement_impose_base.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Milieu_base.h>

Implemente_base(Op_Diff_PolyMAC_P0_base, "Op_Diff_PolyMAC_P0_base", Operateur_Diff_base);

Sortie& Op_Diff_PolyMAC_P0_base::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Diff_PolyMAC_P0_base::readOn(Entree& s) { return s; }

void Op_Diff_PolyMAC_P0_base::mettre_a_jour(double t)
{
  Operateur_base::mettre_a_jour(t);
  //si le champ est constant en temps, alors pas besoin de recalculer nu_ et les interpolations
  if (t <= t_last_maj_)
    return;

  if (!nu_constant_)
    nu_a_jour_ = 0, phif_a_jour_ = 0;

  t_last_maj_ = t;
}

void Op_Diff_PolyMAC_P0_base::completer()
{
  Operateur_base::completer();
  const Equation_base& eq = equation();
  int N = eq.inconnue().valeurs().line_size(), N_mil = eq.milieu().has_masse_volumique() ? eq.milieu().masse_volumique().valeurs().line_size() : N;
  int N_diff = diffusivite().valeurs().line_size(), D = dimension, N_nu = std::max(N * dimension_min_nu(), N_diff);

  if ((N_nu == N_mil) | (N_nu == N))
    nu_.resize(0, N); //isotrope
  else if ((N_nu == N_mil * D) | (N_nu == N * D))
    nu_.resize(0, N, D); //diagonal
  else if ((N_nu == N_mil * D * D) | (N_nu == N * D * D))
    nu_.resize(0, N, D, D); //complet
  else
    Process::exit(Nom("Op_Diff_PolyMAC_P0_base : diffusivity component count ") + Nom(N_nu) + " not among (" + Nom(N) + ", " + Nom(N * D) + ", " + Nom(N * D * D) + ")!");

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  domaine.domaine().creer_tableau_elements(nu_);
  const Conds_lim& cls = eq.domaine_Cl_dis().les_conditions_limites();
  nu_constant_ = (sub_type(Champ_Uniforme, diffusivite()) || sub_type(Champ_Don_Fonc_xyz, diffusivite()));
  if (nu_constant_)
    for (auto &itr : cls)
      if (sub_type(Echange_impose_base, itr.valeur()) || sub_type(Frottement_impose_base, itr.valeur()))
        nu_constant_ = 0;
}

void Op_Diff_PolyMAC_P0_base::update_nu() const
{
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const DoubleTab& nu_src = diffusivite().valeurs();
  int e, i, m, n, c_nu = nu_src.dimension_tot(0) == 1, d, db, D = dimension;
  int N = equation().inconnue().valeurs().line_size(),
      N_mil = equation().milieu().has_masse_volumique() ? equation().milieu().masse_volumique().valeurs().line_size() : N,
      N_nu = nu_.line_size(), N_nu_src = nu_src.line_size(), mult = N_nu / N;
  assert(N_nu % N == 0);

  /* nu_ : si necessaire, on doit etendre la champ source */
  if (N_nu == N && N_nu_src == N_mil)
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (n = 0; n < N; n++)
        nu_.addr()[N_nu * e + n] = nu_src(!c_nu * e, n); //facile
  else if (N_nu == N * D && N_nu_src == N_mil)
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++) //diagonal
          nu_(e, n, d) = nu_src(!c_nu * e, n);
  else if (N_nu == N * D * D && (N_nu_src == N_mil || N_nu_src == N_mil * D))
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (n = 0; n < N; n++) //complet
        for (d = 0; d < D; d++)
          for (db = 0; db < D; db++)
            nu_(e, n, d, db) = (d == db) * nu_src(!c_nu * e, N_nu_src == N_mil ? n : D * n + d);
  else
    abort();

  /* ponderation de nu par la porosite et par alpha (si pb_Multiphase) */
  const DoubleTab *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  for (e = 0; e < domaine.nb_elem_tot(); e++)
    for (n = 0, i = 0; n < N; n++)
      for (m = 0; m < mult; m++, i++)
        nu_.addr()[N_nu * e + i] *= equation().milieu().porosite_elem()(e) * (alp ? std::max((*alp)(e, n), 1e-8) : 1);

  /* modification par une classe fille */
  modifier_mu(nu_);

  nu_a_jour_ = 1;
}

void Op_Diff_PolyMAC_P0_base::update_phif(int full_stencil) const
{
  if (!full_stencil && phif_a_jour_)
    return; //deja fait, sauf si on demande tout le stencil
  const Champ_Inc_base& ch = equation().inconnue();
  const IntTab& fcl = sub_type(Champ_Face_PolyMAC_P0, ch) ? ref_cast(Champ_Face_PolyMAC_P0, ch).fcl() : ref_cast(Champ_Elem_PolyMAC_P0, ch).fcl();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  domaine.fgrad(ch.valeurs().line_size(), 0, la_zcl_poly_->les_conditions_limites(), fcl, &nu(), &som_ext, sub_type(Champ_Face_PolyMAC_P0, ch), full_stencil, phif_d, phif_e, phif_c);
  phif_a_jour_ = 1;
}

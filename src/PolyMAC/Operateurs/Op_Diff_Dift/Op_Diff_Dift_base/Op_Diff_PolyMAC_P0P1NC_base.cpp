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

#include <Op_Diff_PolyMAC_P0P1NC_Face.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Simpler_Base.h>
#include <Milieu_base.h>
#include <EChaine.h>
#include <deque>

Implemente_base(Op_Diff_PolyMAC_P0P1NC_base, "Op_Diff_PolyMAC_P0P1NC_base", Op_Diff_PolyMAC_Gen_base);

Sortie& Op_Diff_PolyMAC_P0P1NC_base::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Diff_PolyMAC_P0P1NC_base::readOn(Entree& s) { return s; }

void Op_Diff_PolyMAC_P0P1NC_base::mettre_a_jour(double t)
{
  Operateur_base::mettre_a_jour(t);
  //si le champ est constant en temps, alors pas besoin de recalculer nu_ et les interpolations
  if (t <= t_last_nu_)
    return;

  if (!nu_constant_)
    nu_a_jour_ = 0;

  t_last_nu_ = t;
}

void Op_Diff_PolyMAC_P0P1NC_base::completer()
{
  Operateur_base::completer();
  const Equation_base& eq = equation();
  int N = eq.inconnue().valeurs().line_size(), D = dimension, N_nu = std::max(N * dimension_min_nu(), diffusivite().valeurs().line_size());

  if (N_nu == N)
    nu_.resize(0, N); //isotrope
  else if (N_nu == N * D)
    nu_.resize(0, N, D); //diagonal
  else if (N_nu == N * D * D)
    nu_.resize(0, N, D, D); //complet
  else
    Process::exit(Nom("Op_Diff_PolyMAC_P0P1NC_base : diffusivity component count ") + Nom(N_nu) + "not among (" + Nom(N) + ", " + Nom(N * D) + ", " + Nom(N * D * D) + ")!");

  le_dom_poly_->domaine().creer_tableau_elements(nu_);
}

void Op_Diff_PolyMAC_P0P1NC_base::update_nu() const
{
  const Domaine_PolyMAC_P0P1NC& domaine = ref_cast(Domaine_PolyMAC_P0P1NC, le_dom_poly_.valeur());
  const DoubleTab& nu_src = diffusivite().valeurs();
  int e, i, m, n, N = equation().inconnue().valeurs().line_size(), N_nu = nu_.line_size(), N_nu_src = nu_src.line_size(), mult = N_nu / N, c_nu = nu_src.dimension_tot(0) == 1, d, db, D = dimension;
  assert(N_nu % N == 0);

  /* nu_ : si necessaire, on doit etendre la champ source */
  if (N_nu == N_nu_src)
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (n = 0; n < N_nu; n++)
        nu_.addr()[N_nu * e + n] = nu_src(!c_nu * e, n); //facile
  else if (N_nu == N * D && N_nu_src == N)
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (n = 0; n < N; n++)
        for (d = 0; d < D; d++) //diagonal
          nu_(e, n, d) = nu_src(!c_nu * e, n);
  else if (N_nu == N * D * D && (N_nu_src == N || N_nu_src == N * D))
    for (e = 0; e < domaine.nb_elem_tot(); e++)
      for (n = 0; n < N; n++) //complet
        for (d = 0; d < D; d++)
          for (db = 0; db < D; db++)
            nu_(e, n, d, db) = (d == db) * nu_src(!c_nu * e, N_nu_src == N ? n : D * n + d);
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

/* calcul des variables auxiliaires en semi-implicite */
void Op_Diff_PolyMAC_P0P1NC_base::update_aux(double t) const
{
  const std::string& nom_inco = (le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue()).le_nom().getString();
  int i, j, n_ext = (int) op_ext.size(), first_run = mat_aux.nb_lignes() == 0; /* nombre d'operateurs */
  if (first_run)
    for (mat_aux.dimensionner(n_ext, n_ext), i = 0; i < n_ext; i++)
      for (j = 0; j < n_ext; j++)
        mat_aux.get_bloc(i, j).typer("Matrice_Morse");
  std::vector<const Op_Diff_PolyMAC_P0P1NC_base*> opp_ext(n_ext);
  for (i = 0; i < n_ext; i++)
    opp_ext[i] = &ref_cast(Op_Diff_PolyMAC_P0P1NC_base, *op_ext[i]);
  std::vector<matrices_t> lines(n_ext); /* sous forme d'arguments pour dimensionner/assembler_blocs */
  for (i = 0; i < n_ext; i++)
    for (j = 0; j < n_ext; j++)
      lines[i][nom_inco + (j == i ? "" : "/" + op_ext[j]->equation().probleme().le_nom().getString())] = &ref_cast(Matrice_Morse, mat_aux.get_bloc(i, j).valeur());
  if (first_run)
    for (i = 0; i < n_ext; i++)
      opp_ext[i]->dimensionner_blocs_ext(1, lines[i]); //dimensionnement

  /* inconnue / second membre */
  std::deque<ConstDoubleTab_parts> v_part;
  for (i = 0; i < n_ext; i++)
    v_part.emplace_back(op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue().valeurs() : op_ext[i]->equation().inconnue().valeurs());
  MD_Vector_composite mdc; //MD_Vector composite : a partir de tous les seconds blocs
  for (i = 0; i < n_ext; i++)
    mdc.add_part(v_part[i][1].get_md_vector(), v_part[i][1].line_size());
  MD_Vector mdv;
  mdv.copy(mdc);
  DoubleTrav inco, secmem;
  MD_Vector_tools::creer_tableau_distribue(mdv, inco), MD_Vector_tools::creer_tableau_distribue(mdv, secmem);
  DoubleTab_parts p_inc(inco), p_sec(secmem);
  for (i = 0; i < n_ext; i++)
    p_inc[i] = v_part[i][1];

  /* assemblage */
  if (!first_run)
    for (i = 0; i < n_ext; i++)
      for (j = 0; j < n_ext; j++)
        ref_cast(Matrice_Morse, mat_aux.get_bloc(i, j).valeur()).get_set_coeff() = 0;
  for (i = 0; i < n_ext; i++)
    opp_ext[i]->ajouter_blocs_ext(1, lines[i], p_sec[i]);
  /* passage incremente/inconnues */
  mat_aux.ajouter_multvect(inco, secmem);
  /* resolution */
  if (first_run)
    {
      if (equation().parametre_equation().non_nul())
        solv_aux = ref_cast(Parametre_implicite, equation().parametre_equation().valeur()).solveur(); //on copie le solveur de l'equation
      else
        {
          EChaine chl("petsc cholesky { }");
          chl >> solv_aux;
        }
      solv_aux->fixer_limpr(-1);
    }

  solv_aux->reinit();
  solv_aux.resoudre_systeme(mat_aux, secmem, inco);
  /* maj de var_aux / t_last_aux dans tous les operateurs */
  for (i = 0; i < n_ext; i++)
    opp_ext[i]->var_aux = p_inc[i], opp_ext[i]->t_last_aux_ = t, opp_ext[i]->use_aux_ = 1;
}

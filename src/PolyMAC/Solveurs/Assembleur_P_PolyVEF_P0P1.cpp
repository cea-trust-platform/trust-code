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

#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Assembleur_P_PolyVEF_P0P1.h>
#include <Op_Grad_PolyVEF_P0P1_Face.h>
#include <Op_Div_PolyVEF_P0P1.h>
#include <Neumann_sortie_libre.h>
#include <Champ_Face_PolyVEF.h>
#include <Masse_PolyVEF_Face.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Matrice_Diagonale.h>
#include <Navier_Stokes_std.h>
#include <Matrice_Morse_Sym.h>
#include <Schema_Temps_base.h>
#include <Matrice_Bloc_Sym.h>
#include <Static_Int_Lists.h>
#include <Operateur_Grad.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Dirichlet.h>
#include <Debog.h>
#include <Piso.h>

Implemente_instanciable(Assembleur_P_PolyVEF_P0P1, "Assembleur_P_PolyVEF_P0P1", Assembleur_P_PolyMAC_P0);

Sortie& Assembleur_P_PolyVEF_P0P1::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Assembleur_P_PolyVEF_P0P1::readOn(Entree& s) { return Assembleur_P_PolyMAC_P0::readOn(s); }

int  Assembleur_P_PolyVEF_P0P1::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur()), mat_div_v, mat_div_p, mat_grad;
  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, equation());
  const Op_Div_PolyVEF_P0P1& div = ref_cast(Op_Div_PolyVEF_P0P1, eq.operateur_divergence().valeur());
  const Op_Grad_PolyVEF_P0P1_Face& grad = ref_cast(Op_Grad_PolyVEF_P0P1_Face, eq.operateur_gradient().valeur());

  const Domaine_PolyVEF_P0P1& dom = ref_cast(Domaine_PolyVEF_P0P1, le_dom_PolyMAC.valeur());
  int ne_tot = dom.nb_elem_tot(), nf_tot = dom.nb_faces_tot(), ns_tot = dom.nb_som_tot(), d, D = dimension, f, i;
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces();

  /* 1. stencil : seulement au premier passage */
  if (!stencil_done)
    {
      //dimensionner_blocs de div et grad, puis produit
      div.dimensionner_blocs({ { "vitesse", &mat_div_v }, { "pression", &mat_div_p } });
      grad.dimensionner_blocs_ext({ { "pression", &mat_grad } }, 1); //avec les faces virtuelles
      mat.affecte_prod(mat_div_v, mat_grad), mat.set_nb_columns(ne_tot + ns_tot);
      mat += mat_div_p;
      //stockage des stencils : pour mat, on ne peut stocker que la taille
      tab1.resize(1), tab1(0) = mat.get_set_tab1().size(), tab2.resize(1), tab2(0) = mat.get_set_tab2().size();
      div_v_tab1.ref_array(mat_div_v.get_set_tab1()), div_v_tab2.ref_array(mat_div_v.get_set_tab2());
      div_p_tab1.ref_array(mat_div_p.get_set_tab1()), div_p_tab2.ref_array(mat_div_p.get_set_tab2());
      grad_tab1.ref_array(mat_grad.get_set_tab1()), grad_tab2.ref_array(mat_grad.get_set_tab2());
      stencil_done = 1;
    }
  else //sinon, on recycle
    {
      mat.get_set_tab1().resize(tab1(0)), mat.get_set_tab2().resize(tab2(0)), mat.get_set_coeff().resize(tab2(0)), mat.set_nb_columns(ne_tot + ns_tot);
      mat_div_v.get_set_tab1().ref_array(div_v_tab1), mat_div_v.get_set_tab2().ref_array(div_v_tab2), mat_div_v.get_set_coeff().resize(div_v_tab2.size()), mat_div_v.set_nb_columns(D * nf_tot);
      mat_div_p.get_set_tab1().ref_array(div_p_tab1), mat_div_p.get_set_tab2().ref_array(div_p_tab2), mat_div_p.get_set_coeff().resize(div_p_tab2.size()), mat_div_p.set_nb_columns(ne_tot + ns_tot);
      mat_grad.get_set_tab1().ref_array(grad_tab1), mat_grad.get_set_tab2().ref_array(grad_tab2), mat_grad.get_set_coeff().resize(grad_tab2.size()), mat_grad.set_nb_columns(ne_tot + ns_tot);
    }

  /* gradient : par op_grad, puis produit par -1 / diag */
  DoubleTrav sec_grad(nf_tot, D), sec_div(ne_tot + ns_tot, 1), inv_diag(D * nf_tot);
  grad.ajouter_blocs_ext({ { "pression", &mat_grad } }, sec_grad, 1); //avec lignes virtuelles
  div.ajouter_blocs({ { "vitesse", &mat_div_v }, { "pression", &mat_div_p } }, sec_div); //avec lignes virtuelles
  for (f = 0, i = 0; f < nf_tot; f++)
    for (d = 0; d < D; d++, i++) inv_diag(i) = 1. / (diag.size() ? diag(i) : pf(f) * vf(f));
  mat_grad.diagmulmat(inv_diag);
  mat.affecte_prod(mat_div_v, mat_grad);
  mat_div_p *= -1;
  mat += mat_div_p; //partie (p, p) (pressions imposees) de la divergence

  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process premier process dont le domaine est non vide
  has_P_ref=0;
  for (int n_bord=0; n_bord<le_dom_PolyMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, le_dom_Cl_PolyMAC->les_conditions_limites(n_bord).valeur()) )
      has_P_ref=1;

  return 1;
}

int Assembleur_P_PolyVEF_P0P1::modifier_solution(DoubleTab& pression)
{
  DoubleTab_parts ppart(pression);
  for (int i = 0; !has_P_ref && i < ppart.size(); i++)
    ppart[i] -= mp_min_vect(ppart[i]);
  return 1;
}

void Assembleur_P_PolyVEF_P0P1::modifier_secmem_pour_incr_p(const DoubleTab& press, const double fac, DoubleTab& secmem) const
{
  const Domaine_PolyMAC& zone = ref_cast(Domaine_PolyMAC, le_dom_PolyMAC.valeur());
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, mon_equation->inconnue());
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const IntTab& scl_d = ch.scl_d(1), &scl_c = ch.scl_c(1);
  int s, ne_tot = zone.nb_elem_tot(), m, M = equation().probleme().get_champ("pression").valeurs().line_size();
  for (s = 0; s < zone.nb_som(); s++)
    if (scl_d(s) < scl_d(s + 1)) /* pression imposee */
      for (m = 0; m < M; m++)
        secmem(ne_tot + s, m) = (ref_cast(Neumann_sortie_libre, cls[scl_c(scl_d(s), 0)].valeur()).flux_impose(scl_c(scl_d(s), 1), m) - press(ne_tot + s, m)) / fac;
}

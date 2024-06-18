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

#include <Linear_algebra_tools_impl.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Frottement_impose_base.h>
#include <Masse_PolyVEF_Face.h>
#include <Champ_Face_PolyVEF.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Masse_ajoutee_base.h>
#include <Option_PolyVEF.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Dirichlet.h>
#include <Matrice33.h>
#include <Vecteur3.h>
#include <TRUSTTab.h>
#include <Piso.h>
#include <Synonyme_info.h>

Implemente_instanciable(Masse_PolyVEF_Face, "Masse_PolyVEF_P0_Face|Masse_PolyVEF_P0P1_Face", Masse_PolyMAC_P0_Face);
Add_synonym(Masse_PolyVEF_Face, "Masse_PolyVEF_P0P1NC_Face");

Sortie& Masse_PolyVEF_Face::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_PolyVEF_Face::readOn(Entree& s) { return s ; }

DoubleTab& Masse_PolyVEF_Face::appliquer_impl(DoubleTab& sm) const
{
  const Domaine_PolyVEF& dom =  ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const IntTab& f_e = dom.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue()).fcl();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces(), &fs = dom.face_surfaces();
  int i, e, f, d, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);
  const DoubleTab *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().champ_conserve().passe() : nullptr, &vfd = dom.volumes_entrelaces_dir(), &nf = dom.face_normales();
  double fac;

  //vitesses aux faces
  DoubleTrav proj(N);
  for (f = 0; f < dom.nb_faces(); f++)
    {
      if (fcl(f, 0) > 1)
        {
          for (proj = 0, d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              proj(n) += sm(f, N * d + n) * nf(f, d) / fs(f);
          //Dirichlet (ou Symetrie avec sym_as_diri) : on enleve tout en P0P1, on garde la compo normale pour les autres
          if (fcl(f, 0) > (Option_PolyVEF::sym_as_diri ? 1 : 2))
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                sm(f, N * d + n) = p0p1 ? 0 : proj(n) * nf(f, d) / fs(f);
          else if (fcl(f, 0) == 2 && !Option_PolyVEF::sym_as_diri && p0p1) //Symetrie en P0P1 sans sym_as_diri : on enleve la compo normale
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                sm(f, N * d + n) -= proj(n) * nf(f, d) / fs(f);
        }
      for (d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          {
            for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += vfd(f, i) / vf(f) * (a_r ? (*a_r)(e, n) : 1);
            if (fac > 1e-10) sm(f, N * d + n) /= pf(f) * vf(f) * fac; //vitesse calculee
            else sm(f, N * d + n) = 0; //cas d'une evanescence
          }
    }
  sm.echange_espace_virtuel();
  return sm;
}

void Masse_PolyVEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse *mat = matrices.at(nom_inc);
  const Domaine_PolyVEF& dom =  ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const DoubleTab& inco = equation().inconnue().valeurs();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue()).fcl();
  int i, j, k, f, d, db, D = dimension, n, m, N = inco.line_size() / D, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);

  for (auto &&kv : matrices)
    if (kv.second == mat || kv.second->nb_colonnes())
      {
        IntTrav sten(0, 2);
        std::vector<std::set<int>> s_sten(N * D); //stencil a mettre a chaque ligne
        for (f = 0; f < dom.nb_faces(); f++)
          {
            int mix = p0p1 ? (!Option_PolyVEF::sym_as_diri && fcl(f, 0) == 2) : (fcl(f, 0) > (Option_PolyVEF::sym_as_diri ? 1 : 2)); //va-t-on devoir melanger les lignes?
            for (i = 0; i < N * D; i++) s_sten[i].clear();
            if (kv.second == mat) //terme de masse : peut melanger les directions et/ou les composantes
              for (d = 0; d < D; d++)
                for (n = 0; n < N; n++)
                  for (db = (mix ? 0 : d);  db < (mix ? D : d + 1); db++)
                    for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)
                      s_sten[N * d + n].insert(N * (D * f + db) + m);
            //Dirichlet en P0/P0P1NC ou Symetrie en P0P1 : melange des directions (sans melanger les phases) de toutes les lignes
            if (mix)
              for (d = 0, i = N * D * f; d < D; d++)
                for (n = 0; n < N; n++, i++)
                  for (j = kv.second->get_tab1()(i) - 1; j < kv.second->get_tab1()(i + 1) - 1; j++)
                    for (k = kv.second->get_tab2()(j) - 1, db = 0; db < D; db++)
                      s_sten[N * db + n].insert(k);
            for (i = 0; i < N * D; i++)
              for (auto &&col : s_sten[i])
                sten.append_line(N * D * f + i, col);
          }
        Matrice_Morse mat2;
        Matrix_tools::allocate_morse_matrix(inco.size_totale(), kv.second->nb_colonnes(), sten, mat2);
        *kv.second += mat2;
      }
}

// XXX : a voir si on peut utiliser Solveur_Masse_Face_proto::ajouter_blocs_proto ...
void Masse_PolyVEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
  Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
  const Domaine_PolyVEF& dom =  ref_cast(Domaine_PolyVEF, le_dom_PolyMAC.valeur());
  const IntTab& f_e = dom.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue()).fcl();
  const DoubleVect& pf = equation().milieu().porosite_face(), &vf = dom.volumes_entrelaces(), &fs = dom.face_surfaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const DoubleTab& rho = equation().milieu().masse_volumique().passe(), &nf = dom.face_normales(),
                   *alpha = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr, *a_r = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr, &vfd = dom.volumes_entrelaces_dir();
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee")) : nullptr;
  const Conds_lim& cls = equation().inconnue().domaine_Cl_dis().les_conditions_limites();
  int i, j, k, e, f, m, d, db, D = dimension, n, N = inco.line_size() / D, cR = rho.dimension_tot(0) == 1, p0p1 = sub_type(Domaine_PolyVEF_P0P1, dom);
  double scal;
  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  DoubleTrav masse(N, N), masse_e(N, N), p_eq(N), p_cl(N), cl(D, N), nfu(D); //masse alpha * rho, contribution
  for (f = 0; f < dom.nb_faces(); f++) //faces reelles
    {
      /* calcul de la masse */
      if (!pbm)
        for (masse = 0, n = 0; n < N; n++) masse(n, n) = 1; //pas Pb_Multiphase -> pas de alpha * rho
      else for (masse = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (masse_e = 0, n = 0; n < N; n++) masse_e(n, n) = (*a_r)(e, n); //partie diagonale
            if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse_e); //partie masse ajoutee
            for (n = 0; n < N; n++)
              for (m = 0; m < N; m++) masse(n, m) += vfd(f, i) / vf(f) * masse_e(n, m); //contribution au alpha * rho de la face
          }
      masse *= pf(f) * vf(f) / dt;
      /* contribution hors CLs */
      for (d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          {
            for (m = 0; m < N; m++) secmem(f, N * d + n) += masse(n, m) * (passe(f, N * d + m) - resoudre_en_increments * inco(f, N * d + m));
            if (mat)
              for (m = 0; m < N; m++)
                if (masse(n, m))
                  (*mat)(N * (D * f + d) + n, N * (D * f + d) + m) += masse(n, m);
          }

      if (p0p1 && fcl(f, 0) > (Option_PolyVEF::sym_as_diri ? 1 : 2)) /* Dirichlet en P0P1 : remplacement total par la CL */
        {
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              secmem(f, N * d + n) = masse(n, n) * ((fcl(f, 0) == 3 ? ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n) : 0) - resoudre_en_increments * inco(f, N * d + n));
          for (auto &&kv : matrices)
            if (kv.second->nb_colonnes())
              for (i = N * D * f, d = 0; d < D; d++)
                for (n = 0; n < N; n++, i++)
                  for (j = kv.second->get_tab1()(i) - 1; j < kv.second->get_tab1()(i + 1) - 1; j++)
                    kv.second->get_set_coeff()(j) = kv.second == mat && mat->get_tab2()(j) - 1 == i ? masse(n, n) : 0;
        }
      else if (p0p1 ? (!Option_PolyVEF::sym_as_diri && fcl(f, 0) == 2) : (fcl(f, 0) > (Option_PolyVEF::sym_as_diri ? 1 : 2))) /* Symetrie en P0P1 / Dirichlet sur les autres : projection + CL */
        {
          for (d = 0; d < D; d++) nfu(d) = nf(f, d) / fs(f);
          /* second membre */
          for (p_eq = 0, d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              p_eq(n) += secmem(f, N * d + n) * nfu(d);
          /* CL */
          for (cl = 0, p_cl = 0, d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              cl(d, n) = masse(n, n) * ((fcl(f, 0) == 3 ? ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n) : 0) - resoudre_en_increments * inco(f, N * d + n)), p_cl(n) += cl(d, n) * nfu(d);
          /* projection de secmem */
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++)
              secmem(f, N * d + n) = p0p1 ? secmem(f, N * d + n) + (p_cl(n) - p_eq(n)) * nfu(d) : cl(d, n) + (p_eq(n) - p_cl(n)) * nfu(d);
          for (auto &&kv : matrices)
            if (kv.second->nb_colonnes())
              {
                /* projection des lignes de mat */
                for (i = N * D * f, n = 0; n < N; n++, i += N)
                  for (j = kv.second->get_tab1()(i) - 1; j < kv.second->get_tab1()(i + 1) - 1; j++)
                    {
                      for (k = kv.second->get_tab2()(j) - 1, scal = 0, d = 0; d < D; d++)
                        scal += (*kv.second)(N * (D * f + d) + n, k) * nfu(d);
                      for (d = 0; d < D; d++)
                        {
                          double& coeff = (*kv.second)(N * (D * f + d) + n, k);
                          coeff = p0p1 ? coeff - scal * nfu(d) : scal * nfu(d);
                        }
                    }
                /* ajout de la partie CL */
                if (kv.second == mat)
                  for (d = 0; d < D; d++)
                    for (n = 0; n < N; n++)
                      for (db = 0; db < D; db++)
                        (*mat)(N * (D * f + d) + n, N * (D * f + db) + n) += masse(n, n) * (p0p1 ? nfu(d) * nfu(db) : (d == db) - nfu(d) * nfu(db));
              }
        }
    }
}

//recalcule les vitesses tangentielles aux faces a partir des vitesses normales
DoubleTab& Masse_PolyVEF_Face::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  ref_cast(Champ_Face_PolyVEF, equation().inconnue()).update_vf2(x, incr);
  return x;
}

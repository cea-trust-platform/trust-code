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

#include <Op_Evanescence_Homogene_PolyVEF_P0_Face.h>
#include <Viscosite_turbulente_base.h>
#include <Op_Grad_PolyVEF_P0_Face.h>
#include <Champ_Face_PolyVEF_P0.h>
#include <Champ_Elem_PolyVEF_P0.h>
#include <Vitesse_relative_base.h>
#include <Echange_impose_base.h>
#include <Gravite_Multiphase.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyVEF_P0.h>
#include <Matrix_tools.h>
#include <Milieu_composite.h>
#include <Interface_base.h>
#include <Neumann_paroi.h>
#include <Pb_Multiphase.h>
#include <Dirichlet.h>
#include <SETS.h>

Implemente_instanciable(Op_Evanescence_Homogene_PolyVEF_P0_Face, "Op_Evanescence_HOMOGENE_PolyVEF_P0_Face", Op_Evanescence_Homogene_Face_base);

Sortie& Op_Evanescence_Homogene_PolyVEF_P0_Face::printOn(Sortie& os) const { return os; }
Entree& Op_Evanescence_Homogene_PolyVEF_P0_Face::readOn(Entree& is) { return Op_Evanescence_Homogene_Face_base::readOn(is); }

void Op_Evanescence_Homogene_PolyVEF_P0_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& inco = ch.valeurs();

  /* on doit pouvoir ajouter / soustraire les equations entre composantes */
  int i, j, f, d, D = dimension, n, N = inco.line_size() / D;
  if (N == 1) return; //pas d'evanescence en simple phase!
  for (auto &&n_m : matrices)
    if (n_m.second->nb_colonnes())
      {
        IntTrav sten(0, 2);

        std::set<int> idx;
        Matrice_Morse& mat = *n_m.second, mat2;
        /* equations aux faces : celles calculees seulement */
        for (f = 0; f < domaine.nb_faces(); f++, idx.clear())
          for (d = 0; d < D; d++)
            {
              for (i = N * (D * f + d), n = 0; n < N; n++, i++)
                for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                  idx.insert(mat.get_tab2()(j) - 1);
              for (i = N * (D * f + d), n = 0; n < N; n++, i++)
                for (auto &&c : idx) sten.append_line(i, c);
            }

        Matrix_tools::allocate_morse_matrix(mat.nb_lignes(), mat.nb_colonnes(), sten, mat2);
        mat = mat2; //pour forcer l'ordre des coefficients dans la matrice (accelere les operations ligne a ligne)
      }
}

void Op_Evanescence_Homogene_PolyVEF_P0_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{

  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleTab& inco = ch.valeurs(), &vfd = domaine.volumes_entrelaces_dir(), &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe(),
                   &rho = equation().milieu().masse_volumique().passe(),
                    &temp  = ref_cast(Pb_Multiphase, equation().probleme()).equation_energie().inconnue().passe(),
                     &press = ref_cast(QDM_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_qdm()).pression().passe(),
                      &mu = ref_cast(Milieu_composite, equation().milieu()).viscosite_dynamique().passe(),
                       *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr,
                        *k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr,
                         *gravity = (equation().probleme().has_champ("gravite")) ? &equation().probleme().get_champ("gravite").valeurs() : nullptr ;

  const DoubleVect& vf = domaine.volumes_entrelaces(), &dh_e = milc.diametre_hydraulique_elem();
  int d, D = dimension, e, f, i, j, k, n, m, N = inco.line_size() / D, Nk = (k_turb) ? (*k_turb).line_size() : 0, cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Np = press.line_size();
  if (N == 1) return; //pas d'evanescence en simple phase!

  double a_eps = alpha_res_, a_eps_min = alpha_res_min_, a_m, a_max; //seuil de declenchement du traitement de l'evanescence

  /* recherche de phases evanescentes et traitement des seconds membres */
  DoubleTrav coeff(D, N, 2); //coeff(d, n, 0/1) : coeff a appliquer a l'equation existante / a l'eq. "inco = v_maj"
  Matrice_Morse& mat_diag = *matrices.at(ch.le_nom().getString());

  DoubleTab dvr_face(domaine.nb_faces(), N, N, N); // Derivee de vr(n,k) en f par rapport a la phase l ; pour l'instant toujours selon d2=d
  // On se le trimballe parce que quelqu'un a separe la boucle sur les matrices de celle sur le secmem

  /* calcul de la vitesse de derive : on va chercher les quantites intermediaires requises */
  Vitesse_relative_base::input_t in;
  Vitesse_relative_base::output_t out;
  out.vr.resize(N, N, D), out.dvr.resize(N, N, D, N*D);
  const Vitesse_relative_base* correlation_vd = pbm.has_correlation("vitesse_relative") ? &ref_cast(Vitesse_relative_base, pbm.get_correlation("vitesse_relative")) : nullptr;
  DoubleTrav gradAlpha, vort, nut, fvn(N);
  const int is_turb = ref_cast(Operateur_Diff_base, ref_cast(QDM_Multiphase, pbm.equation_qdm()).operateur_diff().l_op_base()).is_turb();
  if (correlation_vd)
    {
      in.alpha.resize(N), in.rho.resize(N), in.mu.resize(N), in.d_bulles.resize(N), in.k.resize(N), in.nut.resize(N), in.v.resize(D, N), in.sigma.resize(N*(N-1)/2), in.g.resize(D);
      if (correlation_vd->needs_grad_alpha())
        {
          gradAlpha.resize(domaine.nb_faces(), D, N);
          abort();
          // calc_grad_alpha_faces(gradAlpha);
          in.gradAlpha.resize(D, N);
        }
      if (correlation_vd->needs_vort())
        {
          vort.resize(domaine.nb_faces(), D, N);
          abort();
          // calc_vort_faces(vort);
          in.vort.resize(D, N);
        }
      if (is_turb)
        {
          nut.resize(domaine.nb_elem_tot(), N);
          ref_cast(Viscosite_turbulente_base, (*ref_cast(Operateur_Diff_base, equation().operateur(0).l_op_base()).correlation_viscosite_turbulente())).eddy_viscosity(nut); //remplissage par la correlation
        }
    }

  for (f = 0; f < domaine.nb_faces(); f++)
    {
      /* phase majoritaire : avec alpha interpole par defaut, avec alpha amont pour les ierations de SETS / ICE */
      for (a_max = 0, k = -1, n = 0; n < N; n++)
        {
          for (a_m = 0, i = 0; i < 2; i++)
            if ((e = f_e(f, i)) >= 0)
              a_m += vfd(f, i) / vf(f) * alpha(e, n);
          if (a_m > a_max) k = n, a_max = a_m;
        }
      if (k < 0) abort();

      if (correlation_vd)
        {
          for (n = 0; n < N; n++)
            for (d = 0; d < D; d++) in.v(d, n) = inco(f, N * d + n);
          for (in.alpha = 0, in.rho = 0, in.mu = 0, in.d_bulles = 0, in.k = 0, in.nut = 0, in.dh = 0, in.g = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            {
              for (n = 0; n < N; n++)
                {
                  in.alpha(n) += vfd(f, i) / vf(f) * alpha(e, n);
                  in.rho(n) += vfd(f, i) / vf(f) * rho(!cR * e, n);
                  in.mu(n) += vfd(f, i) / vf(f) * mu(!cM * e, n);
                  in.d_bulles(n)+=vfd(f, i) / vf(f) *((d_bulles) ? (*d_bulles)(e, n) : -1.) ;
                  in.dh += vfd(f, i) / vf(f) * dh_e(e);
                  for (m = n+1; m < N; m++)
                    if (milc.has_interface(n, m))
                      {
                        const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (m-n-1); // Et oui ! matrice triang sup !
                        Interface_base& sat = milc.get_interface(n, m);
                        in.sigma(ind_trav) = sat.sigma(temp(e, n), press(e, n * (Np > 1)));
                      }
                }
              for (n = 0; n < Nk; n++) in.k(n) += vfd(f, i) / vf(f) *((k_turb) ? (*k_turb)(e, n) : -1.), in.nut(n) += vfd(f, i) / vf(f) *((is_turb) ? nut(e, n) : -1.) ;
              for (d = 0; d < D; d++) in.g(d) += vfd(f, i) / vf(f) * (*gravity)(e,d);
              if (correlation_vd->needs_grad_alpha())
                for (n = 0; n < N; n++)
                  for (d = 0; d < D; d++) in.gradAlpha(d, n) = gradAlpha(f, d, n);
              if (correlation_vd->needs_vort())
                for (n = 0; n < N; n++)
                  for (d = 0; d < D; d++) in.vort(d, n) = vort(f, d, n);
            }
          correlation_vd->vitesse_relative(in, out);
        }

      /* phases evanescentes : avec alpha amont. La phase majoritaire ne peut pas etre evanescente! */
      for (coeff = 0, d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          {
            for (a_m = 0, i = 0; i < 2; i++)
              if ((e = f_e(f, i)) >= 0)
                a_m += vfd(f, i) / vf(f) * alpha(e, n);
            if (n != k && a_m < a_eps)
              {
                coeff(d, n, 1) = mat_diag(N * (D * f + d) + k, N * (D * f + d) + k) * (coeff(d, n, 0) = std::min(std::max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
                double flux = coeff(d, n, 0) * secmem(f, N * d + n) + coeff(d, n, 1) * (inco(f, N * d + n) - inco(f, N * d + k) - out.vr(n, k, d));
                secmem(f, N * d + k) += flux, secmem(f, N * d + n) -= flux;
              }
          }

      /* lignes de matrices */
      for (auto &&n_m : matrices)
        if (n_m.second->nb_colonnes())
          {
            int diag = (n_m.first == ch.le_nom().getString()); //est-on sur le bloc diagonal?
            Matrice_Morse& mat = *n_m.second;
            for (d = 0; d < D; d++)
              for (n = 0; n < N; n++)
                if (coeff(d, n, 0))
                  for (i = mat.get_tab1()(N * (D * f + d) + n) - 1, j = mat.get_tab1()(N * (D * f + d) + k) - 1; i < mat.get_tab1()(N * (D * f + d) + n + 1) - 1; i++, j++)
                    {
                      assert(mat.get_tab2()(i) == mat.get_tab2()(j));
                      int c = diag * mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                      mat.get_set_coeff()(j) +=  coeff(d, n, 0) * mat.get_set_coeff()(i) - coeff(d, n, 1) * ((c == N * (D * f + d) + n) - (c == N * (D * f + d) + k));
                      mat.get_set_coeff()(i) += -coeff(d, n, 0) * mat.get_set_coeff()(i) + coeff(d, n, 1) * ((c == N * (D * f + d) + n) - (c == N * (D * f + d) + k));

                      mat.get_set_coeff()(j) +=  - coeff(d, n, 1) * ( - out.dvr(n, k, d, N * d + n) * (c == N * f + n) -  out.dvr(n, k, d, N * d + k) * (c == N * f + k));
                      mat.get_set_coeff()(i) +=  + coeff(d, n, 1) * ( - out.dvr(n, k, d, N * d + n) * (c == N * f + n) -  out.dvr(n, k, d, N * d + k) * (c == N * f + k));
                    }
          }
    }
}

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

#include <Op_Evanescence_Homogene_Face_base.h>
#include <Viscosite_turbulente_base.h>
#include <Vitesse_relative_base.h>
#include <Gravite_Multiphase.h>
#include <Milieu_composite.h>
#include <Champ_Face_base.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Domaine_VF.h>
#include <Param.h>
#include <SETS.h>

Implemente_base(Op_Evanescence_Homogene_Face_base,"Op_Evanescence_Homogene_Face_base",Operateur_Evanescence_base);

Sortie& Op_Evanescence_Homogene_Face_base::printOn(Sortie& os) const { return os; }

Entree& Op_Evanescence_Homogene_Face_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("alpha_res", &alpha_res_, Param::REQUIRED);
  param.ajouter("alpha_res_min", &alpha_res_min_);
  param.lire_avec_accolades_depuis(is);

  Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  if (pbm.has_correlation("Vitesse_relative") && !pbm.has_correlation("gravite")) Process::exit(que_suis_je() + " : you must define a multiphase gravity field if you want a drift flux!!");
  if (pbm.has_correlation("Vitesse_relative") && ref_cast(Vitesse_relative_base, pbm.get_correlation("Vitesse_relative")).needs_vort()) pbm.creer_champ("vorticite");

  return is;
}

void Op_Evanescence_Homogene_Face_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* on doit pouvoir ajouter / soustraire les equations entre composantes */
  int i, j, f, n, N = inco.line_size();
  if (N == 1) return; //pas d'evanescence en simple phase!
  for (auto &&n_m : matrices)
    if (n_m.second->nb_colonnes())
      {
        IntTab sten(0, 2);


        std::set<int> idx;
        Matrice_Morse& mat = *n_m.second, mat2;
        /* equations aux faces : celles calculees seulement */
        for (f = 0; f < domaine.nb_faces(); f++, idx.clear())
          if (fcl(f, 0) < 2)
            {
              for (i = N * f, n = 0; n < N; n++, i++)
                for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                  idx.insert(mat.get_tab2()(j) - 1);
              for (i = N * f, n = 0; n < N; n++, i++)
                for (auto &&c : idx)
                  if (c >= 0)
                    sten.append_line(i, c);
            }
          else if (n_m.first == ch.le_nom().getString()) /* CLs a valeur imposee : diagonale */
            for (n = 0, i = N * f; n < N; n++, i++) sten.append_line(i, i);

        /* equations aux elements si aux */
        dimensionner_blocs_aux(idx, sten, mat);

        Matrix_tools::allocate_morse_matrix(mat.nb_lignes(), mat.nb_colonnes(), sten, mat2);
        mat = mat2; //pour forcer l'ordre des coefficients dans la matrice (accelere les operations ligne a ligne)
      }
}

void Op_Evanescence_Homogene_Face_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{

  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const bool reso_en_T = pbm.resolution_en_T();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& inco = ch.valeurs(), &vfd = domaine.volumes_entrelaces_dir(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe(),
                    &rho = equation().milieu().masse_volumique().passe(),
                     &temp_ou_enth  = ref_cast(Pb_Multiphase, equation().probleme()).equation_energie().inconnue().passe(),
                      &press = ref_cast(QDM_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_qdm()).pression().passe(),
                       &mu = ref_cast(Milieu_composite, equation().milieu()).viscosite_dynamique().passe(),
                        *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr,
                         *k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr,
                          *gravity = (equation().probleme().has_champ("gravite")) ? &equation().probleme().get_champ("gravite").valeurs() : nullptr ;

  const DoubleVect& vf = domaine.volumes_entrelaces(), &dh_e = milc.diametre_hydraulique_elem();
  int e, f, i, j, k, l, n, m, N = inco.line_size(), Nk = (k_turb) ? (*k_turb).line_size() : 0, d, D = dimension, cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Np = press.line_size(),
                              iter = sub_type(SETS, equation().schema_temps()) ? 0 * ref_cast(SETS, equation().schema_temps()).iteration_ : 0;
  if (N == 1) return; //pas d'evanescence en simple phase!

  double a_eps = alpha_res_, a_eps_min = alpha_res_min_, a_m, a_max; //seuil de declenchement du traitement de l'evanescence

  /* recherche de phases evanescentes et traitement des seconds membres */
  IntTrav maj(inco.dimension_tot(0)); //maj(i) : phase majoritaire de la ligne i
  DoubleTrav coeff(inco.dimension_tot(0), inco.line_size(), 2); //coeff(i, n, 0/1) : coeff a appliquer a l'equation existante / a l'eq. "inco = v_maj"
  Matrice_Morse& mat_diag = *matrices.at(ch.le_nom().getString());

  DoubleTab dvr_face(domaine.nb_faces(), N, N, N); // Derivee de vr(n,k) en f par rapport a la phase l ; pour l'instant toujours selon d2=d
  // On se le trimballe parce que quelqu'un a separe la boucle sur les matrices de celle sur le secmem

  /* calcul de la vitesse de derive : on va chercher les quantites intermediaires requises */
  Vitesse_relative_base::input_t in;
  Vitesse_relative_base::output_t out;
  out.vr.resize(N, N, D), out.dvr.resize(N, N, D, N*D);
  const Vitesse_relative_base* correlation_vd = pbm.has_correlation("vitesse_relative") ? &ref_cast(Vitesse_relative_base, pbm.get_correlation("vitesse_relative")) : nullptr;
  DoubleTab gradAlpha, vort, nut;
  const int is_turb = ref_cast(Operateur_Diff_base, ref_cast(QDM_Multiphase, pbm.equation_qdm()).operateur_diff().l_op_base()).is_turb();
  if (correlation_vd)
    {
      in.alpha.resize(N), in.rho.resize(N), in.mu.resize(N), in.d_bulles.resize(N), in.k.resize(N), in.nut.resize(N), in.v.resize(D, N), in.sigma.resize(N*(N-1)/2), in.g.resize(D);
      if (correlation_vd->needs_grad_alpha())
        {
          gradAlpha.resize(domaine.nb_faces(), D, N);
          calc_grad_alpha_faces(gradAlpha);
          in.gradAlpha.resize(D, N);
        }
      if (correlation_vd->needs_vort())
        {
          vort.resize(domaine.nb_faces(), D, N);
          calc_vort_faces(vort);
          in.vort.resize(D, N);
        }
      if (is_turb)
        {
          nut.resize(domaine.nb_elem_tot(), N);
          ref_cast(Viscosite_turbulente_base, (*ref_cast(Operateur_Diff_base, equation().operateur(0).l_op_base()).correlation_viscosite_turbulente())).eddy_viscosity(nut); //remplissage par la correlation
        }
    }

  for (f = 0; f < domaine.nb_faces(); f++)
    if (fcl(f, 0) < 2)
      {
        /* phase majoritaire : avec alpha interpole par defaut, avec alpha amont pour les ierations de SETS / ICE */
        for (a_max = 0, k = -1, n = 0; n < N; n++)
          {
            if (iter) a_m = alpha(f_e(f, f_e(f, 1) >= 0 && inco(f, n) < 0), n);
            else
              {
                for (a_m = 0, i = 0; i < 2; i++)
                  if ((e = f_e(f, i)) >= 0)
                    a_m += vfd(f, i) / vf(f) * alpha(e, n);
              }
            if (a_m > a_max) k = n, a_max = a_m;
          }
        if (k >= 0) maj(f) = k;
        else abort();

        if (correlation_vd)
          {
            for (n = 0; n < N; n++)
              for (d = 0; d < D; d++) in.v(d, n) = inco(f, n) * domaine.face_normales(f, d) / domaine.face_surfaces(f);
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
                          in.sigma(ind_trav) = reso_en_T ? sat.sigma(temp_ou_enth(e, n), press(e, n * (Np > 1))) : sat.sigma_h(temp_ou_enth(e, n), press(e, n * (Np > 1)));
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
        for (n = 0; n < N; n++)
          {
            if (iter) a_m = alpha(f_e(f, f_e(f, 1) >= 0 && inco(f, n) < 0), n);
            else
              {
                for (a_m = 0, i = 0; i < 2; i++)
                  if ((e = f_e(f, i)) >= 0)
                    a_m += vfd(f, i) / vf(f) * alpha(e, n);
              }
            if (n != k && a_m < a_eps)
              {
                coeff(f, n, 1) = mat_diag(N * f + k, N * f + k) * (coeff(f, n, 0) = std::min(std::max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
                DoubleTab vr_face(N, N);
                for (d = 0; d < D; ++d)
                  {
                    vr_face(n, k) += out.vr(n, k, d) * domaine.face_normales(f, d) / domaine.face_surfaces(f);
                    for (l=0 ; l<N ; l++) dvr_face(f, n, k,l) += out.dvr(n, k, d, l*D+d) * domaine.face_normales(f, d) / domaine.face_surfaces(f);
                  }
                double flux = coeff(f, n, 0) * secmem(f, n) + coeff(f, n, 1) * (inco(f, n) - inco(f, k) - vr_face(n, k));
                secmem(f, k) += flux, secmem(f, n) -= flux;
              }
          }
      }

  /* lignes de matrices */
  for (auto &&n_m : matrices)
    if (n_m.second->nb_colonnes())
      {
        int diag = (n_m.first == ch.le_nom().getString()); //est-on sur le bloc diagonal?
        Matrice_Morse& mat = *n_m.second;
        /* faces */
        for (f = 0; f < domaine.nb_faces(); f++)
          if (fcl(f, 0) < 2)
            for (n = 0; n < N; n++)
              if (coeff(f, n, 0))
                {
                  k = maj(f);
                  for (i = mat.get_tab1()(N * f + n) - 1, j = mat.get_tab1()(N * f + k) - 1; i < mat.get_tab1()(N * f + n + 1) - 1; i++, j++)
                    {
                      assert(mat.get_tab2()(i) == mat.get_tab2()(j));
                      int c = diag * mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                      mat.get_set_coeff()(j) +=  coeff(f, n, 0) * mat.get_set_coeff()(i) - coeff(f, n, 1) * ((c == N * f + n) - (c == N * f + k));
                      mat.get_set_coeff()(i) += -coeff(f, n, 0) * mat.get_set_coeff()(i) + coeff(f, n, 1) * ((c == N * f + n) - (c == N * f + k));

                      mat.get_set_coeff()(j) +=  - coeff(f, n, 1) * ( - dvr_face(f, n, k, n)*(c == N * f + n) - dvr_face(f, n, k, n)*(c == N * f + k));
                      mat.get_set_coeff()(i) +=  + coeff(f, n, 1) * ( - dvr_face(f, n, k, n)*(c == N * f + n) - dvr_face(f, n, k, k)*(c == N * f + k));
                    }
                }
      }

  // si var aux :
  ajouter_blocs_aux(maj, coeff, matrices, secmem);
}

void Op_Evanescence_Homogene_Face_base::calc_vort_faces(DoubleTab& vort) const
{
  vort = 0;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces();
  const DoubleTab& n_f = domaine.face_normales(), &vfd = domaine.volumes_entrelaces_dir();
  const DoubleTab& vorticite = equation().probleme().get_champ("vorticite").passe(),
                   &grad_v = equation().probleme().get_champ("gradient_vitesse").passe();

  const int D = dimension, N = vort.dimension_tot(2), nf = domaine.nb_faces(), nf_tot = domaine.nb_faces_tot();
  int c, e, f, n, d, d2, n_l = 0;
  DoubleTrav grad_l(D, D), scal_grad(D);

  for (f = 0; f < nf; f++)
    for (c = 0; c < 2; c++)
      {
        e = f_e(f, c);

        if (e < 0)
          continue;

        if (D == 2)
          {
            for (n = 0; n < N; n++)
              for (d = 0; d < D; d++)
                vort(f, d, n) += vorticite(e, n) * vfd(f, c) / vf(f);
          }
        else
          {
            for (n = 0; n < N; n++)
              for (d = 0; d < D; d++)
                vort(f, d, n) += vorticite(e, N * d + n) * vfd(f, c) / vf(f);
          }
      }

  if (D == 3)
    for (f = 0; f < nf; f++)
      {
        grad_l = 0; // we fill grad_l so that grad_l(d, d2) = du_d/dx_d2 by averaging between both elements
        for (d = 0; d < D; d++)
          for (d2 = 0; d2 < D; d2++)
            for (c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
              grad_l(d, d2) += vfd(f, c) / vf(f) * grad_v(nf_tot + D * e + d2, n_l * D + d);
        //We replace the n_l components by the one calculated without interpolation to elements
        scal_grad = 0; // scal_grad(d) = grad(u_d).n_f
        for (d = 0; d < D; d++)
          for (d2 = 0; d2 < D; d2++)
            scal_grad(d) += grad_l(d, d2) * n_f(f, d2) / fs(f);
        for (d = 0; d < D; d++)
          for (d2 = 0; d2 < D; d2++)
            grad_l(d, d2) += (grad_v(f, n_l * D + d) - scal_grad(d)) * n_f(f, d2) / fs(f);
        // We calculate the local vorticity using this local gradient
        vort(f, 0, n_l) = grad_l(2, 1) - grad_l(1, 2); // dUz/dy - dUy/dz
        vort(f, 1, n_l) = grad_l(0, 2) - grad_l(2, 0); // dUx/dz - dUz/dx
        vort(f, 2, n_l) = grad_l(1, 0) - grad_l(0, 1); // dUy/dx - dUx/dy
      }
}

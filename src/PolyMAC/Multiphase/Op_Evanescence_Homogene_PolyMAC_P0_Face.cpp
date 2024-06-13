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

#include <Op_Evanescence_Homogene_PolyMAC_P0_Face.h>
#include <Viscosite_turbulente_base.h>
#include <Op_Grad_PolyMAC_P0_Face.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Vitesse_relative_base.h>
#include <Echange_impose_base.h>
#include <Gravite_Multiphase.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC_P0.h>
#include <Milieu_composite.h>
#include <Interface_base.h>
#include <Neumann_paroi.h>
#include <Pb_Multiphase.h>
#include <Dirichlet.h>

Implemente_instanciable(Op_Evanescence_Homogene_PolyMAC_P0_Face, "Op_Evanescence_HOMOGENE_PolyMAC_P0_Face", Op_Evanescence_Homogene_Face_base);

Sortie& Op_Evanescence_Homogene_PolyMAC_P0_Face::printOn(Sortie& os) const { return os; }
Entree& Op_Evanescence_Homogene_PolyMAC_P0_Face::readOn(Entree& is) { return Op_Evanescence_Homogene_Face_base::readOn(is); }

void Op_Evanescence_Homogene_PolyMAC_P0_Face::dimensionner_blocs_aux(std::set<int>& idx, IntTrav& sten,  Matrice_Morse& mat ) const
{
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  const int nf_tot = domaine.nb_faces_tot(), D = dimension, N = ch.valeurs().line_size() ;
  int i, j, l, e, d, n;

  for (e = 0, l = nf_tot; e < domaine.nb_elem_tot(); e++)
    for (d = 0; d < D; d++, l++, idx.clear())
      {
        for (i = N * l, n = 0; n < N; n++, i++)
          for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
            idx.insert(mat.get_tab2()(j) - 1);
        for (i = N * l, n = 0; n < N; n++, i++)
          for (auto &&c : idx)
            sten.append_line(i, c);
      }
}

void Op_Evanescence_Homogene_PolyMAC_P0_Face::ajouter_blocs_aux(IntTrav& maj, DoubleTrav coeff, matrices_t matrices, DoubleTab& secmem) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const bool res_en_T = pbm.resolution_en_T();
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const Champ_Face_base& ch = ref_cast(Champ_Face_base, equation().inconnue().valeur());
  const DoubleTab& inco = ch.valeurs(), &alpha = pbm.equation_masse().inconnue().passe(),
                   &rho = equation().milieu().masse_volumique().passe(),
                    &temp_ou_enth  = pbm.equation_energie().inconnue().passe(),
                     &press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().passe(),
                      &mu = ref_cast(Milieu_composite, equation().milieu()).viscosite_dynamique().passe(),
                       *d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : nullptr,
                        *k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : nullptr,
                         *gravity = (equation().probleme().has_champ("gravite")) ? &equation().probleme().get_champ("gravite").valeurs() : nullptr ;

  const DoubleVect& dh_e = milc.diametre_hydraulique_elem();
  int e, i, j, k, l, n, m, N = inco.line_size(), Nk = (k_turb) ? (*k_turb).line_size() : 0, d, D = dimension, nf_tot = domaine.nb_faces_tot(), cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Np = press.line_size();
  double a_eps = alpha_res_, a_eps_min = alpha_res_min_, a_m, a_max; //seuil de declenchement du traitement de l'evanescence
  Matrice_Morse& mat_diag = *matrices.at(ch.le_nom().getString());

  if (N == 1) return; //pas d'evanescence en simple phase!

  DoubleTab dvr_elem(domaine.nb_elem_tot()*dimension, N, N, N); // Derivee de vr(n,k) en e par rapport a la phase l selon d ; pour l'instant toujours selon d2=d
  // On se le trimballe parce que quelqu'un a separe la boucle sur les matrices de celle sur le secmem


  /* calcul de la vitesse de derive : on va chercher les quantites intermediaires requises */
  Vitesse_relative_base::input_t in;
  Vitesse_relative_base::output_t out;
  out.vr.resize(N, N, D), out.dvr.resize(N, N, D, N*D);
  const Vitesse_relative_base* correlation_vd = pbm.has_correlation("vitesse_relative") ? &ref_cast(Vitesse_relative_base, pbm.get_correlation("vitesse_relative").valeur()) : nullptr;
  DoubleTab gradAlpha, vort, nut;
  const int is_turb = ref_cast(Operateur_Diff_base, ref_cast(QDM_Multiphase, pbm.equation_qdm()).operateur_diff().l_op_base()).is_turb();
  if (correlation_vd)
    {
      in.alpha.resize(N), in.rho.resize(N), in.mu.resize(N), in.d_bulles.resize(N), in.k.resize(N), in.nut.resize(N), in.v.resize(D, N), in.sigma.resize(N*(N-1)/2), in.g.resize(D);
      if (correlation_vd->needs_grad_alpha())
        {
          gradAlpha.resize(domaine.nb_elem_tot(), D, N);
          calc_grad_alpha_elem(gradAlpha);
          in.gradAlpha.resize(D, N);
        }
      if (correlation_vd->needs_vort())
        {
          vort.resize(domaine.nb_elem_tot(), D, N);
          calc_vort_elem(vort);
          in.vort.resize(D, N);
        }
      if (is_turb)
        {
          nut.resize(domaine.nb_elem_tot(), N);
          ref_cast(Viscosite_turbulente_base, (*ref_cast(Operateur_Diff_base, equation().operateur(0).l_op_base()).correlation_viscosite_turbulente()).valeur()).eddy_viscosity(nut); //remplissage par la correlation
        }
    }

  for (e = 0; e < domaine.nb_elem_tot(); e++) /* elements : a faire D fois par element */
    {
      /* phase majoritaire : directement dans l'element */
      for (a_max = 0, k = -1, n = 0; n < N; n++)
        if ((a_m = alpha(e, n)) > a_max) k = n, a_max = a_m;
      if (k >= 0)
        for (i = nf_tot + D * e, d = 0; d < D; d++, i++) maj(i) = k;
      else abort();

      /* calcul de la vitesse de derive */
      if (correlation_vd)
        {
          in.dh = dh_e(e) ;
          for (n = 0; n < N; n++)
            {
              in.alpha(n) = alpha(e, n);
              in.rho(n) = rho(!cR * e, n);
              in.mu(n) = mu(!cM * e, n);
              in.d_bulles(n) = (d_bulles) ? (*d_bulles)(e, n) : -1. ;
              for (d = 0; d < D; d++) in.v(d, n) = inco(nf_tot + D * e + d, n);
              for (m = n+1; m < N; m++)
                if (milc.has_interface(n, m))
                  {
                    const int ind_trav = (n*(N-1)-(n-1)*(n)/2) + (m-n-1); // Et oui ! matrice triang sup !
                    Interface_base& sat = milc.get_interface(n, m);
                    in.sigma(ind_trav) = res_en_T ? sat.sigma(temp_ou_enth(e, n), press(e, n * (Np > 1))) : sat.sigma_h(temp_ou_enth(e, n), press(e, n * (Np > 1)));
                  }
            }
          for (n = 0; n < Nk; n++) in.k(n) = (k_turb) ? (*k_turb)(e, n) : -1., in.nut(n) = (is_turb) ? nut(e, n) : -1. ;
          for (d = 0; d < D; d++) in.g(d) = (*gravity)(e,d);
          if (correlation_vd->needs_grad_alpha())
            for (n = 0; n < N; n++)
              for (d = 0; d < D; d++) in.gradAlpha(d, n) = gradAlpha(e, d, n);
          if (correlation_vd->needs_vort())
            for (n = 0; n < N; n++)
              for (d = 0; d < D; d++) in.vort(d, n) = vort(e, d, n);

          correlation_vd->vitesse_relative(in, out);
        }

      /* coeff d'evanescence */
      for (i = nf_tot + D * e, d = 0; d < D; d++, i++)
        for (n = 0; n < N; n++)
          if (n != k && (a_m = alpha(e, n)) < a_eps)
            {
              coeff(i, n, 1) = mat_diag(N * i + k, N * i + k) * (coeff(i, n, 0) = std::min(std::max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
              for (l=0 ; l<N ; l++) dvr_elem(D*e+d, n, k, l) = out.dvr(n, k, d, l*D+d) ;
              double flux = coeff(i, n, 0) * secmem(i, n) + coeff(i, n, 1) * (inco(i, n) - inco(i, k) - out.vr(n, k, d));
              secmem(i, k) += flux, secmem(i, n) -= flux;
            }
    }

  /* lignes de matrices */
  for (auto &&n_m : matrices)
    if (n_m.second->nb_colonnes())
      {
        int diag = (n_m.first == ch.le_nom().getString()); //est-on sur le bloc diagonal?
        Matrice_Morse& mat = *n_m.second;
        for (e = 0, l = nf_tot; e < domaine.nb_elem_tot(); e++) /* elements : l est l'indice de ligne */
          for (d = 0; d < D; d++, l++)
            for (n = 0; n < N; n++)
              if (coeff(l, n, 0))
                for (k = maj(l), i = mat.get_tab1()(N * l + n) - 1, j = mat.get_tab1()(N * l + k) - 1; i < mat.get_tab1()(N * l + n + 1) - 1; i++, j++)
                  {
                    assert(mat.get_tab2()(i) == mat.get_tab2()(j));
                    int c = diag * mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                    mat.get_set_coeff()(j) += coeff(l, n, 0) * mat.get_set_coeff()(i) - coeff(l, n, 1) * ((c == N * l + n) - (c == N * l + k));
                    mat.get_set_coeff()(i) += -coeff(l, n, 0) * mat.get_set_coeff()(i) + coeff(l, n, 1) * ((c == N * l + n) - (c == N * l + k));

                    mat.get_set_coeff()(j) +=  - coeff(l, n, 1) * ( - dvr_elem(D*e+d, n, k, n)*(c == N * l + n) - dvr_elem(D*e+d, n, k, k)*(c == N * l + k));
                    mat.get_set_coeff()(i) +=  + coeff(l, n, 1) * ( - dvr_elem(D*e+d, n, k, n)*(c == N * l + n) - dvr_elem(D*e+d, n, k, k)*(c == N * l + k));
                  }
      }
}

void Op_Evanescence_Homogene_PolyMAC_P0_Face::calc_grad_alpha_elem(DoubleTab& gradAlphaElem) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const DoubleTab& alpha = pbm.equation_masse().inconnue().passe();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();

  int N = alpha.line_size(), D = dimension, nf_tot = domaine.nb_faces_tot(), ne_tot = domaine.nb_elem_tot();;

  /* calculaiton of the gradient of alpha at the face */
  const Champ_Elem_PolyMAC_P0& ch_a = ref_cast(Champ_Elem_PolyMAC_P0, pbm.equation_masse().inconnue().valeur());
  DoubleTrav grad_f_a(nf_tot, N);
  ch_a.init_grad(0);
  const IntTab& fg_d = ch_a.fgrad_d, &fg_e = ch_a.fgrad_e;  // Tables utilisees dans domaine_PolyMAC_P0::fgrad pour le calcul du gradient
  const DoubleTab&  fg_w = ch_a.fgrad_w;
  const Conds_lim& cls_a = ch_a.domaine_Cl_dis().les_conditions_limites(); 		// conditions aux limites du champ alpha
  const IntTab&    fcl_a = ch_a.fcl();	// tableaux utilitaires sur les CLs : fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)

  for (int n = 0; n < N; n++)
    for (int f = 0; f < nf_tot; f++)
      {
        grad_f_a(f, n) = 0;
        for (int j = fg_d(f); j < fg_d(f+1) ; j++)
          {
            int e = fg_e(j);
            int f_bord;
            if ( (f_bord = e-ne_tot) < 0) //contribution d'un element
              grad_f_a(f, n) += fg_w(j) * alpha(e, n);
            else if (fcl_a(f_bord, 0) == 1 || fcl_a(f_bord, 0) == 2) //Echange_impose_base
              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Echange_impose_base, cls_a[fcl_a(f_bord, 1)].valeur()).T_ext(fcl_a(f_bord, 2), n) : 0;
            else if (fcl_a(f_bord, 0) == 4) //Neumann non homogene
              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Neumann_paroi      , cls_a[fcl_a(f_bord, 1)].valeur()).flux_impose(fcl_a(f_bord, 2), n) : 0;
            else if (fcl_a(f_bord, 0) == 6) // Dirichlet
              grad_f_a(f, n) += fg_w(j) * ref_cast(Dirichlet, cls_a[fcl_a(f_bord, 1)].valeur()).val_imp(fcl_a(f_bord, 2), n);
          }
      }

  /* Calcul du grad aux elems */
  for (int n = 0; n < N; n++)
    for (int e = 0; e < ne_tot; e++)
      for (int d = 0; d < D; d++)
        {
          gradAlphaElem(e, d, n) = 0 ;
          for (int j = 0, f; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
            gradAlphaElem(e, d, n) += (e == f_e(f, 0) ? 1 : -1) * fs(f) * (xv(f, d) - xp(e, d)) / ve(e) * grad_f_a(f, n);
        }
}

void Op_Evanescence_Homogene_PolyMAC_P0_Face::calc_grad_alpha_faces(DoubleTab& gradAlphaFaces) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const DoubleTab& alpha = pbm.equation_masse().inconnue().passe();
  const DoubleTab& n_f = domaine.face_normales(), &vf_dir = domaine.volumes_entrelaces_dir();
  const DoubleVect& vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv();

  int N = alpha.line_size(), D = dimension, ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot(), nf = domaine.nb_faces();

  DoubleTrav gradAlphaElem(ne_tot, D, N);

  /* calculaiton of the gradient of alpha at the face */
  const Champ_Elem_PolyMAC_P0& ch_a = ref_cast(Champ_Elem_PolyMAC_P0, pbm.equation_masse().inconnue().valeur());
  DoubleTrav grad_f_a(nf_tot, N);
  ch_a.init_grad(0);
  const IntTab& fg_d = ch_a.fgrad_d, &fg_e = ch_a.fgrad_e;  // Tables utilisees dans domaine_PolyMAC_P0::fgrad pour le calcul du gradient
  const DoubleTab&  fg_w = ch_a.fgrad_w;
  const Conds_lim& cls_a = ch_a.domaine_Cl_dis().les_conditions_limites(); 		// conditions aux limites du champ alpha
  const IntTab&    fcl_a = ch_a.fcl();	// tableaux utilitaires sur les CLs : fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)

  for (int n = 0; n < N; n++)
    for (int f = 0; f < nf_tot; f++)
      {
        grad_f_a(f, n) = 0;
        for (int j = fg_d(f); j < fg_d(f+1) ; j++)
          {
            int e = fg_e(j);
            int f_bord;
            if ( (f_bord = e-ne_tot) < 0) //contribution d'un element
              grad_f_a(f, n) += fg_w(j) * alpha(e, n);
            else if (fcl_a(f_bord, 0) == 1 || fcl_a(f_bord, 0) == 2) //Echange_impose_base
              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Echange_impose_base, cls_a[fcl_a(f_bord, 1)].valeur()).T_ext(fcl_a(f_bord, 2), n) : 0;
            else if (fcl_a(f_bord, 0) == 4) //Neumann non homogene
              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Neumann_paroi      , cls_a[fcl_a(f_bord, 1)].valeur()).flux_impose(fcl_a(f_bord, 2), n) : 0;
            else if (fcl_a(f_bord, 0) == 6) // Dirichlet
              grad_f_a(f, n) += fg_w(j) * ref_cast(Dirichlet, cls_a[fcl_a(f_bord, 1)].valeur()).val_imp(fcl_a(f_bord, 2), n);
          }
      }

  /* Calcul du grad aux elems */
  for (int n = 0; n < N; n++)
    for (int e = 0; e < ne_tot; e++)
      for (int d = 0; d < D; d++)
        for (int j = 0, f; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
          gradAlphaElem(e, d, n) += (e == f_e(f, 0) ? 1 : -1) * fs(f) * (xv(f, d) - xp(e, d)) / ve(e) * grad_f_a(f, n);

  /* Calcul du grad vectoriel aux faces */
  double scalGradElem=0.;
  int c, e;
  for (int n = 0; n < N; n++)
    for (int f = 0; f < nf; f++)
      {
        for (c=0 ; c<2 && (e = f_e(f, c)) >= 0; c++)
          for (int d = 0; d < D; d++)
            gradAlphaFaces(f, d, n) += vf_dir(f, c)/vf(f)*gradAlphaElem(e, d, n);
        scalGradElem=0;
        for (int d = 0; d < D; d++)
          scalGradElem += gradAlphaFaces(f, d, n)*n_f(f,d)/fs(f);
        for (int d = 0; d < D; d++)
          gradAlphaFaces(f, d, n) += (grad_f_a(f, n) - scalGradElem)*n_f(f,d)/fs(f);
      }
}

void Op_Evanescence_Homogene_PolyMAC_P0_Face::calc_vort_faces(DoubleTab& vort) const
{
  vort = 0;
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis().valeur());
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces();
  const DoubleTab& n_f = domaine.face_normales(), &vfd = domaine.volumes_entrelaces_dir();
  const DoubleTab& vorticite = equation().probleme().get_champ("vorticite").passe(),
                   &grad_v = equation().probleme().get_champ("gradient_vitesse").passe();
  int D = dimension, N = vort.dimension_tot(2), nf = domaine.nb_faces(), nf_tot= domaine.nb_faces_tot();
  int c, e, f, n, d, d2, n_l=0 ;
  DoubleTrav grad_l(D,D), scal_grad(D);

  for (f = 0 ; f<nf ; f++)
    for (c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
      for (n=0; n<N; n++)
        for (d=0; d<D; d++)
          vort(f, d, n) += vorticite(e, N*d+n)*vfd(f, c)/vf(f);
  if (D==3)
    {
      for (f = 0 ; f<nf ; f++)
        grad_l = 0; // we fill grad_l so that grad_l(d, d2) = du_d/dx_d2 by averaging between both elements
      for (d = 0 ; d<D ; d++)
        for (d2 = 0 ; d2<D ; d2++)
          for (c=0 ; c<2  && (e = f_e(f, c)) >= 0; c++)
            grad_l(d, d2) += vfd(f, c)/vf(f)*grad_v(nf_tot + D*e + d2 , n_l * D + d) ;
      //We replace the n_l components by the one calculated without interpolation to elements
      scal_grad = 0 ; // scal_grad(d) = grad(u_d).n_f
      for (d = 0 ; d<D ; d++)
        for (d2 = 0 ; d2<D ; d2++)
          scal_grad(d) += grad_l(d, d2)*n_f(f, d2)/fs(f);
      for (d = 0 ; d<D ; d++)
        for (d2 = 0 ; d2<D ; d2++)
          grad_l(d, d2) += (grad_v(f ,n_l*D+d) - scal_grad(d)) * n_f(f, d2)/fs(f);
      // We calculate the local vorticity using this local gradient
      vort(f, 0, n_l) = grad_l(2, 1) - grad_l(1, 2); // dUz/dy - dUy/dz
      vort(f, 1, n_l) = grad_l(0, 2) - grad_l(2, 0); // dUx/dz - dUz/dx
      vort(f, 2, n_l) = grad_l(1, 0) - grad_l(0, 1); // dUy/dx - dUx/dy
    }
}

void Op_Evanescence_Homogene_PolyMAC_P0_Face::calc_vort_elem(DoubleTab& vort) const
{
  const DoubleTab& vorticite = equation().probleme().get_champ("vorticite").valeurs();
  int D = dimension, N = vort.dimension_tot(2), ne_tot = vort.dimension_tot(0);
  int e, n, d ;
  for (e=0; e<ne_tot; e++)
    for (n=0; n<N; n++)
      for (d=0; d<D; d++)
        vort(e, d, n) = vorticite(e, N*d+n);
}

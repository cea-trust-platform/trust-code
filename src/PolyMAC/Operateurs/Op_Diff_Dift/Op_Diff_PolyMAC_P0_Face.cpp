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

#include <Op_Diff_PolyMAC_P0_Face.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Option_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Synonyme_info.h>
#include <Pb_Multiphase.h>
#include <Statistiques.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

extern Stat_Counter_Id diffusion_counter_;

Implemente_instanciable( Op_Diff_PolyMAC_P0_Face, "Op_Diff_PolyMAC_P0_Face|Op_Dift_PolyMAC_P0_Face_PolyMAC_P0", Op_Diff_PolyMAC_P0_base );
Add_synonym(Op_Diff_PolyMAC_P0_Face, "Op_Diff_PolyMAC_P0_var_Face");
Add_synonym(Op_Diff_PolyMAC_P0_Face, "Op_Dift_PolyMAC_P0_var_Face_PolyMAC_P0");

Sortie& Op_Diff_PolyMAC_P0_Face::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0_base::printOn(os); }

Entree& Op_Diff_PolyMAC_P0_Face::readOn(Entree& is) { return Op_Diff_PolyMAC_P0_base::readOn(is); }

void Op_Diff_PolyMAC_P0_Face::completer()
{
  Op_Diff_PolyMAC_P0_base::completer();

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().inconnue());
  if (le_champ_inco.non_nul())
    ch.init_auxiliary_variables(); // cas flica5 : ce n'est pas l'inconnue qui est utilisee, donc on cree les variables auxiliaires ici

  flux_bords_.resize(domaine.premiere_face_int(), dimension * ch.valeurs().line_size());
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyMAC_P0_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }
  porosite_e.ref(equation().milieu().porosite_elem());
  porosite_f.ref(equation().milieu().porosite_face());
}

double Op_Diff_PolyMAC_P0_Face::calculer_dt_stab() const
{
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue()).fcl();

  const DoubleTab& nf = domaine.face_normales(), &vfd = domaine.volumes_entrelaces_dir(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ?
                          &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr,
                          *a_r = sub_type(Pb_Multiphase, equation().probleme())
                                 ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().champ_conserve().passe()
                                 : (has_champ_masse_volumique() ? &get_champ_masse_volumique().valeurs() : nullptr); /* produit alpha * rho */

  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(),
                    &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes();

  update_nu();

  const int N = equation().inconnue().valeurs().line_size();
  double dt = 1e10;

  DoubleTrav flux(N);

  for (int e = 0; e < domaine.nb_elem(); e++)
    {
      flux = 0.;
      double vol = pe(e) * ve(e);

      for (int i = 0; i < e_f.dimension(1); i++)
        {
          const int f = e_f(e, i);
          if (f < 0) continue;

          if (!Option_PolyMAC::TRAITEMENT_AXI || (Option_PolyMAC::TRAITEMENT_AXI && fcl(f,0) != 2) )
            for (int n = 0; n < N; n++)
              {
                flux(n) += domaine.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
                if (fcl(f, 0) < 2)
                  vol = std::min(vol, pf(f) * vf(f) / vfd(f, e != f_e(f, 0)) * vf(f)); //cf. Op_Conv_EF_Stab_PolyMAC_P0_Face.cpp
              }
        }

      for (int n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 0.25) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = std::min(dt, vol * (a_r ? (*a_r)(e, n) : 1) / flux(n));
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyMAC_P0_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue());
  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco))
    return; //semi-implicite ou pas de bloc diagonal -> rien a faire

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl(), &equiv = domaine.equiv();
  const DoubleTab& nf = domaine.face_normales();
  const DoubleVect& fs = domaine.face_surfaces();

  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  const int N = ch.valeurs().line_size(), ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot(), D = dimension;

  IntTab stencil(0, 2), tpfa(0, N);

  domaine.creer_tableau_faces(tpfa);

  /* stencils du flux : ceux (reduits) de update_nu si nu constant ou scalaire, ceux (complets) du domaine sinon */
  update_phif(!nu_constant_); //si nu variable, stencil complet

  Cerr << "Op_Diff_PolyMAC_P0_Face::dimensionner() : ";

  for (int f = 0; f < domaine.nb_faces(); f++)
    if (fcl(f, 0) < 2)
      for (int i = 0; i < 2; i++)
        {
          const int e = f_e(f, i); /* op. aux faces : contrib de l'elem e */
          if (e < 0) continue;

          //recherche de i_f : indice de f dans l'element e
          int i_f = -1;
          for (int j = 0; j < e_f.dimension(1); j++)
            {
              const int fb = e_f(e, j);
              if (fb < 0) continue;

              if (fb == f)
                i_f = j;
            }

          //contribution de la diffusion a la face fb
          for (int j = 0; j < e_f.dimension(1); j++)
            {
              const int fb = e_f(e, j);
              if (fb < 0) continue;

              const int c = (e != f_e(fb, 0));
              for (int k = phif_d(fb); k < phif_d(fb + 1); k++)
                {
                  const int e_s = phif_e(k);

                  if (e_s >= ne_tot) continue; //bord -> pas de terme de matrice

                  const int fc = equiv(fb, c, i_f);
                  const int f_s = (e_s == e) ? f : fc;

                  if (fc >= 0 && fcl(f_s, 0) < 2) /* amont/aval si equivalence : operateur entre faces */
                    for (int n = 0; n < N; n++)
                      stencil.append_line(N * f + n, N * f_s + n);

                  /* sinon : elem -> face, avec un traitement particulier de e_s == e pour eviter les modes en echiquier dans la diffusion */
                  for (int d = 0; d < D; d++)
                    if (std::fabs(nf(f, d)) > 1e-6 * fs(f))
                      for (int n = 0; n < N; n++)
                        stencil.append_line(N * f + n, N * (nf_tot + D * e_s + d) + n);

                  if (e_s == e)
                    for (int n = 0; n < N; n++)
                      stencil.append_line(N * f + n, N * f + n);
                }

            }
        }

  for (int e = 0; e < ne_tot; e++)
    for (int i = 0; i < e_f.dimension(1); i++) /* aux elements : on doit traiter aussi les elems virtuels */
      {
        const int f = e_f(e, i);
        if (f < 0) continue;

        for (int j = phif_d(f); j < phif_d(f + 1); j++)
          {
            const int e_s = phif_e(j);
            if (e_s < ne_tot) //contrib d'un element
              for (int d = 0; d < D; d++)
                for (int n = 0; n < N; n++)
                  stencil.append_line(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * e_s + d) + n);
          }
      }

  tpfa = 1;

  for (int f = 0; f < domaine.nb_faces(); f++)
    for (int i = phif_d(f); i < phif_d(f + 1); i++)
      {
        const int e_s = phif_e(i);
        if (e_s < ne_tot && e_s != f_e(f, 0) && e_s != f_e(f, 1))
          for (int n = 0; n < N; n++)
            if (phif_c(i, n))
              tpfa(f, n) = 0;
      }

  tableau_trier_retirer_doublons(stencil);
  const double face_t = static_cast<double>(domaine.md_vector_faces()->nb_items_seq_tot()),
               elem_t = static_cast<double>(domaine.domaine().md_vector_elements()->nb_items_seq_tot());
  const double width = mp_sum_as_double(stencil.dimension(0)) / (N * (face_t + D * elem_t));
  const double perc = mp_somme_vect_as_double(tpfa) * 100. / (N * face_t);
  Cerr << "width " << width << " " << perc  << "% TPFA " << finl;
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), stencil, mat2);

  if (mat.nb_colonnes())
    mat += mat2;
  else
    mat = mat2;
}

void Op_Diff_PolyMAC_P0_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices[nom_inco] : nullptr; //facultatif

  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : le_champ_inco.non_nul() ? le_champ_inco->valeurs() : equation().inconnue().valeurs();

  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue());
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl(), &equiv = domaine.equiv();
  const DoubleVect& fs = domaine.face_surfaces(), &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes(), &pf = porosite_f, &pe = porosite_e;
  const DoubleTab& nf = domaine.face_normales(), &xp = domaine.xp(), &xv = domaine.xv(), &vfd = domaine.volumes_entrelaces_dir();

  const int N = inco.line_size(), D = dimension, ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot();

  update_phif();

  DoubleTrav coeff(N), fac(N);

  for (int f = 0; f < domaine.nb_faces(); f++)
    {
      if (fcl(f, 0) < 2)
        {
          for (int i = 0; i < 2; i++  )
            {
              int e = f_e(f, i);
              if (e < 0) continue;

              // Recherche de i_f : indice de la face f dans l'element e
              int i_f = -1;
              for (int j = 0; i_f < 0 && j < e_f.dimension(1); j++)
                {
                  const int fb = e_f(e, j);
                  if (fb < 0) continue;

                  if (fb == f)
                    i_f = j;
                }

              // Contribution de la diffusion a la face fb
              for (int j = 0; j < e_f.dimension(1); j++)
                {
                  int fb = e_f(e, j);
                  if (fb < 0) continue;

                  int tpfa = 1;
                  for (int k = phif_d(fb); k < phif_d(fb + 1); k++)
                    {
                      int e_s = phif_e(k);
                      if (e_s < ne_tot && e_s != f_e(fb, 0) && e_s != f_e(fb, 1))
                        tpfa = 0;
                    }

                  double df_sur_d = tpfa && fcl(fb, 0) ?
                                    std::max(std::fabs(domaine.dot(&xv(fb, 0), &nf(fb, 0), &xv(f, 0)) / domaine.dot(&xv(fb, 0), &nf(fb, 0), &xp(e, 0))), 1.0) : 1.0;

                  int c = (e != f_e(fb, 0));
                  for (int  n = 0; n < N; n++)
                    coeff[n] = vfd(f, i) / ve(e) * fs(fb) * (c ? 1 : -1) / df_sur_d;

                  for (int k = phif_d(fb); k < phif_d(fb + 1); k++)
                    {
                      for (int n = 0; n < N; n++)
                        fac[n] = coeff[n] * phif_c(k, n);

                      int e_s = phif_e(k);
                      int  fc = equiv(fb, c, i_f);

                      if (e_s >= ne_tot)
                        {
                          // Contribution d'un bord
                          int f_s = e_s - ne_tot;
                          if (fcl(f_s, 0) == 3 && sub_type(Dirichlet, cls[fcl(f_s, 1)].valeur()))
                            {
                              for (int d = 0; d < D; d++)
                                for (int n = 0; n < N; n++)
                                  secmem(f, n) -= fac[n] * ref_cast(Dirichlet, cls[fcl(f_s, 1)].valeur()).val_imp(fcl(f_s, 2), N * d + n) * nf(f, d) / fs(f);
                            }
                        }
                      else if (tpfa && fc >= 0)
                        {
                          // Amont/aval si equivalence : operateur entre faces
                          int f_s = (e_s == e) ? f : fc;
                          int sgn = (domaine.dot(&nf(f_s, 0), &nf(f, 0)) > 0) ? 1 : -1;
                          for (int n = 0; n < N; n++)
                            secmem(f, n) -= fac[n] * sgn * pf(f_s) / pe(e_s) * inco(f_s, n);

                          if (mat && fcl(f_s, 0) < 2)
                            for (int n = 0; n < N; n++)
                              (*mat)(N * f + n, N * f_s + n) += fac[n] * sgn * pf(f_s) / pe(e_s);
                        }
                      else
                        {
                          // Sinon : element -> face, avec traitement particulier
                          double f_eps = (e_s != e) ? 0 : tpfa && fcl(fb, 0) ? 1 : std::min(eps, 1000 * std::pow(vf(f) / fs(f), 2));

                          for (int d = 0; d < D; d++)
                            for (int n = 0; n < N; n++)
                              secmem(f, n) -= fac[n] * (1 - f_eps) * inco(nf_tot + D * e_s + d, n) * nf(f, d) / fs(f);

                          if (mat)
                            for (int d = 0; d < D; d++)
                              if (std::fabs(nf(f, d)) > 1e-6 * fs(f))
                                for (int n = 0; n < N; n++)
                                  (*mat)(N * f + n, N * (nf_tot + D * e_s + d) + n) += fac[n] * (1 - f_eps) * nf(f, d) / fs(f);

                          if (!f_eps) continue;

                          for (int n = 0; n < N; n++)
                            secmem(f, n) -= fac[n] * f_eps * inco(f, n);

                          if (mat)
                            for (int n = 0; n < N; n++)
                              (*mat)(N * f + n, N * f + n) += fac[n] * f_eps;
                        }
                    }
                }
            }
        }
    }

  for (int e = 0; e < ne_tot; e++)
    for (int i = 0; i < e_f.dimension(1); i++) /* aux elements : on doit traiter aussi les elems virtuels */
      {
        const int f = e_f(e, i);
        if (f < 0) continue;

        const int c = (e != f_e(f, 0));

        for (int j = phif_d(f); j < phif_d(f + 1); j++)
          {
            for (int n = 0; n < N; n++)
              coeff(n) = fs(f) * (c ? 1 : -1) * phif_c(j, n);

            const int e_s = phif_e(j);
            const int f_s = e_s - ne_tot;

            if (e_s < ne_tot) //contrib d'un element
              {
                for (int d = 0; d < D; d++)
                  for (int n = 0; n < N; n++)
                    secmem(nf_tot + D * e + d, n) -= coeff(n) * inco(nf_tot + D * e_s + d, n);

                if (mat)
                  for (int d = 0; d < D; d++)
                    for (int n = 0; n < N; n++)
                      (*mat)(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * e_s + d) + n) += coeff(n);
              }
            else if (fcl(f_s, 0) == 3 && sub_type(Dirichlet, cls[fcl(f_s, 1)].valeur())) //contrib d'un bord : seul Dirichlet contribue
              for (int d = 0; d < D; d++)
                for (int n = 0; n < N; n++)
                  secmem(nf_tot + D * e + d, n) -= coeff(n) * ref_cast(Dirichlet, cls[fcl(f_s, 1)].valeur()).val_imp(fcl(f_s, 2), N * d + n);
          }
      }

  statistiques().end_count(diffusion_counter_);
}

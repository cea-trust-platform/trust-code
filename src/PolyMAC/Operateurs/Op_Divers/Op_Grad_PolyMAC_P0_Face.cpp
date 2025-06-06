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

#include <Op_Grad_PolyMAC_P0_Face.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Masse_PolyMAC_P0_Face.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Neumann_sortie_libre.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Navier_Stokes_std.h>
#include <Pb_Multiphase.h>
#include <Probleme_base.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <Periodique.h>
#include <TRUSTTrav.h>

extern Stat_Counter_Id gradient_counter_;

Implemente_instanciable(Op_Grad_PolyMAC_P0_Face, "Op_Grad_PolyMAC_P0_Face", Op_Grad_PolyMAC_P0P1NC_Face);

Sortie& Op_Grad_PolyMAC_P0_Face::printOn(Sortie& s) const { return s << que_suis_je() ; }

Entree& Op_Grad_PolyMAC_P0_Face::readOn(Entree& s) { return s ; }

void Op_Grad_PolyMAC_P0_Face::completer()
{
  Operateur_Grad_base::completer();

  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, ref_domaine.valeur());

  /* besoin d'un joint de 1 */
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Grad_PolyMAC_P0_Face : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }

  if (sub_type(Navier_Stokes_std, equation()) && ref_cast(Navier_Stokes_std, equation()).has_grad_P())
    {
      Champ_Face_PolyMAC_P0& gradp = ref_cast(Champ_Face_PolyMAC_P0, ref_cast(Navier_Stokes_std, equation()).grad_P());
      gradp.init_auxiliary_variables();
    }
}

void Op_Grad_PolyMAC_P0_Face::update_grad(int full_stencil) const
{
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, ref_domaine.valeur());
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue());
  const DoubleTab& press = le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const int M = press.line_size();
  double t_past = equation().inconnue().recuperer_temps_passe();
  if (!full_stencil && (alp ? (last_gradp_ >= t_past) : (last_gradp_ != -DBL_MAX)))
    return; //deja calcule a ce temps -> rien a faire

  /* gradient */
  domaine.fgrad(M, 1, ref_dcl->les_conditions_limites(), ch.fcl(), nullptr, nullptr, 1, full_stencil, fgrad_d, fgrad_e, fgrad_c);
  last_gradp_ = t_past;
}

void Op_Grad_PolyMAC_P0_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, ref_domaine.valeur());
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue());
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& nf = domaine.face_normales(), &xp = domaine.xp(), &xv = domaine.xv();
  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const int ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot(), D = dimension, N = ch.valeurs().line_size(),
            M = (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()).line_size();

  update_grad(sub_type(Pb_Multiphase, equation().probleme())); //provoque le calcul du gradient

  IntTab sten_p(0, 2), sten_v(0, 2); //stencils (NS, pression), (NS, vitesse)

  const std::string& nom_inc = ch.le_nom().getString();
  Matrice_Morse *mat_p = matrices["pression"],
                 *mat_v = !semi_impl.count(nom_inc) && matrices.count(nom_inc) ? matrices.at(nom_inc) : nullptr,
                  mat2_p, mat2_v;

  std::map<int, std::set<int>> dpb_v, dgp_pb; //dependances vitesses -(dpb_v)-> pressions au bord -(dgp_pb)-> gradient

  if (mat_v)
    for (int f = 0; f < domaine.nb_faces_tot(); f++)
      if (fcl(f, 0) > 1)
        {
          const int e = f_e(f, 0);
          int m = 0;
          for (int n = 0; n < N; n++, m += (M > 1))
            {
              int i = nf_tot + D * e;
              for (int d = 0; d < D; d++, i++)
                if (std::fabs(nf(f, d)) > 1e-6 * fs(f))
                  {
                    dpb_v[M * f + m].insert(N * i + n);

                    for (int j = mat_v->get_tab1()(N * i + n) - 1; j < mat_v->get_tab1()(N * i + n + 1) - 1; j++)
                      dpb_v[M * f + m].insert(mat_v->get_tab2()(j) - 1);
                  }
            }
        }

  /* aux faces : gradient aux faces + remplissage de dgp_pb */
  std::vector<std::set<int>> dfgpf(N); //dfgpf[n][idx] = coeff : dependance en les pfb variables (presents dans dpf_ve)
  for (int f = 0; f < domaine.nb_faces_tot(); f++)
    {
      /* |f| grad p */
      for (int i = fgrad_d(f); i < fgrad_d(f + 1); i++) //face interne -> flux multipoints
        {
          const int e = fgrad_e(i);
          int m = 0;
          for (int n = 0; n < N; n++, m += (M > 1))
            {
              if (f < domaine.nb_faces() && e < ne_tot)
                sten_p.append_line(N * f + n, M * e + m);

              if (e >= ne_tot && dpb_v.count(M * (e - ne_tot) + m))
                dfgpf[n].insert(M * (e - ne_tot) + m);
            }
        }

      /* face -> vf(f) * phi grad p */
      if (fcl(f, 0) < 2 && f < domaine.nb_faces())
        {
          int m = 0;
          for (int n = 0; n < N; n++, m += (M > 1))
            for (auto &c : dfgpf[n])
              dgp_pb[N * f + n].insert(M * c + m);
        }

      /* elems amont/aval -> ve(e) * phi grad p */
      for (int i = 0; i < 2; i++)
        {
          const int e = f_e(f, i);
          if (e < 0) continue; // elem virt

          if (e < domaine.nb_elem())
            for (int d = 0; d < D; d++)
              if (fs(f) * std::fabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e))
                {
                  int m = 0;
                  for (int n = 0; n < N; n++, m += (M > 1))
                    for (auto &c : dfgpf[n])
                      dgp_pb[N * (nf_tot + D * e + d) + n].insert(M * c + m);
                }
        }

      for (int n = 0; n < N; n++)
        dfgpf[n].clear();
    }

  /* aux elements : gradient aux elems */
  for (int e = 0; e < domaine.nb_elem(); e++)
    for (int i = 0; i < e_f.dimension(1); i++)
      {
        const int f = e_f(e, i);
        if (f < 0) continue; // face in-existante

        for (int j = fgrad_d(f); j < fgrad_d(f + 1); j++)
          {
            const int eb = fgrad_e(j);
            if (eb < ne_tot)
              for (int d = 0; d < D; d++)
                if (std::fabs(fs(f) * (xv(f, d) - xp(e, d))) > 1e-6 * ve(e))
                  {
                    int m = 0;
                    for (int n = 0; n < N; n++, m += (M > 1))
                      sten_p.append_line(N * (nf_tot + D * e + d) + n, M * eb + m);
                  }
          }
      }

  /* sten_v en une ligne */
  for (auto &i_sc : dgp_pb)
    for (auto &c : i_sc.second)
      for (auto &k : dpb_v.at(c))
        sten_v.append_line(i_sc.first, k);

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten_p);
  tableau_trier_retirer_doublons(sten_v);

  if (mat_p)
    {
      Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), M * ne_tot, sten_p, mat2_p);

      if (mat_p->nb_colonnes())
        *mat_p += mat2_p;
      else
        *mat_p = mat2_p;
    }

  if (mat_v)
    {
      Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), sten_v, mat2_v);

      if (mat_v->nb_colonnes())
        *mat_v += mat2_v;
      else
        *mat_v = mat2_v;
    }
}

void Op_Grad_PolyMAC_P0_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(gradient_counter_);
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, ref_domaine.valeur());
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue());
  const Conds_lim& cls = ref_dcl->les_conditions_limites();
  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& nf = domaine.face_normales(), &xp = domaine.xp(), &xv = domaine.xv(), &vfd = domaine.volumes_entrelaces_dir(),
                   &press = semi_impl.count("pression") ? semi_impl.at("pression") : (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;

  const DoubleVect& fs = domaine.face_surfaces(), &ve = domaine.volumes(), &pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face();

  const int ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot(), D = dimension, N = secmem.line_size(), M = press.line_size();

  update_grad();

  const std::string& nom_inc = ch.le_nom().getString();
  Matrice_Morse *mat_p = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : nullptr, *mat_v =
                           !semi_impl.count(nom_inc) && matrices.count(nom_inc) ? matrices.at(nom_inc) : nullptr;

  DoubleTrav gb(nf_tot, M), gf(N), a_v(N); //-grad p aux bords , (grad p)_f, produit alpha * vol

  //std::map<int, std::map<int, double>> dgp_gb, dgb_v; //dependances vitesses -(dgb_v)-> -grad p aux bords -(dgp_gb)-> grad p ailleurs
  dgp_gb_.clear();
  dgb_v_.clear();

  for (int f = 0; f < domaine.nb_faces_tot(); f++)
    if (fcl(f, 0) > 1)  //Dirichlet/Symetrie : pression du voisin + correction en regardant l'eq de NS dans celui-ci
      {
        const int e = f_e(f, 0);
        int m = 0;
        for (int n = 0; n < N; n++, m += (M > 1))
          {
            double fac = 1. / (fs(f) * ve(e));
            std::map<int, double> *dv = mat_v ? &dgb_v_[M * f + m] : nullptr; //dv[indice dans mat_NS] = coeff
            int i = nf_tot + D * e;
            for (int d = 0; d < D; d++, i++)
              if (std::fabs(nf(f, d)) > 1e-6 * fs(f)) //boucle sur la direction : i est l'indice dans mat_v
                {
                  gb(f, m) += fac * nf(f, d) * secmem(i, n); //partie constante -> directement dans pfb

                  if (dv)
                    for (int j = mat_v->get_tab1()(N * i + n) - 1; j < mat_v->get_tab1()(N * i + n + 1) - 1; j++) //partie lineaire -> dans dgb_v
                      if (mat_v->get_coeff()(j))
                        (*dv)[mat_v->get_tab2()(j) - 1] -= fac * nf(f, d) * mat_v->get_coeff()(j);
                }
          }
      }

  for (int f = 0; f < domaine.nb_faces(); f++)
    if (fgrad_d(f + 1) == fgrad_d(f))
      abort();

  /* aux faces */
  //std::vector<std::map<int, double>> dgf_pe(N), dgf_gb(N); //dependance de [grad p]_f en les pressions aux elements, en les grad p aux faces de bord
  if (dgf_pe_.size()==0)
    {
      dgf_pe_.resize(N);
      dgf_gb_.resize(N);
    }

  for (int f = 0; f < domaine.nb_faces_tot(); f++)
    {
      for (int n = 0; n < N; n++)
        dgf_pe_[n].clear(), dgf_gb_[n].clear();

      a_v = 0.;

      for (int i = 0; i < 2; i++)
        {
          const int e = f_e(f, i);
          if (e < 0) continue;

          for (int n = 0; n < N; n++)
            a_v(n) += vfd(f, i) * (alp ? (*alp)(e, n) : 1);
        }

      /* |f| grad p */
      gf = 0.;
      for (int i = fgrad_d(f); i < fgrad_d(f + 1); i++)
        {
          const int e = fgrad_e(i);
          const int fb = e - ne_tot;
          int m = 0;

          for (int n = 0; n < N; n++, m += (M > 1))
            {
              const double fac = fgrad_c(i, m);
              const double val = (e < ne_tot ? press(e, m) : fcl(fb, 0) == 1 ? ref_cast(Neumann, cls[fcl(fb, 1)].valeur()).flux_impose(fcl(fb, 2), m) : gb(e - ne_tot, m));

              gf(n) += fac * val;

              if (mat_p && e < ne_tot)
                dgf_pe_[n].push_back({e, fac});

              if (mat_v && fb >= 0 && dgb_v_.count(M * fb + m))
                dgf_gb_[n].push_back({M * fb + m, fac});
            }
        }

      /* face -> vf(f) * phi grad p */
      if (fcl(f, 0) < 2 && f < domaine.nb_faces())
        {
          int m = 0;
          for (int n = 0; n < N; n++, m += (M > 1))
            {
              const double fac = a_v(n) * pf(f);
              secmem(f, n) -= fac * gf(n);

              for (auto &&i_c : dgf_pe_[n])
                (*mat_p)(N * f + n, M * i_c.first + m) += fac * i_c.second;

              for (auto &&i_c : dgf_gb_[n])
                dgp_gb_[N * f + n][M * i_c.first + m] += fac * i_c.second;
            }
        }

      /* elems amont/aval -> ve(e) * phi grad p */
      for (int i = 0; i < 2; i++)
        {
          const int e = f_e(f, i);
          if (e < 0) continue;

          if (e < domaine.nb_elem())
            for (int d = 0; d < D; d++)
              if (fs(f) * std::fabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e))
                {
                  int m = 0;
                  for (int n = 0; n < N; n++, m += (M > 1))
                    {
                      const double fac = (i ? -1 : 1) * fs(f) * pe(e) * (xv(f, d) - xp(e, d)) * (alp ? (*alp)(e, n) : 1);

                      secmem(nf_tot + D * e + d, n) -= fac * gf(n);

                      for (auto &i_c : dgf_pe_[n])
                        (*mat_p)(N * (nf_tot + D * e + d) + n, M * i_c.first + m) += fac * i_c.second;

                      for (auto &i_c : dgf_gb_[n])
                        dgp_gb_[N * (nf_tot + D * e + d) + n][M * i_c.first + m] += fac * i_c.second;
                    }
                }
        }
    }

  /* correction de mat_NS : en une seule ligne! */
  if (mat_v)
    for (auto &i_jc : dgp_gb_)
      for (auto &j_c : i_jc.second)
        if (dgb_v_.count(j_c.first))
          for (auto &k_d : dgb_v_.at(j_c.first))
            (*mat_v)(i_jc.first, k_d.first) += j_c.second * k_d.second;

  statistiques().end_count(gradient_counter_);
}

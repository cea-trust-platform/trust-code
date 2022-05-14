/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Grad_PolyMAC_V2_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_PolyMAC_V2_Face.h>
#include <Champ_Elem_PolyMAC_V2.h>
#include <Zone_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC_V2.h>
#include <Neumann_sortie_libre.h>
#include <Periodique.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <communications.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <TRUSTTrav.h>
#include <Masse_PolyMAC_V2_Face.h>
#include <Pb_Multiphase.h>
#include <cfloat>

Implemente_instanciable(Op_Grad_PolyMAC_V2_Face,"Op_Grad_PolyMAC_V2_Face",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_PolyMAC_V2_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_PolyMAC_V2_Face::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Grad_PolyMAC_V2_Face::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch)
{
  ref_zone = ref_cast(Zone_PolyMAC_V2, zone_dis.valeur());
  ref_zcl = ref_cast(Zone_Cl_PolyMAC, zone_cl_dis.valeur());
}

void Op_Grad_PolyMAC_V2_Face::completer()
{
  Operateur_Grad_base::completer();
  const Zone_PolyMAC_V2& zone = ref_zone.valeur();
  /* besoin d'un joint de 1 */
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Grad_PolyMAC_V2_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  Champ_Face_PolyMAC_V2& gradp = ref_cast(Champ_Face_PolyMAC_V2, ref_cast(Navier_Stokes_std, equation()).grad_P().valeur());
  gradp.init_auxiliary_variables();
  last_gradp_ = -DBL_MAX;
}

void Op_Grad_PolyMAC_V2_Face::update_grad(int full_stencil) const
{
  const Zone_PolyMAC_V2& zone = ref_zone.valeur();
  const Champ_Face_PolyMAC_V2& ch = ref_cast(Champ_Face_PolyMAC_V2, equation().inconnue().valeur());
  const DoubleTab& press = le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const int M = press.line_size();
  double t_past = equation().inconnue().valeur().recuperer_temps_passe();
  if (!full_stencil && (alp ? (last_gradp_ >= t_past) : (last_gradp_ != -DBL_MAX))) return; //deja calcule a ce temps -> rien a faire

  /* gradient */
  zone.fgrad(M, 1, ref_zcl->les_conditions_limites(), ch.fcl(), NULL, NULL, 1, full_stencil, fgrad_d, fgrad_e, fgrad_c);
  last_gradp_ = t_past;
}

void Op_Grad_PolyMAC_V2_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC_V2& zone = ref_zone.valeur();
  const Champ_Face_PolyMAC_V2& ch = ref_cast(Champ_Face_PolyMAC_V2, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes();
  int i, j, e, eb, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = ch.valeurs().line_size(),
                      m, M = (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()).line_size();
  update_grad(sub_type(Pb_Multiphase, equation().probleme())); //provoque le calcul du gradient

  IntTrav sten_p(0, 2), sten_v(0, 2); //stencils (NS, pression), (NS, vitesse)
  sten_p.set_smart_resize(1), sten_v.set_smart_resize(1);

  const std::string& nom_inc = ch.le_nom().getString();
  Matrice_Morse *mat_p = matrices["pression"], *mat_v = !semi_impl.count(nom_inc) && matrices.count(nom_inc) ? matrices.at(nom_inc) : NULL, mat2_p, mat2_v;
  std::map<int, std::set<int>> dpb_v, dgp_pb; //dependances vitesses -(dpb_v)-> pressions au bord -(dgp_pb)-> gradient
  if (mat_v) for (f = 0; f < zone.nb_faces_tot(); f++) if (fcl(f, 0) > 1) for (e = f_e(f, 0), n = 0, m = 0; n < N; n++, m += (M > 1))
          for (d = 0, i = nf_tot + D * e; d < D; d++, i++) if (std::fabs(nf(f, d)) > 1e-6 * fs(f))
              for (dpb_v[M * f + m].insert(N * i + n), j = mat_v->get_tab1()(N * i + n) - 1; j < mat_v->get_tab1()(N * i + n + 1) - 1; j++)
                dpb_v[M * f + m].insert(mat_v->get_tab2()(j) - 1);

  /* aux faces : gradient aux faces + remplissage de dgp_pb */
  std::vector<std::set<int>> dfgpf(N); //dfgpf[n][idx] = coeff : dependance en les pfb variables (presents dans dpf_ve)
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      /* |f| grad p */
      for (i = fgrad_d(f); i < fgrad_d(f + 1); i++) //face interne -> flux multipoints
        for (e = fgrad_e(i), n = 0, m = 0; n < N; n++, m += (M > 1))
          {
            if (f < zone.nb_faces() && e < ne_tot) sten_p.append_line(N * f + n, M * e + m);
            if (e >= ne_tot && dpb_v.count(M * (e - ne_tot) + m)) dfgpf[n].insert(M * (e - ne_tot) + m);
          }

      /* face -> vf(f) * phi grad p */
      if (fcl(f, 0) < 2 && f < zone.nb_faces()) for (n = 0, m = 0; n < N; n++, m += (M > 1)) for (auto &c : dfgpf[n]) dgp_pb[N * f + n].insert(M * c + m);
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++)
            if (fs(f) * std::fabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e)) for (n = 0, m = 0; n < N; n++, m += (M > 1)) for (auto &c : dfgpf[n])
                  dgp_pb[N * (nf_tot + D * e + d) + n].insert(M * c + m);
      for (n = 0; n < N; n++) dfgpf[n].clear();
    }

  /* aux elements : gradient aux elems */
  for (e = 0; e < zone.nb_elem(); e++) for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = fgrad_d(f); j < fgrad_d(f + 1); j++)
        if ((eb = fgrad_e(j)) < ne_tot) for (d = 0; d < D; d++)  if (std::fabs(fs(f) * (xv(f, d) - xp(e, d))) > 1e-6 * ve(e)) for (n = 0, m = 0; n < N; n++, m += (M > 1))
                sten_p.append_line(N * (nf_tot + D * e + d) + n, M * eb + m);

  /* sten_v en une ligne */
  for (auto &i_sc : dgp_pb) for (auto &c : i_sc.second) for (auto &k : dpb_v.at(c)) sten_v.append_line(i_sc.first, k);

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten_p), tableau_trier_retirer_doublons(sten_v);
  if (mat_p) Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), M * ne_tot               , sten_p, mat2_p), mat_p->nb_colonnes() ? *mat_p += mat2_p : *mat_p = mat2_p;
  if (mat_v) Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), sten_v, mat2_v), mat_v->nb_colonnes() ? *mat_v += mat2_v : *mat_v = mat2_v;
}

void Op_Grad_PolyMAC_V2_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC_V2& zone = ref_zone.valeur();
  const Champ_Face_PolyMAC_V2& ch = ref_cast(Champ_Face_PolyMAC_V2, equation().inconnue().valeur());
  const Conds_lim& cls = ref_zcl->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv(), &vfd = zone.volumes_entrelaces_dir(),
                   &press = semi_impl.count("pression") ? semi_impl.at("pression") : (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : ref_cast(Navier_Stokes_std, equation()).pression().valeurs()),
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &pe = zone.porosite_elem(), &pf = zone.porosite_face();
  int i, j, e, f, fb, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = secmem.line_size(), m, M = press.line_size();
  update_grad();
  const std::string& nom_inc = ch.le_nom().getString();
  Matrice_Morse *mat_p = !semi_impl.count("pression") && matrices.count("pression") ? matrices.at("pression") : NULL,
                 *mat_v = !semi_impl.count(nom_inc) && matrices.count(nom_inc) ? matrices.at(nom_inc) : NULL;

  DoubleTrav gb(nf_tot, M), gf(N), alpha(N); //-grad p aux bords , alpha |f| (grad p)_f
  std::map<int, std::map<int, double>> dgp_gb, dgb_v; //dependances vitesses -(dgb_v)-> -grad p aux bords -(dgp_gb)-> grad p ailleurs
  for (f = 0; f < zone.nb_faces_tot(); f++) if (fcl(f, 0) > 1)  //Dirichlet/Symetrie : pression du voisin + correction en regardant l'eq de NS dans celui-ci
      for (e = f_e(f, 0), n = 0, m = 0; n < N; n++, m += (M > 1))
        {
          double fac = 1. / (fs(f) * ve(e));
          std::map<int, double> *dv = mat_v ? &dgb_v[M * f + m] : NULL; //dv[indice dans mat_NS] = coeff
          for (d = 0, i = nf_tot + D * e; d < D; d++, i++) if (std::fabs(nf(f, d)) > 1e-6 * fs(f)) //boucle sur la direction : i est l'indice dans mat_v
              {
                gb(f, m) += fac * nf(f, d) * secmem(i, n); //partie constante -> directement dans pfb
                if (dv) for (j = mat_v->get_tab1()(N * i + n) - 1; j < mat_v->get_tab1()(N * i + n + 1) - 1; j++) //partie lineaire -> dans dgb_v
                    if (mat_v->get_coeff()(j)) (*dv)[mat_v->get_tab2()(j) - 1] -= fac * nf(f, d) * mat_v->get_coeff()(j);
              }
        }

  for (f = 0; f < zone.nb_faces(); f++) if (fgrad_d(f + 1) == fgrad_d(f)) abort();
  /* aux faces */
  std::vector<std::map<int, double>> dgf_pe(N), dgf_gb(N); //dependance de [grad p]_f en les pressions aux elements, en les grad p aux faces de bord
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      for (alpha = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (n = 0; n < N; n++) alpha(n) += vfd(f, i) * (alp ? (*alp)(e, n) : 1);

      /* |f| grad p */
      for (gf = 0, i = fgrad_d(f); i < fgrad_d(f + 1); i++)
        for (e = fgrad_e(i), fb = e - ne_tot, n = 0, m = 0; n < N; n++, m += (M > 1))
          {
            double fac = fgrad_c(i, m);
            gf(n) += fac * (e < ne_tot ? press(e, m) : fcl(fb, 0) == 1 ? ref_cast(Neumann, cls[fcl(fb, 1)].valeur()).flux_impose(fcl(fb, 2), m) : gb(e - ne_tot, m));
            if (mat_p && e < ne_tot) dgf_pe[n][e] += fac;
            if (mat_v && fb >= 0 && dgb_v.count(M * fb + m)) dgf_gb[n][M * fb + m] += fac;
          }

      /* face -> vf(f) * phi grad p */
      if (fcl(f, 0) < 2 && f < zone.nb_faces()) for (n = 0, m = 0; n < N; n++, m += (M > 1))
          {
            double fac = alpha(n) * pf(f);
            secmem(f, n) -= fac * gf(n);
            for (auto &&i_c : dgf_pe[n]) (*mat_p)(N * f + n, M * i_c.first + m) += fac * i_c.second;
            for (auto &&i_c : dgf_gb[n]) dgp_gb[N * f + n][M * i_c.first + m] += fac * i_c.second;
          }
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++)
            if (fs(f) * std::fabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e)) for (n = 0, m = 0; n < N; n++, m += (M > 1))
                {
                  double fac = (i ? -1 : 1) * fs(f) * pe(e) * (xv(f, d) - xp(e, d)) * (alp ? (*alp)(e, n) : 1);
                  secmem(nf_tot + D * e + d, n) -= fac * gf(n);
                  for (auto &i_c : dgf_pe[n]) (*mat_p)(N * (nf_tot + D * e + d) + n, M * i_c.first + m) += fac * i_c.second;
                  for (auto &i_c : dgf_gb[n]) dgp_gb[N * (nf_tot + D * e + d) + n][M * i_c.first + m] += fac * i_c.second;
                }
      for (n = 0; n < N; n++) dgf_pe[n].clear(), dgf_gb[n].clear();
    }

  /* correction de mat_NS : en une seule ligne! */
  if (mat_v) for (auto &i_jc : dgp_gb) for (auto &j_c : i_jc.second) if (dgb_v.count(j_c.first)) for (auto &k_d : dgb_v.at(j_c.first))
            (*mat_v)(i_jc.first, k_d.first) += j_c.second * k_d.second;
}

int Op_Grad_PolyMAC_V2_Face::impr(Sortie& os) const
{
  return 0;
}

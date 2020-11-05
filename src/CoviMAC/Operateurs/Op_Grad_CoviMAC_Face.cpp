/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Op_Grad_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_CoviMAC_Face.h>
#include <Champ_P0_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Neumann_sortie_libre.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Check_espace_virtuel.h>
#include <communications.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <DoubleTrav.h>
#include <Masse_CoviMAC_Face.h>
#include <Milieu_base.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Op_Grad_CoviMAC_Face,"Op_Grad_CoviMAC_Face",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_CoviMAC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_CoviMAC_Face::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Grad_CoviMAC_Face::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch)
{
  ref_zone = ref_cast(Zone_CoviMAC, zone_dis.valeur());
  ref_zcl = ref_cast(Zone_Cl_CoviMAC, zone_cl_dis.valeur());
}

void Op_Grad_CoviMAC_Face::completer()
{
  Operateur_Grad_base::completer();
  /* dimensionnement de fgrad_c (gradient aux faces internes) */
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Equation_base& eq = equation();

  zone.init_feb();
  fgrad_d.resize(zone.nb_faces_tot() + 1), fgrad_j.resize(zone.feb_d(zone.nb_faces_tot()));
  fgrad_c.resize(zone.feb_d(zone.nb_faces_tot()), eq.inconnue().valeurs().line_size());
  zone.fgrad(zone.nb_faces_tot(), NULL, fgrad_d, fgrad_j, fgrad_c);

  /* besoin d'un joint de 1 */
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Grad_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
}

void Op_Grad_CoviMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes();
  int i, j, e, eb, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = ch.valeurs().line_size(),
                      m, M = ref_cast(Navier_Stokes_std, equation()).pression().valeurs().line_size();
  zone.init_ve(), zone.init_feb(), ch.init_cl();

  IntTrav sten_p(0, 2), sten_v(0, 2); //stencils (NS, pression), (NS, vitesse)
  sten_p.set_smart_resize(1), sten_v.set_smart_resize(1);

  const std::string& nom_inc = ch.le_nom().getString();
  Matrice_Morse *mat_p = matrices["pression"], *mat_v = !semi_impl.count(nom_inc) && matrices.count(nom_inc) ? matrices.at(nom_inc) : NULL, mat2_p, mat2_v;
  std::map<int, std::set<int>> dpb_v, dgp_pb; //dependances vitesses -(dpb_v)-> pressions au bord -(dgp_pb)-> gradient
  if (mat_v) for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0) > 1) for (e = f_e(f, 0), n = 0, m = 0; n < N; n++, m += (M > 1))
          for (d = 0, i = N * (nf_tot + D * e) + n; d < D; d++, i += N) if (dabs(nf(f, d)) > 1e-6 * fs(f))
              for (dpb_v[M * f + m].insert(i), j = mat_v->get_tab1()(i) - 1; j < mat_v->get_tab1()(i + 1) - 1; j++)
                dpb_v[M * f + m].insert(mat_v->get_tab2()(j) - 1);

  /* aux faces : gradient aux faces + remplissage de dgp_pb */
  std::vector<std::set<int>> dfgpf(N); //dfgpf[n][idx] = coeff : dependance en les pfb variables (presents dans dpf_ve)
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      /* |f| grad p */
      if (!ch.fcl(f, 0))
        {
          for (i = fgrad_d(f); i < fgrad_d(f + 1); i++) //face interne -> flux multipoints
            for (e = fgrad_j(i), n = 0, m = 0; n < N; n++, m += (M > 1))
              {
                if (f < zone.nb_faces()) sten_p.append_line(N * f + n, M * (e < ne_tot ? e : f_e(e - ne_tot, 0)) + m);
                if (e >= ne_tot && dpb_v.count(M * (e - ne_tot) + m)) dfgpf[n].insert(e - ne_tot);
              }
        }
      else for (e = f_e(f, 0), n = 0, m = 0; n < N; n++, m += (M > 1))
          {
            if (ch.fcl(f, 0) == 1 && f < zone.nb_faces()) sten_p.append_line(N * f + n, M * e + m);
            if (dpb_v.count(M * f + m)) dfgpf[n].insert(f);
          }

      /* face -> vf(f) * phi grad p */
      if (ch.fcl(f, 0) < 2) for (n = 0, m = 0; n < N; n++, m += (M > 1)) for (auto &c : dfgpf[n]) dgp_pb[N * f + n].insert(M * c + m);
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++)
            if (fs(f) * dabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e)) for (n = 0, m = 0; n < N; n++, m += (M > 1)) for (auto &c : dfgpf[n])
                  dgp_pb[N * (nf_tot + D * e + d) + n].insert(M * c + m);
      for (n = 0; n < N; n++) dfgpf[n].clear();
    }

  /* aux elements : gradient aux elems */
  for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++) for (f = zone.vej(i), j = fgrad_d(f); j < fgrad_d(f + 1); j++)
        for (eb = fgrad_j(j), d = 0; d < D; d++)  if (dabs(zone.vec(i, d)) > 1e-6) for (n = 0, m = 0; n < N; n++, m += (M > 1))
              sten_p.append_line(N * (nf_tot + D * e + d) + n, M * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + m);

  /* sten_v en une ligne */
  for (auto &i_sc : dgp_pb) for (auto &c : i_sc.second) for (auto &k : dpb_v.at(c)) sten_v.append_line(i_sc.first, k);

  /* allocation / remplissage */
  tableau_trier_retirer_doublons(sten_p), tableau_trier_retirer_doublons(sten_v);
  if (mat_p) Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), M * ne_tot               , sten_p, mat2_p), mat_p->nb_colonnes() ? *mat_p += mat2_p : *mat_p = mat2_p;
  if (mat_v) Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), sten_v, mat2_v), mat_v->nb_colonnes() ? *mat_v += mat2_v : *mat_v = mat2_v;
}

void Op_Grad_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = ref_zcl->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv(),
                   &press = ref_cast(Navier_Stokes_std, equation()).pression().valeurs(), &mu_f = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).mu_f,
                    *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().valeurs() : NULL;
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces();
  int i, j, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = secmem.line_size(), m, M = press.line_size();

  const std::string& nom_inc = ch.le_nom().getString();
  Matrice_Morse *mat_p = matrices["pression"], *mat_v = !semi_impl.count(nom_inc) && matrices.count(nom_inc) ? matrices.at(nom_inc) : NULL;

  DoubleTrav pfb(nf_tot, M), fgpf(N), alpha(N); // partie constante de la pression aux faces de bord, alpha |f| (grad p)_f
  std::map<int, std::map<int, double>> dgp_pb, dpb_v; //dependances vitesses -(dpb_v)-> pressions au bord -(dgp_pb)-> gradient
  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (ch.fcl(f, 0) == 1) for (m = 0; m < M; m++) //Neumann : valeur imposee
        pfb(f, m) = ref_cast(Neumann_val_ext, cls[ch.fcl(f, 1)].valeur()).flux_impose(ch.fcl(f, 2), m);
    else if (ch.fcl(f, 0) > 1)  //Dirichlet/Symetrie : pression du voisin + correction en regardant l'eq de NS dans celui-ci
      {
        for (e = f_e(f, 0), m = 0; m < M; m++) pfb(f, m) = press.addr()[M * e + m];
        for (n = 0, m = 0; n < N; n++, m += (M > 1)) if (!alp || M == 1 || alp->addr()[N * e + n] > 1e-8) //pas de correction dans la phase evanescente si 1 pression / phase
            {
              double fac = zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) / (fs(f) * fs(f) * ve(e) * (alp && M > 1 ? alp->addr()[N * e + n] : 1));
              std::map<int, double> *dv = mat_v ? &dpb_v[M * f + m] : NULL; //dv[indice dans mat_NS] = coeff
              for (d = 0, i = N * (nf_tot + D * e) + n; d < D; d++, i += N) if (dabs(nf(f, d)) > 1e-6 * fs(f)) //boucle sur la direction : i est l'indice dans mat_v
                  {
                    pfb(f, m) += fac * nf(f, d) * secmem.addr()[i]; //partie constante -> directement dans pfb
                    if (dv) for (j = mat_v->get_tab1()(i) - 1; j < mat_v->get_tab1()(i + 1) - 1; j++) //partie lineaire -> dans dpb_v
                        if (mat_v->get_coeff()(j)) (*dv)[mat_v->get_tab2()(j) - 1] -= fac * nf(f, d) * mat_v->get_coeff()(j);
                  }
            }
      }


  /* aux faces : deux points si bord, multipoints si interne */
  std::vector<std::map<int, double>> dfg_pe(N), dfg_pb(N); //dependance de [grad p]_f en les pressions aux elements, en les pressions au bord de dpb_v
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      //taux de vide pondere a la face
      for (i = 0, alpha = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (n = 0; n < N; n++)
          alpha(n) += (alp ? alp->addr()[N * e + n] : 1) * mu_f(f, n, i);
      /* |f| grad p */
      if (!ch.fcl(f, 0)) for (fgpf = 0, i = fgrad_d(f); i < fgrad_d(f + 1); i++) //face interne -> flux multipoints
          for (e = fgrad_j(i), n = 0, m = 0; n < N; n++, m += (M > 1))
            {
              fgpf(n) += fgrad_c(i, n) * (e < ne_tot ? press.addr()[M * e + m] : pfb(e - ne_tot, m));
              if (mat_p && (e < ne_tot || ch.fcl(e - ne_tot, 0) > 1)) dfg_pe[n][e < ne_tot ? e : f_e(e - ne_tot, 0)] += fgrad_c(i, n);
              if (mat_v && e >= ne_tot && dpb_v.count(M * (e - ne_tot) + m)) dfg_pb[n][e - ne_tot] += fgrad_c(i, n);
            }
      else for (e = f_e(f, 0), n = 0, m = 0; n < N; n++, m += (M > 1)) //face de bord -> flux a deux points
          {
            double fac = fs(f) * fs(f) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
            fgpf(n) = fac * (pfb(f, m) - press.addr()[M * e + m]);
            if (mat_p && ch.fcl(f, 0) == 1) dfg_pe[n][e] -= fac;
            if (mat_v && dpb_v.count(M * f + m)) dfg_pb[n][f] += fac;
          }

      /* face -> vf(f) * phi grad p */
      if (ch.fcl(f, 0) < 2) for (n = 0, m = 0; n < N; n++, m += (M > 1)) if (alpha(n) > 1e-8)
            {
              double fac = alpha(n) * vf(f) / fs(f);
              secmem.addr()[N * f + n] -= fac * fgpf(n);
              if (f < zone.nb_faces()) for (auto &&i_c : dfg_pe[n]) (*mat_p)(N * f + n, M * i_c.first + m) += fac * i_c.second;
              for (auto &&i_c : dfg_pb[n]) dgp_pb[N * f + n][M * i_c.first + m] += fac * i_c.second;
            }
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++)
            if (fs(f) * dabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e)) for (n = 0, m = 0; n < N; n++, m += (M > 1))
                {
                  double fac = (i ? -1 : 1) * (xv(f, d) - xp(e, d)) * (alp ? alp->addr()[N * e + n] : 1);
                  secmem.addr()[N * (nf_tot + D * e + d) + n] -= fac * fgpf(n);
                  for (auto &i_c : dfg_pe[n]) (*mat_p)(N * (nf_tot + D * e + d) + n, M * i_c.first + m) += fac * i_c.second;
                  for (auto &i_c : dfg_pb[n]) dgp_pb[N * (nf_tot + D * e + d) + n][M * i_c.first + m] += fac * i_c.second;
                }
      for (n = 0; n < N; n++) dfg_pe[n].clear(), dfg_pb[n].clear();
    }

  /* correction de mat_NS : en une seule ligne! */
  if (mat_v) for (auto &i_jc : dgp_pb) for (auto &j_c : i_jc.second) if (dpb_v.count(j_c.first)) for (auto &k_d : dpb_v.at(j_c.first))
            (*mat_v)(i_jc.first, k_d.first) += j_c.second * k_d.second;
}


int Op_Grad_CoviMAC_Face::impr(Sortie& os) const
{
  return 0;
}

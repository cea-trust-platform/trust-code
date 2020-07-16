/****************************************************************************
* Copyright (c) 2019, CEA
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
  /* dimensionnement de gradp_c (gradient aux faces internes) */
  const Zone_CoviMAC& zone = ref_zone.valeur();
  zone.init_feb();
  gradp_d.resize(zone.nb_faces_tot() + 1), gradp_j.resize(zone.feb_d(zone.nb_faces_tot()));
  gradp_c.resize(zone.feb_d(zone.nb_faces_tot()), equation().inconnue().valeurs().line_size());
  gradp_a_jour = 0;

  /* besoin d'un joint de 1 */
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Grad_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
}


/* mise a jour de gradp_c (gradient aux faces internes) */
void Op_Grad_CoviMAC_Face::update_gradp() const
{
  if (gradp_a_jour) return;
  const Zone_CoviMAC& zone = ref_zone.valeur();
  zone.flux(zone.nb_faces_tot(), ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e, gradp_d, gradp_j, gradp_c);
  gradp_a_jour = 1;
}

void Op_Grad_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, eb, f, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;

  zone.init_ve(), zone.init_feb(), ch.init_cl();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* stencil : toujours la version reduite (solveur_UP fait dimensionner() a chaque pas de temps) */
  update_gradp();

  /* aux faces : gradient aux faces */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) == 1) for (e = f_e(f, 0), n = 0; n < N; n++) stencil.append_line(N * f + n, N * e + n);
    else if (!ch.fcl(f, 0)) for (i = gradp_d(f); i < gradp_d(f + 1); i++) for (e = gradp_j(i), n = 0; n < N; n++)
          stencil.append_line(N * f + n, N * (e < ne_tot ? e : f_e(e - ne_tot, 0)) + n);

  /* aux elements : gradient aux elems */
  for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++) for (f = zone.vej(i), j = gradp_d(f); j < gradp_d(f + 1); j++)
        for (eb = gradp_j(j), d = 0; d < D; d++)  if (dabs(zone.vec(i, d)) > 1e-6) for (n = 0; n < N; n++)
              stencil.append_line(N * (nf_tot + D * e + d) + n, N * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + n);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * ne_tot, stencil, mat);
}

void Op_Grad_CoviMAC_Face::dimensionner_NS(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes();
  int i, j, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = ch.valeurs().line_size();
  update_gradp();

  std::map<int, std::set<int>> dgp_pf, dpf_ve;
  for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0) > 1) for (e = f_e(f, 0), n = 0; n < N; n++)
        for (d = 0, i = N * (nf_tot + D * e) + n; d < D; d++, i += N) if (dabs(nf(f, d)) > 1e-6 * fs(f))
            for (dpf_ve[N * f + n].insert(i), j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
              dpf_ve[N * f + n].insert(mat.get_tab2()(j) - 1);

  /* aux faces : deux points si bord, multipoints si interne */
  std::vector<std::set<int>> dfgpf(N); //dfgpf[n][idx] = coeff : dependance en les pfb variables (presents dans dpf_ve)
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      /* |f| grad p */
      if (!ch.fcl(f, 0))
        {
          for (i = gradp_d(f); i < gradp_d(f + 1); i++) //face interne -> flux multipoints
            for (e = gradp_j(i), n = 0; n < N; n++) if (e >= ne_tot && dpf_ve.count(N * (e - ne_tot) + n)) dfgpf[n].insert(e - ne_tot);
        }
      else for (e = f_e(f, 0), n = 0; n < N; n++) if (dpf_ve.count(N * f + n)) dfgpf[n].insert(f);

      /* face -> vf(f) * phi grad p */
      if (ch.fcl(f, 0) < 2) for (n = 0; n < N; n++) for (auto &c : dfgpf[n]) dgp_pf[N * f + n].insert(N * c + n);
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++)
            if (fs(f) * dabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e)) for (n = 0; n < N; n++) for (auto &c : dfgpf[n])
                  dgp_pf[N * (nf_tot + D * e + d) + n].insert(N * c + n);

      for (n = 0; n < N; n++) dfgpf[n].clear();
    }

  /* correction de mat_NS : en une seule ligne! */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  for (auto &i_sc : dgp_pf) for (auto &c : i_sc.second) for (auto &k : dpf_ve.at(c)) stencil.append_line(i_sc.first, k);
  tableau_trier_retirer_doublons(stencil);
  Matrice_Morse mat2;
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), stencil, mat2);
  mat += mat2;
}


DoubleTab& Op_Grad_CoviMAC_Face::ajouter_NS(const DoubleTab& inco, DoubleTab& resu, Matrice_Morse* mat_NS, const DoubleTab* inco_NS, const DoubleTab* secmem_NS) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = ref_zcl->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv(),
                   &W_e = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e, &inco_NS_p = equation().inconnue().valeurs();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces();
  int i, j, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = inco.line_size();
  double dt = equation().schema_temps().pas_de_temps();
  update_gradp();


  DoubleTrav pfb(nf_tot, N), fgpf(N); // partie constante de la pression aux faces de bord, |f| (grad p)_f
  std::map<int, std::map<int, double>> dgp_pf, dpf_ve; //correction de mat_NS pour la pression aux bords de Dirichlet : dependance de l'equation en pf, de pf en ve
  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (ch.fcl(f, 0) == 1) for (n = 0; n < N; n++) //Neumann : valeur imposee
        pfb(f, n) = ref_cast(Neumann, cls[ch.fcl(f, 1)].valeur()).flux_impose(ch.fcl(f, 2), n);
    else if (ch.fcl(f, 0) > 1) for (e = f_e(f, 0), n = 0; n < N; n++) //Dirichlet/Symetrie : pression du voisin + correction en regardant l'eq de NS dans celui-ci
        {
          pfb(f, n) = inco.addr()[N * e + n];
          if (!mat_NS) continue; //si appel par ajouter_NS, on peut corriger pfb
          double fac = zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) / zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / ve(e);
          auto& dve = dpf_ve[N * f + n]; //dve[indice dans mat_NS] = coeff
          for (d = 0, i = N * (nf_tot + D * e) + n; d < D; d++, i += N) if (dabs(nf(f, d)) > 1e-6 * fs(f)) //boucle sur la direction : i est l'indice dans mat_NS
              {
                pfb(f, n) += fac * nf(f, d) * (secmem_NS->addr()[i] - ve(e) * inco_NS_p.addr()[i] / dt); //partie constante -> directement dans pfb
                for (dve[i] += fac * nf(f, d) * ve(e) / dt, j = mat_NS->get_tab1()(i) - 1; j < mat_NS->get_tab1()(i + 1) - 1; j++) //partie lineaire -> dans dpf_ve
                  if (mat_NS->get_coeff()(j)) dve[mat_NS->get_tab2()(j) - 1] -= fac * nf(f, d) * mat_NS->get_coeff()(j);
              }
        }

  /* aux faces : deux points si bord, multipoints si interne */
  std::vector<std::map<int, double>> dfgpf(N); //dfgpf[n][idx] = coeff : dependance en les pfb variables (presents dans dpf_ve)
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      /* |f| grad p */
      if (!ch.fcl(f, 0)) for (fgpf = 0, i = gradp_d(f); i < gradp_d(f + 1); i++) //face interne -> flux multipoints
          for (e = gradp_j(i), n = 0; n < N; n++)
            {
              fgpf(n) += gradp_c(i, n) * (e < ne_tot ? inco.addr()[N * e + n] : pfb(e - ne_tot, n));
              if (e >= ne_tot && dpf_ve.count(N * (e - ne_tot) + n)) dfgpf[n][e - ne_tot] += gradp_c(i, n);
            }
      else for (e = f_e(f, 0), n = 0; n < N; n++) //face de bord -> flux a deux points
          {
            double fac = zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
            fgpf(n) = fac * (pfb(f, n) - inco.addr()[N * e + n]);
            if (dpf_ve.count(N * f + n)) dfgpf[n][f] += fac;
          }

      /* face -> vf(f) * phi grad p */
      if (ch.fcl(f, 0) < 2) for (n = 0; n < N; n++)
          {
            resu.addr()[N * f + n] += fgpf(n) * vf(f) / fs(f);
            for (auto &i_c : dfgpf[n]) dgp_pf[N * f + n][N * i_c.first + n] += vf(f) / fs(f) * i_c.second;
          }
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++)
            {
              for (n = 0; n < N; n++) resu.addr()[N * (nf_tot + D * e + d) + n] += (i ? -1 : 1) * (xv(f, d) - xp(e, d)) * fgpf(n);
              if (fs(f) * dabs(xv(f, d) - xp(e, d)) > 1e-6 * ve(e)) for (n = 0; n < N; n++) for (auto &i_c : dfgpf[n])
                    dgp_pf[N * (nf_tot + D * e + d) + n][N * i_c.first + n] += (i ? -1 : 1) * (xv(f, d) - xp(e, d)) * i_c.second;
            }
      for (n = 0; n < N; n++) dfgpf[n].clear();
    }

  /* correction de mat_NS : en une seule ligne! */
  if (mat_NS) for (auto &i_jc : dgp_pf) for (auto &j_c : i_jc.second) for (auto &k_d : dpf_ve.at(j_c.first))
          (*mat_NS)(i_jc.first, k_d.first) += j_c.second * k_d.second;

  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Op_Grad_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  return ajouter_NS(inco, resu, NULL, NULL, NULL);
}

DoubleTab& Op_Grad_CoviMAC_Face::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

DoubleTab& Op_Grad_CoviMAC_Face::calculer_NS(const DoubleTab& inco, DoubleTab& resu, Matrice_Morse* mat_NS, const DoubleTab* inco_NS, const DoubleTab* secmem_NS) const
{
  resu=0;
  return ajouter_NS(inco,resu, mat_NS, inco_NS, secmem_NS);
}

/* sert a appliquer une correction en pression -> n'agit que sur les vitesses aux faces, pas de variation des bords */
DoubleVect& Op_Grad_CoviMAC_Face::multvect(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv(),
                   &W_e = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e;
  const IntTab& f_e = zone.face_voisins();
  int i, e, f, ne_tot = zone.nb_elem_tot(), n, N = inco.line_size();
  update_gradp();

  /* aux faces : deux points si bord, multipoints si interne */
  DoubleTrav fgpf(N); // |f| (grad p)_f
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) < 2) //vf calcule seulement
      {
        /* |f| grad p */
        if (!ch.fcl(f, 0)) for (fgpf = 0, i = gradp_d(f); i < gradp_d(f + 1); i++) //face interne -> flux multipoints
            for (e = gradp_j(i), n = 0; n < N; n++) //au bord : les pressions aux CL de Neumann ne varient pas, celles aux CL de Dirichlet varient comme le voisin
              fgpf(n) += gradp_c(i, n) * (e < ne_tot || ch.fcl(e - ne_tot, 0) > 1 ? inco.addr()[N * (e < ne_tot ? e : f_e(e - ne_tot, 0)) + n] : 0);
        else for (e = f_e(f, 0), n = 0; n < N; n++) //bord de Neumann -> partie interne du flux a deux points
            fgpf(n) = - zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) * inco.addr()[N * e + n];

        /* face -> vf(f) * phi grad p */
        for (n = 0; n < N; n++) resu.addr()[N * f + n] = fgpf(n) * vf(f) / fs(f);
      }

  resu.echange_espace_virtuel();
  return resu;
}

void Op_Grad_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv(),
                   &W_e = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e;
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int i, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = inco.line_size();
  update_gradp();

  /* aux faces : deux points si bord, multipoints si interne */
  std::vector<std::map<int, double>> dfgpf(N); // |f| (grad p)_f, dfgpf[compo][indice] = coeff
  for (f = 0; f < zone.nb_faces_tot(); f++)
    {
      /* coefficients de |f| grad p */
      if (ch.fcl(f, 0) == 1) for (e = f_e(f, 0), n = 0; n < N; n++) //bords de Neumann -> partie interne du flux a 2 points
          dfgpf[n][N * e + n] = - zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
      else if (!ch.fcl(f, 0)) for (i = gradp_d(f); i < gradp_d(f + 1); i++) if ((e = gradp_j(i)) < ne_tot || ch.fcl(e - ne_tot, 0) > 1) //bord interne -> flux multipoints
            for (n = 0; n < N; n++) dfgpf[n][N * (e < ne_tot ? e : f_e(e - ne_tot, 0)) + n] += gradp_c(i, n);

      /* face -> vf(f) * phi grad p */
      if (ch.fcl(f, 0) < 2) for (n = 0; n < N; n++) for (auto &i_c : dfgpf[n]) matrice(N * f + n, i_c.first) -= i_c.second * vf(f) / fs(f);
      /* elems amont/aval -> ve(e) * phi grad p */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (d = 0; d < D; d++) if (fs(f) * dabs(xv(f, d) - xp(e, d)) > 1e-6 *  ve(e))
              for (n = 0; n < N; n++) for (auto &i_c : dfgpf[n])
                  matrice(N * (nf_tot + D * e + d) + n, i_c.first) -= (i ? -1 : 1) * (xv(f, d) - xp(e, d)) * i_c.second;
      for (n = 0; n < N; n++) dfgpf[n].clear();
    }
}

int Op_Grad_CoviMAC_Face::impr(Sortie& os) const
{
  return 0;
}

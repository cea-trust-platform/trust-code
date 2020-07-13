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
  zone.flux(zone.nb_faces_tot(), ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e, gradp_c);
  gradp_a_jour = 1;
}

void Op_Grad_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, eb, f, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;

  zone.init_ve(), zone.init_feb();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* aux faces : gradient aux faces */
  for (f = 0; f < zone.nb_faces(); f++) for (i = zone.feb_d(f); i < zone.feb_d(f + 1); i++) for (e = zone.feb_j(f), n = 0; n < N; n++)
        stencil.append_line(N * f + n, N * (e < ne_tot ? e : f_e(e - ne_tot, 0)) + n);

  /* aux elements : gradient aux elems */
  for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++) for (f = zone.vej(i), j = zone.feb_d(f); j < zone.feb_d(f + 1); j++)
        for (eb = zone.feb_j(j), d = 0; d < D; d++)  if (dabs(zone.vec(i, d)) > 1e-6) for (n = 0; n < N; n++)
              stencil.append_line(N * (nf_tot + D * e + d) + n, N * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + n);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * ne_tot, stencil, mat);
}

DoubleTab& Op_Grad_CoviMAC_Face::ajouter_NS(const DoubleTab& inco, DoubleTab& resu, const Matrice_Morse* mat_NS, const DoubleTab* inco_NS, const DoubleTab* secmem_NS) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = ref_zcl->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xp = zone.xp(), &xv = zone.xv(),
                   &W_e = ref_cast(Masse_CoviMAC_Face, equation().solv_masse().valeur()).W_e;
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces();
  int i, j, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension, n, N = inco.line_size();
  zone.init_ve(), update_gradp();

  /* pression aux faces de bord, |f| phi grad p aux faces */
  DoubleTrav pfb(zone.nb_faces_tot(), N), fgpf(zone.nb_faces_tot(), N);
  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (ch.fcl(f, 0) == 1) for (e = f_e(f, 0), n = 0; n < N; n++) //CL Neumann : pfb direct, gpf par flux a 2 points
        {
          pfb(f, n) = ref_cast(Neumann, cls[ch.fcl(f, 1)].valeur()).flux_impose(ch.fcl(f, 2), n);
          fgpf(f, n) = zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) * (pfb(f, n) - inco.addr()[N * e + n]);
        }
    else if (ch.fcl(f, 0) > 1) for (e = f_e(f, 0), n = 0; n < N; n++) //CL Dirichlet/Symetrie : pfb en regardant N-S, gpf par formule a 2 points
        {
          double residu[3] = { 0, }; //3 composantes du residu (secmem_NS - mat_NS.inco_NS) en e
          if (mat_NS) for (d = 0; d < D; d++) for (i = N * (nf_tot + D * e + d) + n, residu[d] = (*secmem_NS)(i), j = mat_NS->get_tab1()(i) - 1; j < mat_NS->get_tab1()(i + 1) - 1; j++)
                residu[d] -= mat_NS->get_coeff()(j) * (*inco_NS)(mat_NS->get_tab2()(j) - 1);
          fgpf(f, n) = zone.dot(residu, &nf(f, 0)) / ve(e);
          pfb(f, n) = inco.addr()[N * e + n] + fgpf(f, n) * zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) / zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0));
        }
  /* une fois qu'on a pfb : grad p aux autres faces */
  for (f = 0; f < zone.nb_faces(); f++) if (!ch.fcl(f, 0)) for (i = zone.feb_d(f); i < zone.feb_d(f + 1); i++) for (e = zone.feb_j(i), n = 0; n < N; n++)
          fgpf(f, n) += gradp_c(i, n) * (e < ne_tot ? inco.addr()[N * e + n] : pfb(e - ne_tot, n));

  /* contributions */
  //faces : gradient
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) < 2) for (n = 0; n < N; n++) resu.addr()[N * f + n] += fgpf(f, n) * vf(f) / fs(f);

  //elements : interp aux elems du gradient
  for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++) for (f = zone.vej(i), d = 0; d < D; d++) for (n = 0; n < N; n++)
          resu.addr()[N * (nf_tot + D * e + d) + n] += ve(e) * zone.vec(i, d) * fgpf(f, n) / fs(f);

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

DoubleTab& Op_Grad_CoviMAC_Face::calculer_NS(const DoubleTab& inco, DoubleTab& resu, const Matrice_Morse* mat_NS, const DoubleTab* inco_NS, const DoubleTab* secmem_NS) const
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
  zone.init_ve(), update_gradp();

  /* |f| phi grad p aux faces */
  DoubleTrav fgpf(zone.nb_faces(), N);
  for (f = 0; f < zone.nb_faces(); f++)  if (!ch.fcl(f, 0)) for (i = zone.feb_d(f); i < zone.feb_d(f + 1); i++) for (e = zone.feb_j(i), n = 0; n < N; n++)
          fgpf(f, n) += gradp_c(i, n) * (e < ne_tot || ch.fcl(e - ne_tot, 0) > 1 ? inco.addr()[N * (e < ne_tot ? e : f_e(e - ne_tot, 0)) + n] : 0);
    else if (ch.fcl(f, 0) == 1) for (e = f_e(f, 0), n = 0; n < N; n++) //bords de Neumann -> partie interne du flux a 2 poiints
        fgpf(f, n) = - zone.nu_dot(W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) * inco.addr()[N * e + n];

  /* contributions */
  resu = 0;
  //faces : gradient a la face
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++)
      resu.addr()[N * f + n] = fgpf(f, n) * vf(f) / fs(f);

  resu.echange_espace_virtuel();
  return resu;
}

void Op_Grad_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  // const Zone_CoviMAC& zone = ref_zone.valeur();
  // const IntTab& f_e = zone.face_voisins();
  // const DoubleTab& nf = zone.face_normales(), &vfd = zone.volumes_entrelaces_dir();
  // const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  // int i, j, k, l, r, e, f, fb, fc, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot();
  // zone.init_ve(), zone.init_w1();
  //
  // /* aux faces : on depend du gradient aux faces des elements voisins de f */
  // for (f = 0; f < zone.nb_faces(); f++)
  //   {
  //     /* gradient a la face */
  //     for (i = zone.w1d(f); i < zone.w1d(f + 1); i++) for (fb = zone.w1j(i), j = 0; j < 2; j++)
  //         matrice(f, f_e(fb, j)) += pf(f) * vf(f) * zone.w1c(i) * (j ? -1 : 1) * fs(fb) / vf(fb);
  //
  //     /* retrait de la projection du gradient aux elements */
  //     for (i = 0; i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++) for (j = zone.ved(e); j < zone.ved(e + 1); j++)
  //         if (dabs(zone.dot(&nf(f, 0), &zone.vec(j, 0))) > 1e-6 * fs(f))
  //           for (fb = zone.vej(j), k = zone.w1d(fb); k < zone.w1d(fb + 1); k++) for (fc = zone.w1j(k), l = 0; l < 2; l++)
  //               matrice(f, f_e(fc, l)) += pf(f) * vfd(f, i) * zone.dot(&nf(f, 0), &zone.vec(j, 0)) / fs(f) * zone.w1c(k) * (l ? 1 : -1) * fs(fc) / vf(fc);
  //   }
  //
  // /* aux elements : gradient aux elems */
  // for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++)
  //       if (dabs(zone.vec(i, r)) > 1e-6) for (f = zone.vej(i), j = zone.w1d(f); j < zone.w1d(f + 1); j++) for (fb = zone.w1j(j), k = 0; k < 2; k++)
  //             matrice(nf_tot + r * ne_tot + e, f_e(fb, k)) += pe(e) * ve(e) * zone.vec(i, r) * zone.w1c(j) * (k ? -1 : 1) * fs(fb) / vf(fb);
}

int Op_Grad_CoviMAC_Face::impr(Sortie& os) const
{
  return 0;
}

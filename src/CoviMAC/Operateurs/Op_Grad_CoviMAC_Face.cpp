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

void Op_Grad_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();
  int i, j, k, l, r, e, f, fb, fc, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot();

  /* dimensionner / contribuer a besoin de 2, mais ajouter n'a besoin que de 1 */
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Grad_CoviMAC_Face : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();

  zone.init_ve(), zone.init_w1();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* aux faces : on depend du gradient aux faces des elements voisins de f */
  for (f = 0; f < zone.nb_faces(); f++)
    {
      /* gradient a la face */
      for (i = zone.w1d(f); i < zone.w1d(f + 1); i++) for (fb = zone.w1j(i), j = 0; j < 2; j++)
          stencil.append_line(f, f_e(fb, j));
      /* retrait de la projection du gradient aux elements */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++)
        for (j = zone.ved(e); j < zone.ved(e + 1); j++)
          if (dabs(zone.dot(&nf(f, 0), &zone.vec(j, 0))) > 1e-6 * fs(f))
            for (fb = zone.vej(j), k = zone.w1d(fb); k < zone.w1d(fb + 1); k++)
              for (fc = zone.w1j(k), l = 0; l < 2; l++) stencil.append_line(f, f_e(fc, l));
    }

  /* aux elements : gradient aux elems */
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++)
        if (dabs(zone.vec(i, r)) > 1e-6) for (f = zone.vej(i), j = zone.w1d(f); j < zone.w1d(f + 1); j++)
            for (fb = zone.w1j(j), k = 0; k < 2; k++) stencil.append_line(nf_tot + r * ne_tot + e, f_e(fb, k));

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nf_tot + dimension * ne_tot, zone.nb_elems_faces_bord_tot(), stencil, mat);
}

DoubleTab& Op_Grad_CoviMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &vfd = zone.volumes_entrelaces_dir();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int i, r, e, f, fb, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot();
  zone.init_ve(), zone.init_w1();

  /* 1. gradient aux faces */
  DoubleTrav gradf(zone.nb_faces());
  for (f = 0; f < zone.nb_faces(); f++) for (i = zone.w1d(f); i < zone.w1d(f + 1); i++)
      fb = zone.w1j(i), gradf(f) += zone.w1c(i) * fs(fb) / vf(fb) * (inco(f_e(fb, 1)) - inco(f_e(fb, 0)));

  /* 2. gradient aux elements : avec espace virtuel */
  DoubleTrav grade(0, dimension);
  zone.zone().creer_tableau_elements(grade);
  for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++) for (r = 0; r < dimension; r++)
        grade(e, r) += zone.vec(i, r) * gradf(zone.vej(i));
  grade.echange_espace_virtuel();

  /* 3. contributions */
  //faces : gradient a la face - projections des gradients aux elements amont/aval
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0, resu(f) += pf(f) * vf(f) * gradf(f); i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++)
      resu(f) -= pf(f) * vfd(f, i) * zone.dot(&grade(e, 0), &nf(f, 0)) / fs(f);
  //elements : gradient
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++) resu(nf_tot + r * ne_tot + e) += pe(e) * ve(e) * grade(e, r);

  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Op_Grad_CoviMAC_Face::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

/* n'agit que sur les vitesses aux faces */
DoubleVect& Op_Grad_CoviMAC_Face::multvect(const DoubleTab& inco, DoubleTab& resu) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &pf = zone.porosite_face(), &ve = zone.volumes(), &pe = zone.porosite_elem();
  int i, r, e, f, fb, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot();
  zone.init_w1();

  DoubleTrav gradf(zone.nb_faces());
  for (f = 0; f < zone.nb_faces(); f++)
    {
      for (i = zone.w1d(f); i < zone.w1d(f + 1); i++)
        fb = zone.w1j(i), gradf(f) += zone.w1c(i) * fs(fb) / vf(fb) * (inco(f_e(fb, 1)) - inco(f_e(fb, 0)));
      resu(f) = pf(f) * vf(f) * gradf(f);
    }

  for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++) for (r = 0; r < dimension; r++)
        resu(nf_tot + ne_tot * r + e) += pe(e) * ve(e) * zone.vec(i, r) * gradf(zone.vej(i));

  resu.echange_espace_virtuel();
  return resu;
}

void Op_Grad_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone = ref_zone.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &vfd = zone.volumes_entrelaces_dir();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces(), &pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int i, j, k, l, r, e, f, fb, fc, nf_tot = zone.nb_faces_tot(), ne_tot = zone.nb_elem_tot();
  zone.init_ve(), zone.init_w1();

  /* aux faces : on depend du gradient aux faces des elements voisins de f */
  for (f = 0; f < zone.nb_faces(); f++)
    {
      /* gradient a la face */
      for (i = zone.w1d(f); i < zone.w1d(f + 1); i++) for (fb = zone.w1j(i), j = 0; j < 2; j++)
          matrice(f, f_e(fb, j)) += pf(f) * vf(f) * zone.w1c(i) * (j ? -1 : 1) * fs(fb) / vf(fb);

      /* retrait de la projection du gradient aux elements */
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++) for (j = zone.ved(e); j < zone.ved(e + 1); j++)
          if (dabs(zone.dot(&nf(f, 0), &zone.vec(j, 0))) > 1e-6 * fs(f))
            for (fb = zone.vej(j), k = zone.w1d(fb); k < zone.w1d(fb + 1); k++) for (fc = zone.w1j(k), l = 0; l < 2; l++)
                matrice(f, f_e(fc, l)) += pf(f) * vfd(f, i) * zone.dot(&nf(f, 0), &zone.vec(j, 0)) / fs(f) * zone.w1c(k) * (l ? 1 : -1) * fs(fc) / vf(fc);
    }

  /* aux elements : gradient aux elems */
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++) for (i = zone.ved(e); i < zone.ved(e + 1); i++)
        if (dabs(zone.vec(i, r)) > 1e-6) for (f = zone.vej(i), j = zone.w1d(f); j < zone.w1d(f + 1); j++) for (fb = zone.w1j(j), k = 0; k < 2; k++)
              matrice(nf_tot + r * ne_tot + e, f_e(fb, k)) += pe(e) * ve(e) * zone.vec(i, r) * zone.w1c(j) * (k ? -1 : 1) * fs(fb) / vf(fb);
}

int Op_Grad_CoviMAC_Face::impr(Sortie& os) const
{
  return 0;
}

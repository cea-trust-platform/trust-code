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
// File:        Masse_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_CoviMAC_Face.h>
#include <Equation_base.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Debog.h>

Implemente_instanciable(Masse_CoviMAC_Face,"Masse_CoviMAC_Face",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_CoviMAC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_CoviMAC_Face::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_CoviMAC_Face
//
//////////////////////////////////////////////////////////////

void Masse_CoviMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const IntTab& f_e = zone.face_voisins();
  int i, e, f, r, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* vitesses aux faces : diagonale - projection des ve */
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0, stencil.append_line(f, f); i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++)
      for (r = 0; r < dimension; r++) stencil.append_line(f, nf_tot + ne_tot * r + e);

  /* vitesses aux elements : diagonale */
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++)
      stencil.append_line(nf_tot + ne_tot * r + e, nf_tot + ne_tot * r + e);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nf_tot + dimension * ne_tot, nf_tot + dimension * ne_tot, stencil, mat);
}

DoubleTab& Masse_CoviMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int e, f, r, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  //vitesses aux faces
  for (f = 0; f < zone.nb_faces(); f++) sm(f) /= pf(f) * vf(f); //vitesse calculee

  //vitesses aux elements
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++)
      sm(nf_tot + ne_tot * r + e) /= pe(e) * ve(e);

  return sm;
}

DoubleTab& Masse_CoviMAC_Face::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& nf = zone.face_normales(), &vfd = zone.volumes_entrelaces_dir();
  int i, e, f, r, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  /* vitesses aux faces : diagonale - projection des ve */
  for (f = 0; f < zone.nb_faces(); f++)
    for (i = 0, secmem(f) += pf(f) * vf(f) * inco(f) / dt; i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++)
      for (r = 0; r < dimension; r++) secmem(f) -= pf(f) * vfd(f, i) * inco(nf_tot + ne_tot * r + e) * nf(f, r) / fs(f) / dt;

  /* vitesses aux elements : diagonale */
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++)
      secmem(nf_tot + ne_tot * r + e) += pe(e) * ve(e) * inco(nf_tot + ne_tot * r + e) / dt;

  return secmem;
}

Matrice_Base& Masse_CoviMAC_Face::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& nf = zone.face_normales(), &vfd = zone.volumes_entrelaces_dir();
  int i, e, f, r, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  /* vitesses aux faces : diagonale - projection des ve */
  for (f = 0; f < zone.nb_faces(); f++)
    for (i = 0, mat(f, f) += pf(f) * vf(f) / dt; i < 2 && (e = f_e(f, i)) >= 0 && e < ne_tot; i++)
      for (r = 0; r < dimension; r++) mat(f, nf_tot + ne_tot * r + e) -= pf(f) * vfd(f, i) * nf(f, r) / fs(f) / dt;

  /* vitesses aux elements : diagonale */
  for (r = 0; r < dimension; r++) for (e = 0; e < zone.nb_elem(); e++)
      mat(nf_tot + ne_tot * r + e, nf_tot + ne_tot * r + e) += pe(e) * ve(e) / dt;

  return matrice;
}

//sert a remettre en coherence la partie aux elements avec la partie aux faces
DoubleTab& Masse_CoviMAC_Face::corriger_solution(DoubleTab& x, const DoubleTab& y) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  ch.update_ve(x);
  return x;
}

//
void Masse_CoviMAC_Face::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC, la_zone_dis_base);
}

void Masse_CoviMAC_Face::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, la_zone_Cl_dis_base);
}


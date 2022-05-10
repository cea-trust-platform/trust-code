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
// File:        Force_Tchen_PolyMAC_V2.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/PolyMAC
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Force_Tchen_PolyMAC_V2.h>
#include <Pb_Multiphase.h>
#include <Champ_Face_PolyMAC_V2.h>
#include <Op_Grad_PolyMAC_V2_Face.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <math.h>

Implemente_instanciable(Force_Tchen_PolyMAC_V2, "Force_Tchen_Face_PolyMAC_V2|Tchen_force_Face_PolyMAC_V2", Source_base);

Sortie& Force_Tchen_PolyMAC_V2::printOn(Sortie& os) const
{
  return os;
}

Entree& Force_Tchen_PolyMAC_V2::readOn(Entree& is)
{
  //identification des phases
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  return is;
}

void Force_Tchen_PolyMAC_V2::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_V2& ch = ref_cast(Champ_Face_PolyMAC_V2, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(ch.le_nom().getString()), mat2;

  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();
  const IntTab& fcl = ch.fcl();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  int N = inco.line_size(), D = dimension, nf_tot = zone.nb_faces_tot(), nf = zone.nb_faces();

  /* faces */
  for (int f = 0; f < nf; f++) if (fcl(f, 0) < 2)
      for (int k = 0 ; k<N ; k++) if (k != n_l) //phase gazeuse
          stencil.append_line( N * f +  k , N * f + n_l) ;
  /* elements */
  for (int e = 0; e < zone.nb_elem_tot(); e++)
    for (int d = 0 ; d <D ; d++) for (int k = 0 ; k<N ; k++) if (k != n_l) //phase gazeuse
          stencil.append_line(  N *(nf_tot + D * e + d) + k  , N *(nf_tot + D * e + d) + n_l) ;

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;

}

void Force_Tchen_PolyMAC_V2::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Champ_Face_PolyMAC_V2& ch = ref_cast(Champ_Face_PolyMAC_V2, equation().inconnue().valeur());
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;

  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ch.fcl();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &ve = zone.volumes(), &vf = zone.volumes_entrelaces();

  const DoubleTab& inco = ch.valeurs(), &pvit = ch.passe(),
                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
                    &rho   = equation().milieu().masse_volumique().passe(),
                     &vf_dir = zone.volumes_entrelaces_dir();

  double pas_tps = equation().probleme().schema_temps().pas_de_temps();
  int N = inco.line_size(), D = dimension, nf_tot = zone.nb_faces_tot(), nf = zone.nb_faces();

  /* elements */
  for (int e = 0; e < zone.nb_elem_tot(); e++)
    for (int d = 0 ; d <D ; d++) for (int k = 0 ; k<N ; k++) if (k != n_l) //phase gazeuse
          {
            double fac = pe(e) * ve(e) * alpha(e, k) * rho(e, n_l) ;

            secmem(nf_tot + D * e + d, k) += fac * (inco(nf_tot + D * e + d, n_l)-pvit(nf_tot + D * e + d, n_l))/pas_tps;
            secmem(nf_tot + D * e + d, k) -= fac * (inco(nf_tot + D * e + d, n_l)-pvit(nf_tot + D * e + d, n_l))/pas_tps;
            if (mat)
              {
                (*mat)( N *(nf_tot + D * e + d) + k  , N *(nf_tot + D * e + d) + n_l) -= fac/pas_tps ;
                (*mat)( N *(nf_tot + D * e + d) + n_l, N *(nf_tot + D * e + d) + n_l) += fac/pas_tps ;
              }
          }

  /* faces */
  for (int f = 0; f < nf; f++) if (fcl(f, 0) < 2)
      for (int k = 0 ; k<N ; k++) if (k != n_l) //phase gazeuse
          {
            double alpha_loc = f_e(f, 1)>=0 ? alpha(f_e(f, 0), k) * vf_dir(f, 0)/vf(f) +  alpha(f_e(f, 1), k) * vf_dir(f, 1)/vf(f) : alpha(f_e(f, 0), k) ;
            double rho_loc   = f_e(f, 1)>=0 ? rho(f_e(f, 0), n_l) * vf_dir(f, 0)/vf(f) + rho(f_e(f, 1), n_l)  * vf_dir(f, 1)/vf(f) : rho(f_e(f, 0), n_l) ;
            double fac = pf(f) * vf(f) * alpha_loc * rho_loc ;

            secmem(f, k ) += fac * (inco(f, n_l)-pvit(f, n_l))/pas_tps;
            secmem(f,n_l) -= fac * (inco(f, n_l)-pvit(f, n_l))/pas_tps;
            if (mat)
              {
                (*mat)( N * f +  k , N * f + n_l) -= fac/pas_tps ;
                (*mat)( N * f + n_l, N * f + n_l) += fac/pas_tps ;
              }
          }

}

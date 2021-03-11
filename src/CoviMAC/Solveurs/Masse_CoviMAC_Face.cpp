/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <Option_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Piso.h>
#include <Vecteur3.h>
#include <Matrice33.h>
#include <Linear_algebra_tools_impl.h>
#include <Op_Grad_CoviMAC_Face.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>

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

DoubleTab& Masse_CoviMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int i, e, f, nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;
  const DoubleTab *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.champ_conserve().passe() : NULL,
                   &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f();
  double fac;

  //vitesses aux faces
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++)
      {
        for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += mu_f(f, n, i) * (a_r ? (*a_r)(e, n) : 1);
        sm(f, n) /= pf(f) * vf(f) * fac; //vitesse calculee
      }

  //vitesses aux elements
  for (e = 0; e < zone.nb_elem(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
        sm(nf_tot + D * e + d, n) /= pe(e) * ve(e) * (a_r ? (*a_r)(e, n) : 1);

  sm.echange_espace_virtuel();
  return sm;
}

void Masse_CoviMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inc), mat2;
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const DoubleTab& inco = equation().inconnue().valeurs();
  int i, e, f, nf_tot = zone.nb_faces_tot(), n, N = inco.line_size(), d, D = dimension;

  IntTrav sten(0, 2);
  sten.set_smart_resize(1);
  for (f = 0, i = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++, i++) sten.append_line(i, i); //faces reelles
  for (e = 0, i = N * nf_tot; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++, i++) sten.append_line(i, i); //elems reels
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Masse_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
  Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const Conds_lim& cls = la_zone_Cl_CoviMAC->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur()).fcl();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales(),
                   *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.champ_conserve().passe() : NULL,
                    &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f();
  int i, e, f, nf_tot = zone.nb_faces_tot(), n, N = inco.line_size(), d, D = dimension;

  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++)
      {
        double ar_f = 1;
        if (a_r && fcl(f, 0) < 2) for (ar_f = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            ar_f += mu_f(f, n, i) * (*a_r)(e, n);
        double fac = ar_f * pf(f) * vf(f) / dt;
        secmem(f, n) -= fac * resoudre_en_increments * inco(f, n);
        if (fcl(f, 0) < 2) secmem(f, n) += fac * passe(f, n);
        else if (fcl(f, 0) == 3) for (d = 0; d < D; d++)
            secmem(f, n) += fac * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n) * nf(f, d) / fs(f);
        if (mat) (*mat)(N * f + n, N * f + n) += fac;
      }

  for (e = 0, i = nf_tot; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++, i++) for (n = 0; n < N; n++)
        {
          double fac = pe(e) * ve(e) * (a_r ? (*a_r)(e, n) : 1) / dt;
          secmem(i, n) -= fac * (resoudre_en_increments * inco(i, n) - passe(i, n));
          if (mat) (*mat)(N * i + n, N * i + n) += fac;
        }

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

//sert a remettre en coherence la partie aux elements avec la partie aux faces
DoubleTab& Masse_CoviMAC_Face::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  Option_CoviMAC::interp_ve1 ? ch.update_ve(x) : ch.update_ve2(x, incr);
  return x;
}

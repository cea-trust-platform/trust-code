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
// File:        Masse_PolyMAC_P0_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Solveurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_PolyMAC_P0_Face.h>
#include <Equation_base.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC_P0.h>
#include <Option_PolyMAC_P0.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Dirichlet.h>
#include <Piso.h>
#include <Vecteur3.h>
#include <Matrice33.h>
#include <Linear_algebra_tools_impl.h>
#include <Op_Grad_PolyMAC_P0_Face.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Masse_ajoutee_base.h>
#include <TRUSTTab.h>

Implemente_instanciable(Masse_PolyMAC_P0_Face,"Masse_PolyMAC_P0_Face",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_PolyMAC_P0_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_PolyMAC_P0_Face::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_PolyMAC_P0_Face
//
//////////////////////////////////////////////////////////////

void Masse_PolyMAC_P0_Face::completer()
{
  Solveur_Masse_base::completer();
  Equation_base& eq = equation();
  Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, eq.inconnue().valeur());
  ch.init_auxiliary_variables();
}

DoubleTab& Masse_PolyMAC_P0_Face::appliquer_impl(DoubleTab& sm) const
{
  const Zone_PolyMAC_P0& zone = la_zone_PolyMAC_P0.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int i, e, f, nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;
  const DoubleTab *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.champ_conserve().passe() : NULL, &vfd = zone.volumes_entrelaces_dir();
  double fac;

  //vitesses aux faces
  for (f = 0; f < zone.nb_faces(); f++)
    for (n = 0; n < N; n++)
      {
        for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += vfd(f, i) / vf(f) * (a_r ? (*a_r)(e, n) : 1);
        sm(f, n) /= pf(f) * vf(f) * fac; //vitesse calculee
      }

  //vitesses aux elements
  if (sm.dimension_tot(0) > N * nf_tot)
    for (e = 0; e < zone.nb_elem(); e++)
      for (d = 0; d < D; d++)
        for (n = 0; n < N; n++)
          sm(nf_tot + D * e + d, n) /= pe(e) * ve(e) * (a_r ? (*a_r)(e, n) : 1);

  sm.echange_espace_virtuel();
  return sm;
}

void Masse_PolyMAC_P0_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inc), mat2;
  const Zone_PolyMAC_P0& zone = la_zone_PolyMAC_P0;
  const DoubleTab& inco = equation().inconnue().valeurs();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  int i, e, f, nf_tot = zone.nb_faces_tot(), m, n, N = inco.line_size(), d, D = dimension;

  IntTrav sten(0, 2);
  sten.set_smart_resize(1);
  for (f = 0, i = 0; f < zone.nb_faces(); f++)
    for (n = 0; n < N; n++, i++) //faces reelles
      if (corr)
        for (m = 0; m < N; m++) sten.append_line(i, N * f + m);
      else sten.append_line(i, i);
  for (e = 0, i = N * nf_tot; e < zone.nb_elem_tot(); e++)
    for (d = 0; d < D; d++)
      for (n = 0; n < N; n++, i++) //tous les elems (pour Op_Grad_PolyMAC_P0_Face)
        if (corr)
          for (m = 0; m < N; m++) sten.append_line(i, N * (nf_tot + D * e + d) + m);
        else sten.append_line(i, i);
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Masse_PolyMAC_P0_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
  Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
  const Zone_PolyMAC_P0& zone = la_zone_PolyMAC_P0;
  const Conds_lim& cls = la_Zone_Cl_PolyMAC->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur()).fcl();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &fs = zone.face_surfaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const DoubleTab& nf = zone.face_normales(), &rho = equation().milieu().masse_volumique().passe(),
                   *alpha = pbm ? &pbm->eq_masse.inconnue().passe() : NULL, *a_r = pbm ? &pbm->eq_masse.champ_conserve().passe() : NULL, &vfd = zone.volumes_entrelaces_dir();
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  int i, e, f, nf_tot = zone.nb_faces_tot(), m, n, N = inco.line_size(), d, D = dimension, cR = rho.dimension_tot(0) == 1;

  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  DoubleTrav masse(N, N), masse_e(N, N); //masse alpha * rho, contribution
  for (f = 0; f < zone.nb_faces(); f++) //faces reelles
    {
      if (!pbm || fcl(f, 0) >= 2)
        for (masse = 0, n = 0; n < N; n++) masse(n, n) = 1; //pas Pb_Multiphase ou CL -> pas de alpha * rho
      else for (masse = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (masse_e = 0, n = 0; n < N; n++) masse_e(n, n) = (*a_r)(e, n); //partie diagonale
            if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse_e); //partie masse ajoutee
            for (n = 0; n < N; n++)
              for (m = 0; m < N; m++) masse(n, m) += vfd(f, i) / vf(f) * masse_e(n, m); //contribution au alpha * rho de la face
          }
      for (n = 0; n < N; n++)
        {
          double fac = pf(f) * vf(f) / dt;
          for (m = 0; m < N; m++) secmem(f, n) -= fac * resoudre_en_increments * masse(n, m) * inco(f, m);
          if (fcl(f, 0) < 2)
            for (m = 0; m < N; m++) secmem(f, n) += fac * masse(n, m) * passe(f, m);
          else if (fcl(f, 0) == 3)
            for (d = 0; d < D; d++)
              secmem(f, n) += fac * masse(n, n) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n) * nf(f, d) / fs(f);
          if (mat)
            for (m = 0; m < N; m++)
              if (masse(n, m)) (*mat)(N * f + n, N * f + m) += fac * masse(n, m);
        }
    }

  for (e = 0, i = nf_tot; e < zone.nb_elem_tot(); e++) //tous les elems (pour Op_Grad_PolyMAC_P0_Face)
    {
      for (masse = 0, n = 0; n < N; n++) masse(n, n) = a_r ? (*a_r)(e, n) : 1; //partie diagonale
      if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse); //partie masse ajoutee
      for (d = 0; d < D; d++, i++)
        for (n = 0; n < N; n++)
          {
            double fac = pe(e) * ve(e) / dt;
            for (m = 0; m < N; m++) secmem(i, n) -= fac * masse(n, m) * (resoudre_en_increments * inco(i, m) - passe(i, m));
            if (mat)
              for (m = 0; m < N; m++)
                if (masse(n, m)) (*mat)(N * i + n, N * i + m) += fac * masse(n, m);
          }
    }
  i++;
}

//
void Masse_PolyMAC_P0_Face::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_PolyMAC_P0 = ref_cast(Zone_PolyMAC_P0, la_zone_dis_base);
}

void Masse_PolyMAC_P0_Face::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_Zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, la_zone_Cl_dis_base);
}

//sert a remettre en coherence la partie aux elements avec la partie aux faces
DoubleTab& Masse_PolyMAC_P0_Face::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  const Champ_Face_PolyMAC_P0& ch = ref_cast(Champ_Face_PolyMAC_P0, equation().inconnue().valeur());
  Option_PolyMAC_P0::interp_ve1 ? ch.update_ve(x) : ch.update_ve2(x, incr);
  return x;
}

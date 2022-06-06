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

#include <Masse_PolyMAC_Face.h>
#include <Equation_base.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
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
#include <Op_Grad_PolyMAC_Face.h>
#include <Champ_Uniforme.h>
#include <Pb_Multiphase.h>
#include <Masse_ajoutee_base.h>

Implemente_instanciable(Masse_PolyMAC_Face,"Masse_PolyMAC_Face",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_PolyMAC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_PolyMAC_Face::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_PolyMAC_Face
//
//////////////////////////////////////////////////////////////

DoubleTab& Masse_PolyMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face();
  int i, e, f, n, N = equation().inconnue().valeurs().line_size();
  const DoubleTab *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.champ_conserve().passe() : NULL,
                   &vfd = zone.volumes_entrelaces_dir();
  double fac;

  //vitesses aux faces
  for (f = 0; f < zone.nb_faces(); f++)
    for (n = 0; n < N; n++)
      {
        for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += vfd(f, i) * (a_r ? (*a_r)(e, n) : 1);
        sm(f, n) /= pf(f) * fac; //vitesse calculee
      }

  sm.echange_espace_virtuel();
  return sm;
}

void Masse_PolyMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inc = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inc)) return; //rien a faire
  Matrice_Morse& mat = *matrices.at(nom_inc), mat2;
  const Zone_PolyMAC& zone = la_zone_PolyMAC;
  const DoubleTab& inco = equation().inconnue().valeurs();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  int i, f, m, n, N = inco.line_size();

  IntTrav sten(0, 2);
  sten.set_smart_resize(1);
  for (f = 0, i = 0; f < zone.nb_faces(); f++)
    for (n = 0; n < N; n++, i++) //faces reelles
      if (corr)
        for (m = 0; m < N; m++) sten.append_line(i, N * f + m);
      else sten.append_line(i, i);

  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Masse_PolyMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
  Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
  const Zone_PolyMAC& zone = la_zone_PolyMAC;
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &vf = zone.volumes_entrelaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const DoubleTab& vfd = zone.volumes_entrelaces_dir(), &rho = equation().milieu().masse_volumique().passe(),
                   *alpha = pbm ? &pbm->eq_masse.inconnue().passe() : NULL, *a_r = pbm ? &pbm->eq_masse.champ_conserve().passe() : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  int i, e, f, m, n, N = inco.line_size(), cR = rho.dimension_tot(0) == 1;

  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  DoubleTrav masse(N, N), masse_e(N, N); //masse alpha * rho, contribution
  for (f = 0; f < zone.nb_faces(); f++) //faces reelles
    {
      if (!pbm)
        for (masse = 0, n = 0; n < N; n++) masse(n, n) = vf(f); //pas Pb_Multiphase ou CL -> pas de alpha * rho
      else for (masse = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (masse_e = 0, n = 0; n < N; n++) masse_e(n, n) = (*a_r)(e, n); //partie diagonale
            if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse_e); //partie masse ajoutee
            for (n = 0; n < N; n++)
              for (m = 0; m < N; m++) masse(n, m) += vfd(f, i) * masse_e(n, m); //contribution au alpha * rho de la face
          }
      for (n = 0; n < N; n++)
        {
          for (m = 0; m < N; m++) secmem(f, n) += pf(f) / dt * masse(n, m) * (passe(f, m) - resoudre_en_increments * inco(f, m));
          if (mat)
            for (m = 0; m < N; m++)
              if (masse(n, m)) (*mat)(N * f + n, N * f + m) += pf(f) / dt * masse(n, m);
        }
    }
}

//
void Masse_PolyMAC_Face::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_PolyMAC = ref_cast(Zone_PolyMAC, la_zone_dis_base);
}

void Masse_PolyMAC_Face::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, la_zone_Cl_dis_base);
}

//sert a imposer les CLs de Dirichlet en multiphase (ou la variation de P_bord ne permet de corriger que v_melange)
DoubleTab& Masse_PolyMAC_Face::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC;
  const Conds_lim& cls = la_zone_Cl_PolyMAC->les_conditions_limites();
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  const DoubleTab& nf = zone.face_normales(), &vit = equation().inconnue().valeurs();
  const DoubleVect& fs = zone.face_surfaces();
  int f, n, N = x.line_size(), d, D = dimension;

  for (f = 0; f < zone.nb_faces_tot(); f++)
    if (fcl(f, 0) == 2 || fcl(f, 0) == 4)
      for (n = 0; n < N; n++) x(f, n) = incr ? -vit(f, n) : 0; //Dirichlet homogene / Symetrie: on revient a 0
    else if (fcl(f, 0) == 3)
      for (n = 0; n < N; n++)
        for (x(f, n) = incr ? -vit(f, n) : 0, d = 0; d < D; d++) //Dirichlet : valeur de la CL
          x(f, n) += nf(f, d) / fs(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n);

  return x;
}

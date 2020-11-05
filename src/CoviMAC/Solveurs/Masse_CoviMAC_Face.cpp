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

void Masse_CoviMAC_Face::completer()
{
  /* dimensionnement et initialisation de mu_f */
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xv = zone.xv(), &xp = zone.xp();
  const DoubleVect& pe = zone.porosite_elem();
  const Equation_base& eq = equation();

  int i, e, f, n, N = eq.inconnue().valeurs().line_size();
  /* initialisation : de mu_f */
  mu_f.resize(0, N, 2), zone.creer_tableau_faces(mu_f);
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++)
      {
        double fac[2] = {0, }; //amont/aval
        if (f_e(f, 1) >= 0) for (i = 0; i < 2; i++) e = f_e(f, i), fac[i] = (i ? -1 : 1) * pe(e) * zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
        for (i = 0; i < 2; i++) mu_f(f, n, i) = f_e(f, 1) >= 0 ? fac[i] / (fac[0] + fac[1]) : (i == 0);
      }
}


DoubleTab& Masse_CoviMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int i, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension,
               cR = sub_type(Champ_Uniforme, equation().milieu().masse_volumique());
  const DoubleTab& rho = equation().milieu().masse_volumique().valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().valeurs() : NULL;
  double fac;

  //vitesses aux faces
  for (f = 0; f < nf_tot; f++) for (n = 0; n < N; n++)
      {
        for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += mu_f(f, n, i) * (alp ? alp->addr()[N * e + n] * rho(!cR * e, n) : 1);
        sm.addr()[N * f + n] /= pf(f) * vf(f) * fac; //vitesse calculee
      }

  //vitesses aux elements
  for (e = 0; e < ne_tot; e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
        sm.addr()[N * (nf_tot + D * e + d) + n] /= pe(e) * ve(e) * (alp ? alp->addr()[N * e + n] * rho(!cR * e, n) : 1);

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
  for (e = 0, i = N * nf_tot; e < zone.nb_elem(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++, i++) sten.append_line(i, i); //elems reels
  Matrix_tools::allocate_morse_matrix(inco.size_totale(), inco.size_totale(), sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Masse_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
  Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_CoviMAC->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &fs = zone.face_surfaces();
  const DoubleTab& nf = zone.face_normales(), &rho = equation().milieu().masse_volumique().passe(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL,
                    *coef = has_coefficient_temporel_ ? &equation().get_champ(name_of_coefficient_temporel_).valeurs() : NULL;
  int i, e, f, nf_tot = zone.nb_faces_tot(), n, N = inco.line_size(), d, D = dimension, cR = (rho.dimension_tot(0) == 1);

  /* faces : si CLs, pas de produit par alpha * rho en multiphase */
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++)
      {
        double a_m = 1, rho_m = 1;
        if (alp && ch.fcl(f, 0) < 2) for (a_m = rho_m = i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            a_m += mu_f(f, n, i) * alp->addr()[N * e + n], rho_m += mu_f(f, n, i) * rho(!cR * e, n);
        double fac = a_m * rho_m * (coef ? coef->addr()[N * f + n] : 1) * pf(f) * vf(f) / dt;
        secmem.addr()[N * f + n] -= fac * resoudre_en_increments * inco.addr()[N * f + n];
        if (ch.fcl(f, 0) < 2) secmem.addr()[N * f + n] += fac * passe.addr()[N * f + n];
        else if (ch.fcl(f, 0) == 3) for (d = 0; d < D; d++)
            secmem.addr()[N * f + n] += fac * ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), D * n + d) * nf(f, d) / fs(f);
        if (mat) (*mat)(N * f + n, N * f + n) += fac;
      }

  for (e = 0, i = N * nf_tot; e < zone.nb_elem(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++, i++)
        {
          double fac = (coef ? coef->addr()[i] : 1) * pe(e) * ve(e) * (alp ? rho(!cR * e, n) * alp->addr()[N * e + n] : 1) / dt;
          secmem.addr()[i] -= fac * (resoudre_en_increments * inco.addr()[i] - passe.addr()[i]);
          if (mat) (*mat)(i, i) += fac;
        }

}

Matrice_Base& Masse_CoviMAC_Face::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  DoubleTrav secmem(equation().inconnue().valeurs()); //sera jete
  ajouter_blocs({{ equation().inconnue().le_nom().getString(), &mat }}, secmem, dt, {}, 0);
  return matrice;
}

DoubleTab& Masse_CoviMAC_Face::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  ajouter_blocs({}, secmem, dt, {}, 0); //on ne resout pas en increments!
  return secmem;
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
DoubleTab& Masse_CoviMAC_Face::corriger_solution(DoubleTab& x, const DoubleTab& y) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  Option_CoviMAC::interp_ve1 ? ch.update_ve(x) : ch.update_ve2(x);
  return x;
}

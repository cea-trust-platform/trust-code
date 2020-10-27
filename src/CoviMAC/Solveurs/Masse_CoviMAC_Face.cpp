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
  /* dimensionnement et initialisation de W_e, mu_f */
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const IntTab& f_e = zone.face_voisins();
  const DoubleTab& nf = zone.face_normales(), &xv = zone.xv(), &xp = zone.xp();
  const DoubleVect& pe = zone.porosite_elem();
  const Equation_base& eq = equation();

  int i, e, f, n, N = eq.inconnue().valeurs().line_size(), ne_tot = zone.nb_elem_tot(), D = dimension;
  W_e.resize(0, N, D, D), zone.zone().creer_tableau_elements(W_e);
  mu_f.resize(0, N, 2), zone.creer_tableau_faces(mu_f);
  /* initialisation : W_e = Id et mu_f correspondant */
  for (e = 0; e < ne_tot; e++) for (n = 0; n < N; n++) for (i = 0; i < D; i++) W_e(e, n, i, i) = 1;
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++)
      {
        double fac[2] = {0, }; //amont/aval
        if (f_e(f, 1) >= 0) for (i = 0; i < 2; i++) e = f_e(f, i), fac[i] = (i ? -1 : 1) * pe(e) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
        for (i = 0; i < 2; i++) mu_f(f, n, i) = f_e(f, 1) >= 0 ? fac[!i] / (fac[0] + fac[1]) : (i == 0);
      }
}


DoubleTab& Masse_CoviMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  int e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;

  //vitesses aux faces
  for (f = 0; f < nf_tot; f++) for (n = 0; n < N; n++) sm.addr()[N * f + n] /= pf(f) * vf(f); //vitesse calculee

  //vitesses aux elements
  for (e = 0; e < ne_tot; e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
        sm.addr()[N * (nf_tot + D * e + d) + n] /= pe(e) * ve(e);

  return sm;
}

Matrice_Base& Masse_CoviMAC_Face::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  const Matrice_Morse& mat_const = ref_cast(Matrice_Morse, matrice); //pour appeler la version const de M_{ij}...
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  const DoubleTab& nf = zone.face_normales(), &xv = zone.xv(), &xp = zone.xp();
  int i, j, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d, D = dimension,
                  piso = sub_type(Piso, equation().schema_temps()) && !sub_type(Implicite, equation().schema_temps());

  /* aux elements : calcul de W_e, contribution a la diagonale */
  Matrice33 M(0, 0, 0, 0, 0, 0, 0, 0, D < 3), iM; //pour inversions
  for (e = 0; e < zone.nb_elem(); e++) for (n = 0; n < N; n++)
      {
        /* calcul et stockage de W_e */
        for (i = 0; i < D; i++) for (j = 0; j < D; j++)
            M(i, j) = (i == j) + (piso ? mat_const(N * (nf_tot + ne_tot * i + e) + n, N * (nf_tot + ne_tot * j + e) + n) * dt / (pe(e) * ve(e)) : 0);
        Matrice33::inverse(M, iM, 0);
        for (i = 0; i < D; i++) for (j = 0; j < D; j++) W_e(e, n, i, j) = pe(e) * iM(i, j);
      }
  W_e.echange_espace_virtuel();
  if (piso) ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient()).grad_a_jour = 0; //si PISO, on devra recalculer les coeffs du gradient
  for (e = 0; e < zone.nb_elem_tot(); e++)for (d = 0; d < D; d++) for (n = 0; n < N; n++)
        mat(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * e + d) + n) += pe(e) * ve(e) / dt;

  /* aux faces : calcul de mu_f, ligne "diagonale - projection des ve" */
  for (f = 0; f < zone.nb_faces(); f++) for (n = 0; n < N; n++) //vf calcule
      {
        /* calcul de mu_f */
        double fac[2] = {0, }; //amont/aval
        if (f_e(f, 1) >= 0) for (i = 0; i < 2; i++)
            e = f_e(f, i), fac[i] = (i ? -1 : 1) * zone.nu_dot(&W_e, e, n, N, &nf(f, 0), &nf(f, 0)) / zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0));
        for (i = 0; i < 2; i++) mu_f(f, n, i) = f_e(f, 1) >= 0 ? fac[!i] / (fac[0] + fac[1]) : (i == 0);

        /* contribution a mat */
        mat(N * f + n, N * f + n) += pf(f) * vf(f) / dt;
      }

  return matrice;
}

DoubleTab& Masse_CoviMAC_Face::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_CoviMAC->les_conditions_limites();
  const DoubleVect& pf = zone.porosite_face(), &fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &pe = zone.porosite_elem();
  const DoubleTab& nf = zone.face_normales();
  int e, f, nf_tot = zone.nb_faces_tot(), n, N = inco.line_size(), d, D = dimension;

  /* vitesses aux faces : diagonale */
  for (f = 0; f < zone.nb_faces(); f++) if (ch.fcl(f, 0) < 2) for (n = 0; n < N; n++) //vf calcule
        secmem.addr()[N * f + n] += pf(f) * vf(f) / dt * inco.addr()[N * f + n];
    else if (ch.fcl(f, 0) == 3) for (n = 0; n < N; n++) for (d = 0; d < D; d++) //Dirichlet
          secmem.addr()[N * f + n] += mu_f(f, n, 0) * pf(f) * vf(f) / dt * ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), D * n + d) * nf(f, d) / fs(f);

  /* vitesses aux elements : on part de la vitesse aux elements interpolee depuis la vitesse aux faces */
  for (e = 0; e < zone.nb_elem_tot(); e++) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
        secmem.addr()[N * (nf_tot + D * e + d) + n] += pe(e) * ve(e) / dt * inco.addr()[N * (nf_tot + D * e + d) + n];

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

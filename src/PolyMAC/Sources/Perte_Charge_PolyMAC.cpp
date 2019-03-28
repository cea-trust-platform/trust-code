/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Perte_Charge_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Champ_Uniforme.h>
#include <Sous_Zone.h>
#include <Champ_Face_PolyMAC.h>
#include <Check_espace_virtuel.h>
#include <Array_tools.h>
#include <Matrix_tools.h>

Implemente_base_sans_constructeur(Perte_Charge_PolyMAC,"Perte_Charge_PolyMAC",Source_base);

Perte_Charge_PolyMAC::Perte_Charge_PolyMAC():implicite_(1) { }

// printOn
//

Sortie& Perte_Charge_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << endl;
}

// readOn
//

Entree& Perte_Charge_PolyMAC::readOn(Entree& s )
{
  return s;
}

////////////////////////////////////////////////////////////////
//                                                            //
//             Fonction principale : ajouter                  //
//                                                            //
////////////////////////////////////////////////////////////////

void Perte_Charge_PolyMAC::dimensionner(Matrice_Morse& matrice) const
{
  const Zone_PolyMAC& zone = la_Zone_PolyMAC.valeur();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces_const();
  const Sous_Zone *pssz = sous_zone ? &la_sous_zone.valeur() : NULL;
  const IntTab& e_f = zone.elem_faces();
  int i, j, k, e, f, r, n_tot = zone.nb_faces_tot() + (dimension < 3 ? zone.zone().domaine().nb_som_tot() : zone.zone().nb_aretes_tot());
  DoubleVect pos(dimension), ve(dimension), dir(dimension);
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : zone.nb_elem_tot()); i++)
    for (j = 0, e = pssz ? (*pssz)[i] : i; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) if (f < zone.nb_faces())
        for (k = zone.vedeb(e); k < zone.vedeb(e + 1); k++)
          {
            double scal = zone.dot(&zone.veci(k, 0), &nf(f, 0)) / fs(f), proj[3];//projection orthogonale a nf de chaque coefficient
            for (r = 0; r < dimension; r++) proj[r] = zone.veci(k, r) - scal * nf(f, r) / fs(f);
            if (zone.dot(proj, proj) < 1e-16) continue;
            stencil.append_line(f, zone.veji(k));
          }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(n_tot, n_tot, stencil, matrice);
}

DoubleTab& Perte_Charge_PolyMAC::ajouter(DoubleTab& resu) const
{
  const Zone_PolyMAC& zone = la_Zone_PolyMAC.valeur();
  const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &nf = zone.face_normales(), &vit = la_vitesse->valeurs();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &fs = zone.face_surfaces_const();
  const Sous_Zone *pssz = sous_zone ? &la_sous_zone.valeur() : NULL;
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  int i, j, f, fb, r, C_nu = sub_type(Champ_Uniforme,nu.valeur()), C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur());
  double t = equation().schema_temps().temps_courant();
  DoubleVect pos(dimension), ve(dimension), dir(dimension);

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : zone.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (r = 0; r < dimension; r++) pos(r) = xp(e, r);

      /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
      double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0),
             dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      for (j = zone.vedeb(e), ve = 0; j < zone.vedeb(e + 1); j++) for (r = 0; r < dimension; r++)
          fb = zone.veji(j), ve(r) += zone.veci(j, r) * vit(fb) * pf(fb) / pe(e);
      double n_ve = sqrt(zone.dot(ve.addr(), ve.addr())), Re = max( n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;

      coeffs_perte_charge(ve, pos, t, n_ve , dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);
      double n2_dir = zone.dot(dir.addr(), dir.addr()); //si la fonction renvoie un vecteur non norme...

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) if (f < zone.nb_faces())
          {
            /* vecteur vitesse a la face : vf selon nf, ve selon les autres composantes */
            double scal = zone.dot(ve.addr(), &nf(f, 0)) / fs(f), vf[3],
                   fac = pe(e) * fs(f) * (e == f_e(f, 0) ? 1 : - 1);
            for (r = 0; r < dimension; r++) vf[r] = ve(r) + (vit(f) - scal) * nf(f, r) / fs(f);
            /* contribution */
            resu(f) -= fac * (           C_iso * zone.dot(&xv(f, 0), vf, &xp(e, 0))
                                         + (C_dir - C_iso) * zone.dot(vf, dir.addr()) * zone.dot(&xv(f, 0), dir.addr(), &xp(e, 0)) / n2_dir);
          }

    }
  return resu;
}

// Description: copie de ajouter sauf la derniere ligne
void Perte_Charge_PolyMAC::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_PolyMAC& zone = la_Zone_PolyMAC.valeur();
  const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &nf = zone.face_normales(), &vit = inco;
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &fs = zone.face_surfaces_const();
  const Sous_Zone *pssz = sous_zone ? &la_sous_zone.valeur() : NULL;
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  int i, j, k, f, fb, r, C_nu = sub_type(Champ_Uniforme,nu.valeur()), C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur());
  double t = equation().schema_temps().temps_courant();
  DoubleVect pos(dimension), ve(dimension), dir(dimension);

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : zone.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (r = 0; r < dimension; r++) pos(r) = xp(e, r);

      /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
      double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0),
             dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      for (j = zone.vedeb(e), ve = 0; j < zone.vedeb(e + 1); j++) for (r = 0; r < dimension; r++)
          fb = zone.veji(j), ve(r) += zone.veci(j, r) * vit(fb) * pf(fb) / pe(e);
      double n_ve = sqrt(zone.dot(ve.addr(), ve.addr())), Re = max( n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;

      coeffs_perte_charge(ve, pos, t, n_ve , dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);
      double n2_dir = zone.dot(dir.addr(), dir.addr()); //si la fonction renvoie un vecteur non norme...

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) if (f < zone.nb_faces())
          {
            /* vecteur vitesse a la face : vf selon nf, ve selon les autres composantes */
            double fac = pe(e) * fs(f) * (e == f_e(f, 0) ? 1 : - 1);
            /* partie "normale a la face" de vf */
            matrice(f, f) += fac * (C_iso * zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0))
                                    + (C_dir - C_iso) * zone.dot(&nf(f, 0), dir.addr()) * zone.dot(&xv(f, 0), dir.addr(), &xp(e, 0)) / n2_dir) / fs(f);
            /* partie ve -> pfft */
            for (k = zone.vedeb(e); k < zone.vedeb(e + 1); k++)
              {
                double scal = zone.dot(&zone.veci(k, 0), &nf(f, 0)) / fs(f), proj[3];//projection orthogonale a nf de chaque coefficient
                for (r = 0; r < dimension; r++) proj[r] = zone.veci(k, r) - scal * nf(f, r) / fs(f);
                if (zone.dot(proj, proj) < 1e-16) continue;
                fb = zone.veji(k);
                matrice(f, fb) += fac * pf(fb) / pe(e) * (C_iso * zone.dot(&xv(f, 0), proj, &xp(e, 0))
                                                          + (C_dir - C_iso) * zone.dot(proj, dir.addr()) * zone.dot(&xv(f, 0), dir.addr(), &xp(e, 0)) / n2_dir);
              }
          }
    }
}

DoubleTab& Perte_Charge_PolyMAC::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}


void Perte_Charge_PolyMAC::completer()
{
  Source_base::completer();
  if (sous_zone) la_sous_zone = equation().probleme().domaine().ss_zone(nom_sous_zone);
}

////////////////////////////////////////////////////////////////
//                                                            //
//         Fonctions virtuelles pures de Source_base          //
//                                                            //
////////////////////////////////////////////////////////////////


void Perte_Charge_PolyMAC::associer_pb(const Probleme_base& pb)
{
  la_vitesse = ref_cast(Champ_Face_PolyMAC,equation().inconnue().valeur());
  le_fluide = ref_cast(Fluide_Incompressible,equation().milieu());
}

void Perte_Charge_PolyMAC::associer_zones(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_Cl_dis)
{
  la_Zone_PolyMAC = ref_cast(Zone_PolyMAC, zone_dis.valeur());
  la_Zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, zone_Cl_dis.valeur());
}

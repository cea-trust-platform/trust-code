/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Perte_Charge_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Sous_Domaine.h>
#include <Champ_Face_PolyMAC.h>

Implemente_base_sans_constructeur(Perte_Charge_PolyMAC,"Perte_Charge_PolyMAC",Source_base);

Perte_Charge_PolyMAC::Perte_Charge_PolyMAC():implicite_(1) { }

Sortie& Perte_Charge_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << endl;
}

Entree& Perte_Charge_PolyMAC::readOn(Entree& s )
{
  return s;
}

DoubleTab& Perte_Charge_PolyMAC::ajouter(DoubleTab& resu) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vit = la_vitesse->valeurs();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &fs = domaine.face_surfaces();
  const Sous_Domaine *pssz = sous_domaine ? &la_sous_domaine.valeur() : NULL;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int i, j, k, f, fb, r, C_nu = sub_type(Champ_Uniforme,nu.valeur()), C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur());
  double t = equation().schema_temps().temps_courant();
  DoubleVect pos(dimension), ve(dimension), ved(dimension), vep(dimension) , dir(dimension);

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : domaine.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (r = 0; r < dimension; r++) pos(r) = xp(e, r);

      /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
      double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0),
             dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      for (j = domaine.vedeb(e), ve = 0; j < domaine.vedeb(e + 1); j++)
        for (r = 0; r < dimension; r++)
          fb = domaine.veji(j), ve(r) += domaine.veci(j, r) * vit(fb) * pf(fb) / pe(e);
      double n_ve = sqrt(domaine.dot(ve.addr(), ve.addr())), Re = std::max( n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;
      coeffs_perte_charge(ve, pos, t, n_ve, dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        if (f < domaine.nb_faces() && ch.fcl()(f, 0) < 2)
          {
            double m2vf = 0, contrib;
            for (k = domaine.m2i(domaine.m2d(e) + j); k < domaine.m2i(domaine.m2d(e) + j + 1); k++)
              fb = e_f(e, domaine.m2j(k)), m2vf += pf(f) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * domaine.volumes(e) * domaine.m2c(k) * vit(fb) * pf(fb) / pe(e);
            contrib = C_iso * m2vf + fs(f) * pf(f) * (C_dir - C_iso) * domaine.dot(&ve(0), &dir(0)) * (e == f_e(f, 0) ? 1 : -1) * domaine.dot(&xv(f, 0), &dir(0), &xp(e, 0));
            if (contrib <= std::min(C_dir, C_iso) * m2vf) contrib = std::min(C_dir, C_iso) * m2vf; //pour garantir un frottement minimal
            resu(f) -= contrib;
          }
    }
  return resu;
}

/*! @brief copie de ajouter sauf la derniere ligne
 *
 */
void Perte_Charge_PolyMAC::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vit = inco;
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &fs = domaine.face_surfaces();
  const Sous_Domaine *pssz = sous_domaine ? &la_sous_domaine.valeur() : NULL;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int i, j, k, f, fb, r, C_nu = sub_type(Champ_Uniforme,nu.valeur()), C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur());
  double t = equation().schema_temps().temps_courant();
  DoubleVect pos(dimension), ve(dimension), vf(dimension), dir(dimension);

  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : domaine.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (r = 0; r < dimension; r++) pos(r) = xp(e, r);

      /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
      double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0),
             dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      for (j = domaine.vedeb(e), ve = 0; j < domaine.vedeb(e + 1); j++)
        for (r = 0; r < dimension; r++)
          fb = domaine.veji(j), ve(r) += domaine.veci(j, r) * vit(fb) * pf(fb) / pe(e);
      double n_ve = sqrt(domaine.dot(ve.addr(), ve.addr())), Re = std::max( n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;
      coeffs_perte_charge(ve, pos, t, n_ve, dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        if (f < domaine.nb_faces() && ch.fcl()(f, 0) < 2)
          {
            double m2vf = 0, contrib;
            for (k = domaine.m2i(domaine.m2d(e) + j); k < domaine.m2i(domaine.m2d(e) + j + 1); k++)
              fb = e_f(e, domaine.m2j(k)), m2vf += pf(f) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * domaine.volumes(e) * domaine.m2c(k) * vit(fb) * pf(fb) / pe(e);
            contrib = C_iso * m2vf + fs(f) * pf(f) * (C_dir - C_iso) * domaine.dot(&ve(0), &dir(0)) * (e == f_e(f, 0) ? 1 : -1) * domaine.dot(&xv(f, 0), &dir(0), &xp(e, 0));
            if (contrib >= std::min(C_dir, C_iso) * m2vf)
              {
                for (k = domaine.m2i(domaine.m2d(e) + j); k < domaine.m2i(domaine.m2d(e) + j + 1); k++)
                  if (ch.fcl()(fb = e_f(e, domaine.m2j(k)), 0) < 2)
                    matrice(f, fb) += C_iso * pf(f) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * domaine.volumes(e) * domaine.m2c(k) * pf(fb) / pe(e);
                for (k = domaine.vedeb(e); k < domaine.vedeb(e + 1); k++)
                  if (ch.fcl()(fb = domaine.veji(k), 0) < 2)
                    matrice(f, fb) += fs(f) * pf(f) * (C_dir - C_iso) * domaine.dot(&domaine.veci(k, 0), &dir(0)) * pf(fb) / pe(e) * (e == f_e(f, 0) ? 1 : -1) * domaine.dot(&xv(f, 0), &dir(0), &xp(e, 0));
              }
            else
              {
                for (k = domaine.m2i(domaine.m2d(e) + j); k < domaine.m2i(domaine.m2d(e) + j + 1); k++)
                  if (ch.fcl()(fb = e_f(e, domaine.m2j(k)), 0) < 2)
                    matrice(f, fb) += std::min(C_dir, C_iso) * pf(f) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * domaine.volumes(e) * domaine.m2c(k) * pf(fb) / pe(e);
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
  if (sous_domaine) la_sous_domaine = equation().probleme().domaine().ss_domaine(nom_sous_domaine);
}

////////////////////////////////////////////////////////////////
//                                                            //
//         Fonctions virtuelles pures de Source_base          //
//                                                            //
////////////////////////////////////////////////////////////////


void Perte_Charge_PolyMAC::associer_pb(const Probleme_base& pb)
{
  la_vitesse = ref_cast(Champ_Face_PolyMAC,equation().inconnue().valeur());
  le_fluide = ref_cast(Fluide_base,equation().milieu());
}

void Perte_Charge_PolyMAC::associer_domaines(const Domaine_dis& domaine_dis,
                                             const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_PolyMAC = ref_cast(Domaine_PolyMAC, domaine_dis.valeur());
  le_dom_Cl_PolyMAC = ref_cast(Domaine_Cl_PolyMAC, domaine_Cl_dis.valeur());
}

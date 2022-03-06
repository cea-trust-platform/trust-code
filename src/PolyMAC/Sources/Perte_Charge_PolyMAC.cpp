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
#include <Champ_Uniforme.h>
#include <Sous_Zone.h>
#include <Champ_Face_PolyMAC.h>

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

void Perte_Charge_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC& zone = la_Zone_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& dh = diam_hydr;
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &vit = la_vitesse->valeurs(), &nu = le_fluide->viscosite_cinematique().valeurs(), &vfd = zone.volumes_entrelaces_dir();
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &fs = zone.face_surfaces();
  const Sous_Zone *pssz = sous_zone ? &la_sous_zone.valeur() : NULL;
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  int i, j, f, d, D = dimension, C_nu = nu.dimension(0) == 1, C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur()), n, N = vit.line_size();
  double t = equation().schema_temps().temps_courant();
  DoubleTrav pos(D), ve(D), dir(D), C(N);

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : zone.nb_elem_tot()); i++) for (n = 0; n < N; n++)
      {
        int e = pssz ? (*pssz)[i] : i;
        for (d = 0; d < D; d++) pos(d) = xp(e, d);

        /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
        double dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);

        for (n = 0; n < N; n++)
          {
            for (ve = 0, j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) for (d = 0; d < D; d++)
                ve(d) += fs(f) * pf(f) / (ve(e) * pe(e)) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * vit(f, n);
            double n2_ve = zone.dot(ve.addr(), ve.addr()), n_ve = sqrt(n2_ve), nu_e = nu(!C_nu * e, n), Re = std::max( n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;
            coeffs_perte_charge(ve, pos, t, n_ve, dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);
            /* coefficient correspondant a la bonne direction */
            C(n) = C_iso * (C_dir - C_iso) * (n_ve > 1e-8 ? std::pow(zone.dot(ve.addr(), dir.addr()) , 2) / n2_ve : 0);
          }

        /* contributions aux faces de e */
        for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) if (f < zone.nb_faces())
            {
              for (n = 0; n < N; n++)secmem(f, n) -= pf(f) * vfd(f, e != f_e(f, 0)) * C(n) * vit(f, n);
              if (mat) for (n = 0; n < N; n++) (*mat)(N * f + n, N * f + n) += pf(f) * vfd(f, e != f_e(f, 0)) * C(n);
            }
      }
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
  le_fluide = ref_cast(Fluide_base,equation().milieu());
}

void Perte_Charge_PolyMAC::associer_zones(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_Cl_dis)
{
  la_Zone_PolyMAC = ref_cast(Zone_PolyMAC, zone_dis.valeur());
  la_Zone_Cl_PolyMAC = ref_cast(Zone_Cl_PolyMAC, zone_Cl_dis.valeur());
}

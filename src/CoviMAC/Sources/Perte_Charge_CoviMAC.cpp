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
// File:        Perte_Charge_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Sources
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_CoviMAC.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Champ_Uniforme.h>
#include <Sous_Zone.h>
#include <Champ_Face_CoviMAC.h>
#include <Check_espace_virtuel.h>
#include <Array_tools.h>
#include <Matrix_tools.h>

Implemente_base_sans_constructeur(Perte_Charge_CoviMAC,"Perte_Charge_CoviMAC",Source_base);

Perte_Charge_CoviMAC::Perte_Charge_CoviMAC():implicite_(1) { }

// printOn
//

Sortie& Perte_Charge_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << endl;
}

// readOn
//

Entree& Perte_Charge_CoviMAC::readOn(Entree& s )
{
  return s;
}

////////////////////////////////////////////////////////////////
//                                                            //
//             Fonction principale : ajouter                  //
//                                                            //
////////////////////////////////////////////////////////////////

void Perte_Charge_CoviMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  // const Zone_CoviMAC& zone = la_Zone_CoviMAC.valeur();
  // const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  // const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  // const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &vit = la_vitesse->valeurs();
  // const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &fs = zone.face_surfaces();
  // const Sous_Zone *pssz = sous_zone ? &la_sous_zone.valeur() : NULL;
  // const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  // int i, j, k, f, fb, r, C_nu = sub_type(Champ_Uniforme,nu.valeur()), C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur());
  // double t = equation().schema_temps().temps_courant();
  // DoubleVect pos(dimension), ve(dimension), ved(dimension), vep(dimension) , dir(dimension);
  //
  // /* contribution de chaque element ou on applique la perte de charge */
  // for (i = 0; i < (pssz ? pssz->nb_elem_tot() : zone.nb_elem_tot()); i++)
  //   {
  //     int e = pssz ? (*pssz)[i] : i;
  //     for (r = 0; r < dimension; r++) pos(r) = xp(e, r);
  //
  //     /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
  //     double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0),
  //            dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
  //     for (j = zone.ved(e), ve = 0; j < zone.ved(e + 1); j++) for (r = 0; r < dimension; r++)
  //         fb = zone.vej(j), ve(r) += zone.vec(j, r) * vit(fb) * pf(fb) / pe(e);
  //     double n_ve = sqrt(zone.dot(ve.addr(), ve.addr())), Re = max( n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;
  //     coeffs_perte_charge(ve, pos, t, n_ve, dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);
  //
  //     /* contributions aux faces de e */
  //     for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) if (f < zone.nb_faces() && ch.fcl(f, 0) < 2)
  //         {
  //           double m2vf = 0, contrib;
  //           for (k = zone.m2i(zone.m2d(e) + j); k < zone.m2i(zone.m2d(e) + j + 1); k++)
  //             fb = e_f(e, zone.m2j(k)), m2vf += pf(f) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * zone.volumes(e) * zone.m2c(k) * vit(fb) * pf(fb) / pe(e);
  //           contrib = C_iso * m2vf + fs(f) * pf(f) * (C_dir - C_iso) * zone.dot(&ve(0), &dir(0)) * (e == f_e(f, 0) ? 1 : -1) * zone.dot(&xv(f, 0), &dir(0), &xp(e, 0));
  //           if (contrib <= min(C_dir, C_iso) * m2vf) contrib = min(C_dir, C_iso) * m2vf; //pour garantir un frottement minimal
  //           resu(f) -= contrib;
  //         }
  //   }
}


void Perte_Charge_CoviMAC::completer()
{
  Source_base::completer();
  if (sous_zone) la_sous_zone = equation().probleme().domaine().ss_zone(nom_sous_zone);
}

////////////////////////////////////////////////////////////////
//                                                            //
//         Fonctions virtuelles pures de Source_base          //
//                                                            //
////////////////////////////////////////////////////////////////


void Perte_Charge_CoviMAC::associer_pb(const Probleme_base& pb)
{
  la_vitesse = ref_cast(Champ_Face_CoviMAC,equation().inconnue().valeur());
  le_fluide = ref_cast(Fluide_Incompressible,equation().milieu());
}

void Perte_Charge_CoviMAC::associer_zones(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_Cl_dis)
{
  la_Zone_CoviMAC = ref_cast(Zone_CoviMAC, zone_dis.valeur());
  la_Zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, zone_Cl_dis.valeur());
}

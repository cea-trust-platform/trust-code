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
#include <Zone_PolyMAC_P0.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Sous_Zone.h>
#include <Champ_Face_PolyMAC.h>
#include <Pb_Multiphase.h>
#include <Multiplicateur_diphasique_base.h>

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
  const Zone_Poly_base& zone = ref_cast(Zone_Poly_base, equation().zone_dis().valeur());
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& dh = diam_hydr;
  const DoubleTab& xp = zone.xp(), &xv = zone.xv(), &vit = la_vitesse->valeurs(), &pvit = la_vitesse->passe(), &nu = le_fluide->viscosite_cinematique().valeurs(), &vfd = zone.volumes_entrelaces_dir(),
                   &mu = le_fluide->viscosite_dynamique().valeurs(), &rho = le_fluide->masse_volumique().passe(), *alp = pbm ? &pbm->eq_masse.inconnue().passe() : NULL;
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &fs = zone.face_surfaces(), &ve = zone.volumes();
  const Multiplicateur_diphasique_base *fmult = pbm && pbm->has_correlation("multiplicateur_diphasique") ? &ref_cast(Multiplicateur_diphasique_base, pbm->get_correlation("multiplicateur_diphasique").valeur()) : NULL;
  const Sous_Zone *pssz = sous_zone ? &la_sous_zone.valeur() : NULL;
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins(), &fcl = ch.fcl();
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  int i, j, k, f, d, D = dimension, cN = nu.dimension(0) == 1, cM = mu.dimension(0) == 1, cR = rho.dimension(0) == 1, C_dh = sub_type(Champ_Uniforme,diam_hydr.valeur()), m, n,
                     N = vit.line_size(), poly_v2 = sub_type(Zone_PolyMAC_P0, zone), nf_tot = zone.nb_faces_tot();
  double t = equation().schema_temps().temps_courant(), v_min = 0.1, Gm, Fm, nvm, arm, C_dir, C_iso, v_dir;
  DoubleTrav pos(D), v(N, D), vm(D), v_ph(D), dir(D), C(N), nv(N), Cf(N), Cf_t(N), Fk(N), G(N), mult(N, 2);
  for (n = 0; n < N; n++) mult(n, 0) = 1, mult(n, 1) = 0; //valeur par defaut de mult

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : zone.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (d = 0; d < D; d++) pos(d) = xp(e, d);

      /* vecteur vitesse en e */
      double dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      if (poly_v2) for (d = 0; d < D; d++) for (n = 0; n < N; n++) v(n, d) = pvit(nf_tot + D * e + d, n); //vitesse par variable auxiliaire
      else for (v = 0, j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) for (n = 0; n < N; n++) for (d = 0; d < D; d++) /* PolyMAC V1 : vitesse a reconstruire */
              v(n, d) += fs(f) * pf(f) / (ve(e) * pe(e)) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * pvit(f, n);
      /* norme de v (avec seuil), debit surfacique par phase et total */
      for (n = 0, Gm = 0; n < N; Gm += G(n), n++)
        nv(n) = std::max(v_min, sqrt(zone.dot(&v(n, 0), &v(n, 0)))), G(n) = (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n) * nv(n);
      for (arm = 0, n = 0; n < N; n++) for (arm += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n), d = 0; d < D; d++)
          vm(d) += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n) * v(n, d);
      for (d = 0; d < D; d++) vm(d) /= arm;
      nvm = std::max(v_min, sqrt(zone.dot(&vm(0), &vm(0))));

      /* coefficients de frottements par composante : Cf(n) (phase seule), Cf_t(n) (le debit total dans cette phase) */
      for (n = 0; n < N; n++)
        {
          double Re = dh_e * std::max(G(n), 1e-10) / mu(!cM * e, n), Re_m = dh_e * Gm / mu(!cM * e, n);
          for (d = 0; d < D; d++) v_ph(d) = v(n, d);
          /* phase seule */
          coeffs_perte_charge(v_ph, pos, t, nv(n), dh_e, nu(!cN * e, n), Re, C_iso, C_dir, v_dir, dir);
          Cf(n) = (C_iso + (C_dir - C_iso) * (nv(n) > 1e-8 ? std::pow(zone.dot(&v(n, 0), &dir(0)) , 2) / (nv(n) * nv(n)) : 0)) * 2 * dh_e / std::max(G(n), 1e-10);
          /* tout le melange dans la phase */
          coeffs_perte_charge(vm, pos, t, nvm, dh_e, nu(!cN * e, n), Re_m, C_iso, C_dir, v_dir, dir);
          Cf_t(n) = (C_iso + (C_dir - C_iso) * (nvm > 1e-8 ? std::pow(zone.dot(&vm(0), &dir(0)) , 2) / (nvm * nvm) : 0)) * 2 * dh_e / std::max(Gm, 1e-10);
          Fk(n) = Cf(n) * G(n) * G(n) / rho(!cR * e, n); //force
        }
      Fm = Cf_t(0) * Gm * Gm / rho(!cR * e, 0); //force paroi "melange" (debit total, mais proprietes physiques du liquide)

      /* appel du multiplicateur diphasique (si il existe) */
      if (fmult) fmult->coefficient(&(*alp)(e, 0), &rho(!cR * e, 0), &nv(0), &Cf_t(0), &mu(!cM * e, 0), dh_e, 0.24, &Fk(0), Fm, mult);

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        if (f < zone.nb_faces() && (!poly_v2 || fcl(f, 0) < 2)) /* contrib aux faces de Dirichlet en Poly V1 */
          for (n = 0; n < N; n++)
            {
              double fac = pf(f) * vfd(f, e != f_e(f, 0)) * 0.5 / dh_e,
                     fac_n = fac * mult(n, 0) * Cf(n) * G(n) / rho(!cR * e, n),
                     fac_m = fac * mult(n, 1) * Cf_t(n) * Gm / rho(!cR * e, n);
              for (m = 0; m < N; m++) secmem(f, n) -= ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1) * vit(f, m);
              if (mat) for (m = 0; m < N; m++) (*mat)(N * f + n, N * f + m) += ((m == n) * fac_n +  fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1);
            }

      if (poly_v2) for (d = 0, k = nf_tot + D * e; d < D; d++, k++) for (n = 0; n < N; n++) /* PolyMAC V2: contributions aux equations aux elements */
            {
              double fac = pe(e) * ve(e) * 0.5 / dh_e,
                     fac_n = fac * mult(n, 0) * Cf(n) * G(n) / rho(!cR * e, n),
                     fac_m = fac * mult(n, 1) * Cf_t(n) * Gm / rho(!cR * e, n);
              for (m = 0; m < N; m++) secmem(k, n) -= ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1) * vit(k, m);
              if (mat) for (m = 0; m < N; m++) (*mat)(N * k + n, N * k + m) += ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1);
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

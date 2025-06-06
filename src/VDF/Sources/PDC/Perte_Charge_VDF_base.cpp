/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Multiplicateur_diphasique_base.h>
#include <Fluide_Incompressible.h>
#include <Perte_Charge_VDF_base.h>
#include <Schema_Temps_base.h>
#include <Milieu_composite.h>
#include <Champ_Uniforme.h>
#include <QDM_Multiphase.h>
#include <Champ_Face_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Probleme_base.h>
#include <Sous_Domaine.h>
#include <Domaine_VDF.h>
#include <Param.h>

Implemente_base(Perte_Charge_VDF_base, "Perte_Charge_VDF_base", Perte_Charge_Gen);

Sortie& Perte_Charge_VDF_base::printOn(Sortie& s) const { return s << que_suis_je() << endl; }

Entree& Perte_Charge_VDF_base::readOn(Entree& is) { return Perte_Charge_Gen::readOn(is); }

void Perte_Charge_VDF_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine = ref_cast(Domaine_VDF, equation().domaine_dis());
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;
  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue());
  const Champ_Don_base& dh = diam_hydr;

  const DoubleTab& xp = domaine.xp(),&xv = domaine.xv(),
                   &vit = la_vitesse->valeurs(),
                    &pvit = la_vitesse->passe(),
                     &nu = le_fluide->viscosite_cinematique().valeurs(),
                      &vfd = domaine.volumes_entrelaces_dir(),
                       &mu = le_fluide->viscosite_dynamique().valeurs(),
                        &rho = le_fluide->masse_volumique().passe(),
                         *alp = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr;

  const DoubleVect& pe = le_fluide->porosite_elem(),& pf = le_fluide->porosite_face(),
                    &fs = domaine.face_surfaces(), &ve = domaine.volumes();

  const Multiplicateur_diphasique_base *fmult = pbm && pbm->has_correlation("multiplicateur_diphasique") ?
                                                &ref_cast(Multiplicateur_diphasique_base, pbm->get_correlation("multiplicateur_diphasique")) : nullptr;

  const Sous_Domaine *pssz = sous_domaine ? &le_sous_domaine.valeur() : nullptr;

  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ch.fcl();

  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;

  const int D = dimension, cN = nu.dimension(0) == 1, cM = mu.dimension(0) == 1,
            cR = rho.dimension(0) == 1, C_dh = sub_type(Champ_Uniforme, diam_hydr.valeur()),
            N = vit.line_size();

  double t = equation().schema_temps().temps_courant(), v_min = 0.1;
  DoubleTrav pos(D), v(N, D), vm(D), v_ph(D), dir(D), nv(N), Cf(N), Cf_t(N), Fk(N), G(N), mult(N, 2), Sigma_tab;

  for (int n = 0; n < N; n++)
    mult(n, 0) = 1, mult(n, 1) = 0; //valeur par defaut de mult

  if (fmult) //si multiplicateur -> calcul de sigma
    {
      const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
      // Et pour les methodes span de la classe Saturation
      const int ne_tot = domaine.nb_elem_tot(), nb_max_sat =  N * (N-1) /2; // oui !! suite arithmetique !!
      Sigma_tab.resize(ne_tot, nb_max_sat);
      for (int k = 0; k < N; k++)
        for (int l = k + 1; l < N; l++)
          if (milc.has_saturation(k, l))
            {
              const Saturation_base& z_sat = milc.get_saturation(k, l);
              const int ind_trav = (k*(N-1)-(k-1)*(k)/2) + (l-k-1); // Et oui ! matrice triang sup !
              // recuperer Tsat et sigma ...
              const DoubleTab& sig = z_sat.get_sigma_tab();

              // fill in the good case
              for (int ii = 0; ii < ne_tot; ii++)
                Sigma_tab(ii, ind_trav) = sig(ii);
            }
    }

  /* contribution de chaque element ou on applique la perte de charge */
  for (int i = 0; i < (pssz ? pssz->nb_elem_tot() : domaine.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (int d = 0; d < D; d++)
        pos(d) = xp(e, d);

      /* vecteur vitesse en e */
      double dh_e = C_dh ? dh.valeurs()(0, 0) : dh.valeur_a_compo(pos, 0);

      v = 0.;
      for (int j = 0; j < e_f.dimension(1); j++)
        {
          const int f = e_f(e, j);
          for (int d = 0; d < D; d++)
            for (int n = 0; n < N; n++)
              v(n, d) += fs(f) * pf(f) / (ve(e) * pe(e)) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * pvit(f, n);
        }


      /* norme de v (avec seuil), debit surfacique par phase et total */
      double Gm = 0.;
      for (int n = 0; n < N; Gm += G(n), n++)
        {
          nv(n) = std::max(v_min, sqrt(domaine.dot(&v(n, 0), &v(n, 0))));
          G(n) = (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n) * nv(n);
        }

      double arm = 0.;
      for (int n = 0; n < N; n++)
        {
          arm += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n);

          for (int d = 0; d < D; d++)
            vm(d) += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n) * v(n, d);
        }

      for (int d = 0; d < D; d++)
        vm(d) /= arm;

      double nvm = std::max(v_min, sqrt(domaine.dot(&vm(0), &vm(0))));
      double C_iso, C_dir, v_dir;

      /* coefficients de frottements par composante : Cf(n) (phase seule), Cf_t(n) (le debit total dans cette phase) */
      for (int n = 0; n < N; n++)
        {
          double Re = dh_e * std::max(G(n), 1e-10) / mu(!cM * e, n),
                 Re_m = dh_e * Gm / mu(!cM * e, n);

          for (int d = 0; d < D; d++)
            v_ph(d) = v(n, d);

          /* phase seule */
          coeffs_perte_charge(v_ph, pos, t, nv(n), dh_e, nu(!cN * e, n), Re, C_iso, C_dir, v_dir, dir);

          Cf(n) = (C_iso + (C_dir - C_iso) * (nv(n) > 1e-8 ? std::pow(domaine.dot(&v(n, 0), &dir(0)), 2) / (nv(n) * nv(n)) : 0)) * 2 * dh_e / std::max(nv(n), 1e-10);

          /* tout le melange dans la phase */
          coeffs_perte_charge(vm, pos, t, nvm, dh_e, nu(!cN * e, n), Re_m, C_iso, C_dir, v_dir, dir);

          Cf_t(n) = (C_iso + (C_dir - C_iso) * (nvm > 1e-8 ? std::pow(domaine.dot(&vm(0), &dir(0)), 2) / (nvm * nvm) : 0)) * 2 * dh_e / std::max(nvm, 1e-10);

          Fk(n) = Cf(n) * G(n) * G(n) / rho(!cR * e, n) / 2.0 / dh_e; //force
        }
      double Fm = Cf_t(0) * Gm * Gm / rho(!cR * e, 0) / 2.0 / dh_e; //force paroi "melange" (debit total, mais proprietes physiques du liquide)

      /* appel du multiplicateur diphasique (si il existe) */
      if (fmult)
        fmult->coefficient(&(*alp)(e, 0), &rho(!cR * e, 0), &nv(0), &Cf_t(0), &mu(!cM * e, 0), dh_e, Sigma_tab(e,0), &Fk(0), Fm, mult);

      /* contributions aux faces de e */
      for (int j = 0; j < e_f.dimension(1); j++)
        {
          const int f = e_f(e, j);

          if (f < domaine.nb_faces() && (fcl(f, 0) < 2))
            for (int n = 0; n < N; n++)
              {
                int e1 = f_e(f, 0);
                if (e1 < 0) e1 = f_e(f, 1);

                double fac = pf(f) * vfd(f, e != e1) * 0.5 / dh_e;
                double fac_n = fac * mult(n, 0) * Cf(n) * nv(n);
                double fac_m = fac * mult(n, 1) * Cf_t(n) * Gm / rho(!cR * e, n);

                for (int m = 0; m < N; m++)
                  secmem(f, n) -= ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1) * vit(f, m);

                if (mat)
                  for (int m = 0; m < N; m++)
                    (*mat)(N * f + n, N * f + m) += ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1);
              }
        }
    }
}

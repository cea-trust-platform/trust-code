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

#include <Multiplicateur_diphasique_base.h>
#include <Fluide_Incompressible.h>
#include <Perte_Charge_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Probleme_base.h>
#include <Sous_Domaine.h>
#include <Param.h>

Implemente_base(Perte_Charge_PolyMAC, "Perte_Charge_PolyMAC", Source_base);

Sortie& Perte_Charge_PolyMAC::printOn(Sortie& s) const { return s << que_suis_je() << endl; }

Entree& Perte_Charge_PolyMAC::readOn(Entree& is)
{
  Param param(que_suis_je());
  Cerr << que_suis_je() << "::readOn " << finl;
  lambda.setNbVar(4 + dimension);
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  Cerr << "Interpretation de la fonction " << lambda.getString() << " ... ";
  lambda.parseString();
  Cerr << " Ok" << finl;
  if (diam_hydr->nb_comp() != 1)
    {
      Cerr << "Il faut definir le champ diam_hydr a une composante" << finl;
      exit();
    }
  return is;
}

void Perte_Charge_PolyMAC::set_param(Param& param)
{
  param.ajouter_non_std("lambda", (this), Param::REQUIRED);
  param.ajouter("diam_hydr", &diam_hydr, Param::REQUIRED);
  param.ajouter_non_std("sous_domaine|sous_zone", (this));
  param.ajouter("implicite", &implicite_);
}

int Perte_Charge_PolyMAC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "lambda")
    {
      Nom tmp;
      is >> tmp;
      lambda.setString(tmp);
      lambda.addVar("Re");
      lambda.addVar("t");
      lambda.addVar("x");
      if (dimension > 1)
        lambda.addVar("y");
      if (dimension > 2)
        lambda.addVar("z");
      return 1;
    }
  else if (mot == "sous_domaine")
    {
      is >> nom_sous_domaine;
      sous_domaine = true;
      return 1;
    }
  else // non compris
    {
      Cerr << "Mot cle \"" << mot << "\" non compris lors de la lecture d'un " << que_suis_je() << finl;
      Process::exit();
    }
  return -1;
}

void Perte_Charge_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  assert(has_interface_blocs());

  const Domaine_Poly_base& domaine = ref_cast(Domaine_Poly_base, equation().domaine_dis().valeur());
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& dh = diam_hydr;

  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vit = la_vitesse->valeurs(), &pvit = la_vitesse->passe(),
                   &nu = le_fluide->viscosite_cinematique().valeurs(), &vfd = domaine.volumes_entrelaces_dir(),
                    &mu = le_fluide->viscosite_dynamique().valeurs(), &rho = le_fluide->masse_volumique().passe(),
                     *alp = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr;

  const DoubleVect& pe = le_fluide->porosite_elem(), &pf = le_fluide->porosite_face(), &fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const Multiplicateur_diphasique_base *fmult = pbm && pbm->has_correlation("multiplicateur_diphasique") ?
                                                &ref_cast(Multiplicateur_diphasique_base, pbm->get_correlation("multiplicateur_diphasique").valeur()) : nullptr;

  const Sous_Domaine *pssz = sous_domaine ? &le_sous_domaine.valeur() : nullptr;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ch.fcl();
  Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : nullptr;

  int i, j, k, f, d, D = dimension, cN = nu.dimension(0) == 1, cM = mu.dimension(0) == 1, cR = rho.dimension(0) == 1,
                     C_dh = sub_type(Champ_Uniforme, diam_hydr.valeur()), m, n, N = vit.line_size(),
                     poly_v2 = sub_type(Domaine_PolyMAC_P0, domaine), nf_tot = domaine.nb_faces_tot();

  double t = equation().schema_temps().temps_courant(), v_min = 0.1, Gm, Fm, nvm, arm, C_dir, C_iso, v_dir;
  DoubleTrav pos(D), v(N, D), vm(D), v_ph(D), dir(D), nv(N), Cf(N), Cf_t(N), Fk(N), G(N), mult(N, 2);

  for (n = 0; n < N; n++)
    mult(n, 0) = 1, mult(n, 1) = 0; //valeur par defaut de mult

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : domaine.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (d = 0; d < D; d++)
        pos(d) = xp(e, d);

      /* vecteur vitesse en e */
      double dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      if (poly_v2)
        for (d = 0; d < D; d++)
          for (n = 0; n < N; n++)
            v(n, d) = pvit(nf_tot + D * e + d, n); //vitesse par variable auxiliaire
      else
        for (v = 0, j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
          for (n = 0; n < N; n++)
            for (d = 0; d < D; d++) /* PolyMAC_P0P1NC V1 : vitesse a reconstruire */
              v(n, d) += fs(f) * pf(f) / (ve(e) * pe(e)) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * pvit(f, n);
      /* norme de v (avec seuil), debit surfacique par phase et total */
      for (n = 0, Gm = 0; n < N; Gm += G(n), n++)
        nv(n) = std::max(v_min, sqrt(domaine.dot(&v(n, 0), &v(n, 0)))), G(n) = (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n) * nv(n);
      for (arm = 0, n = 0; n < N; n++)
        for (arm += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n), d = 0; d < D; d++)
          vm(d) += (alp ? (*alp)(e, n) : 1) * rho(!cR * e, n) * v(n, d);
      for (d = 0; d < D; d++)
        vm(d) /= arm;
      nvm = std::max(v_min, sqrt(domaine.dot(&vm(0), &vm(0))));

      /* coefficients de frottements par composante : Cf(n) (phase seule), Cf_t(n) (le debit total dans cette phase) */
      for (n = 0; n < N; n++)
        {
          double Re = dh_e * std::max(G(n), 1e-10) / mu(!cM * e, n), Re_m = dh_e * Gm / mu(!cM * e, n);
          for (d = 0; d < D; d++)
            v_ph(d) = v(n, d);
          /* phase seule */
          coeffs_perte_charge(v_ph, pos, t, nv(n), dh_e, nu(!cN * e, n), Re, C_iso, C_dir, v_dir, dir);
          Cf(n) = (C_iso + (C_dir - C_iso) * (nv(n) > 1e-8 ? std::pow(domaine.dot(&v(n, 0), &dir(0)), 2) / (nv(n) * nv(n)) : 0)) * 2 * dh_e / std::max(G(n), 1e-10);
          /* tout le melange dans la phase */
          coeffs_perte_charge(vm, pos, t, nvm, dh_e, nu(!cN * e, n), Re_m, C_iso, C_dir, v_dir, dir);
          Cf_t(n) = (C_iso + (C_dir - C_iso) * (nvm > 1e-8 ? std::pow(domaine.dot(&vm(0), &dir(0)), 2) / (nvm * nvm) : 0)) * 2 * dh_e / std::max(Gm, 1e-10);
          Fk(n) = Cf(n) * G(n) * G(n) / rho(!cR * e, n); //force
        }
      Fm = Cf_t(0) * Gm * Gm / rho(!cR * e, 0); //force paroi "melange" (debit total, mais proprietes physiques du liquide)

      /* appel du multiplicateur diphasique (si il existe) */
      if (fmult)
        fmult->coefficient(&(*alp)(e, 0), &rho(!cR * e, 0), &nv(0), &Cf_t(0), &mu(!cM * e, 0), dh_e, 0.24, &Fk(0), Fm, mult);

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        if (f < domaine.nb_faces() && (!poly_v2 || fcl(f, 0) < 2)) /* contrib aux faces de Dirichlet en Poly V1 */
          for (n = 0; n < N; n++)
            {
              double fac = pf(f) * vfd(f, e != f_e(f, 0)) * 0.5 / dh_e, fac_n = fac * mult(n, 0) * Cf(n) * G(n), fac_m = fac * mult(n, 1) * Cf_t(n) * Gm;
              for (m = 0; m < N; m++)
                secmem(f, n) -= ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1) * vit(f, m);
              if (mat)
                for (m = 0; m < N; m++)
                  (*mat)(N * f + n, N * f + m) += ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1);
            }

      if (poly_v2)
        for (d = 0, k = nf_tot + D * e; d < D; d++, k++)
          for (n = 0; n < N; n++) /* PolyMAC_P0P1NC V2: contributions aux equations aux elements */
            {
              double fac = pe(e) * ve(e) * 0.5 / dh_e, fac_n = fac * mult(n, 0) * Cf(n) * G(n), fac_m = fac * mult(n, 1) * Cf_t(n) * Gm;
              for (m = 0; m < N; m++)
                secmem(k, n) -= ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1) * vit(k, m);
              if (mat)
                for (m = 0; m < N; m++)
                  (*mat)(N * k + n, N * k + m) += ((m == n) * fac_n + fac_m) * (alp ? (*alp)(e, m) : 1) * (pbm ? rho(!cR * e, m) : 1);
            }
    }
}

DoubleTab& Perte_Charge_PolyMAC::ajouter(DoubleTab& resu) const
{
  if (has_interface_blocs()) return Source_base::ajouter(resu);

  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vit = la_vitesse->valeurs();
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &fs = domaine.face_surfaces();
  const Sous_Domaine *pssz = sous_domaine ? &le_sous_domaine.valeur() : NULL;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int i, j, k, f, fb, r, C_nu = sub_type(Champ_Uniforme, nu.valeur()), C_dh = sub_type(Champ_Uniforme, diam_hydr.valeur());
  double t = equation().schema_temps().temps_courant();
  DoubleVect pos(dimension), ve(dimension), dir(dimension);

  /* contribution de chaque element ou on applique la perte de charge */
  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : domaine.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (r = 0; r < dimension; r++)
        pos(r) = xp(e, r);

      /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
      double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0), dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      for (j = domaine.vedeb(e), ve = 0; j < domaine.vedeb(e + 1); j++)
        for (r = 0; r < dimension; r++)
          fb = domaine.veji(j), ve(r) += domaine.veci(j, r) * vit(fb) * pf(fb) / pe(e);
      double n_ve = sqrt(domaine.dot(ve.addr(), ve.addr())), Re = std::max(n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;
      coeffs_perte_charge(ve, pos, t, n_ve, dh_e, nu_e, Re, C_iso, C_dir, v_dir, dir);

      /* contributions aux faces de e */
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
        if (f < domaine.nb_faces() && ch.fcl()(f, 0) < 2)
          {
            double m2vf = 0, contrib;
            for (k = domaine.m2i(domaine.m2d(e) + j); k < domaine.m2i(domaine.m2d(e) + j + 1); k++)
              fb = e_f(e, domaine.m2j(k)), m2vf += pf(f) * (e == f_e(f, 0) ? 1 : -1) * (e == f_e(fb, 0) ? 1 : -1) * domaine.volumes(e) * domaine.m2c(k) * vit(fb) * pf(fb) / pe(e);
            contrib = C_iso * m2vf + fs(f) * pf(f) * (C_dir - C_iso) * domaine.dot(&ve(0), &dir(0)) * (e == f_e(f, 0) ? 1 : -1) * domaine.dot(&xv(f, 0), &dir(0), &xp(e, 0));
            if (contrib <= std::min(C_dir, C_iso) * m2vf)
              contrib = std::min(C_dir, C_iso) * m2vf; //pour garantir un frottement minimal
            resu(f) -= contrib;
          }
    }
  return resu;
}

void Perte_Charge_PolyMAC::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (has_interface_blocs())
    {
      Source_base::contribuer_a_avec(inco, matrice);
      return;
    }

  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  const Champ_Don& nu = le_fluide->viscosite_cinematique(), &dh = diam_hydr;
  const DoubleTab& xp = domaine.xp(), &xv = domaine.xv(), &vit = inco;
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &fs = domaine.face_surfaces();
  const Sous_Domaine *pssz = sous_domaine ? &le_sous_domaine.valeur() : NULL;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  int i, j, k, f, fb, r, C_nu = sub_type(Champ_Uniforme, nu.valeur()), C_dh = sub_type(Champ_Uniforme, diam_hydr.valeur());
  double t = equation().schema_temps().temps_courant();
  DoubleVect pos(dimension), ve(dimension), dir(dimension);

  for (i = 0; i < (pssz ? pssz->nb_elem_tot() : domaine.nb_elem_tot()); i++)
    {
      int e = pssz ? (*pssz)[i] : i;
      for (r = 0; r < dimension; r++)
        pos(r) = xp(e, r);

      /* valeurs evaluees en l'element : nu, Dh, vecteur vitesse, Re, coefficients de perte de charge isotrope et directionel + la direction */
      double nu_e = C_nu ? nu(0, 0) : nu->valeur_a_compo(pos, 0), dh_e = C_dh ? dh(0, 0) : dh->valeur_a_compo(pos, 0);
      for (j = domaine.vedeb(e), ve = 0; j < domaine.vedeb(e + 1); j++)
        for (r = 0; r < dimension; r++)
          fb = domaine.veji(j), ve(r) += domaine.veci(j, r) * vit(fb) * pf(fb) / pe(e);
      double n_ve = sqrt(domaine.dot(ve.addr(), ve.addr())), Re = std::max(n_ve * dh_e / nu_e, 1e-10), C_iso, C_dir, v_dir;
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
                    matrice(f, fb) += fs(f) * pf(f) * (C_dir - C_iso) * domaine.dot(&domaine.veci(k, 0), &dir(0)) * pf(fb) / pe(e) * (e == f_e(f, 0) ? 1 : -1)
                                      * domaine.dot(&xv(f, 0), &dir(0), &xp(e, 0));
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
  resu = 0.;
  return ajouter(resu);
}

void Perte_Charge_PolyMAC::completer()
{
  Source_base::completer();
  if (sous_domaine)
    le_sous_domaine = equation().probleme().domaine().ss_domaine(nom_sous_domaine);
}

void Perte_Charge_PolyMAC::associer_pb(const Probleme_base& pb)
{
  la_vitesse = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur());
  le_fluide = ref_cast(Fluide_base, equation().milieu());
}

void Perte_Charge_PolyMAC::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_PolyMAC = ref_cast(Domaine_PolyMAC, domaine_dis.valeur());
  le_dom_Cl_PolyMAC = ref_cast(Domaine_Cl_PolyMAC, domaine_Cl_dis.valeur());
}

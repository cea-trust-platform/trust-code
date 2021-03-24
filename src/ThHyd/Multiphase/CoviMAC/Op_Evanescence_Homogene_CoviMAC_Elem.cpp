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
// File:        Op_Evanescence_Homogene_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/CoviMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Evanescence_Homogene_CoviMAC_Elem.h>
#include <Zone_CoviMAC.h>
#include <Champ_P0_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Schema_Implicite_base.h>
#include <SETS.h>
#include <Param.h>
#include <Milieu_composite.h>

Implemente_instanciable(Op_Evanescence_Homogene_CoviMAC_Elem,"Op_Evanescence_Homogene_CoviMAC_Elem",Operateur_Evanescence_base);

Sortie& Op_Evanescence_Homogene_CoviMAC_Elem::printOn(Sortie& os) const
{
  return os;
}

Entree& Op_Evanescence_Homogene_CoviMAC_Elem::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("alpha_res", &alpha_res_, Param::REQUIRED);
  param.ajouter("alpha_res_min", &alpha_res_min_);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Op_Evanescence_Homogene_CoviMAC_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();

  /* on doit pouvoir ajouter / soustraire les equations entre composantes */
  int i, j, e, n, N = inco.line_size();
  if (N == 1) return; //pas d'evanescence en simple phase!
  for (auto &&n_m : matrices) if (n_m.second->nb_colonnes())
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        IntTrav sten(0, 2);
        sten.set_smart_resize(1);

        std::set<int> idx;
        for (e = 0; e < zone.nb_elem(); e++, idx.clear())
          {
            for (i = N * e, n = 0; n < N; n++, i++) for (j = mat.get_tab1()(i) - 1; j < mat.get_tab1()(i + 1) - 1; j++)
                idx.insert(mat.get_tab2()(j) - 1);
            for (i = N * e, n = 0; n < N; n++, i++) for (auto &&c : idx) sten.append_line(i, c);
          }
        Matrix_tools::allocate_morse_matrix(mat.nb_lignes(), mat.nb_colonnes(), sten, mat2);
        mat = mat2; //pour forcer l'ordre des coefficients dans la matrice (accelere les operations ligne a ligne)
      }
}

void Op_Evanescence_Homogene_CoviMAC_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
  const DoubleTab& inco = ch.valeurs(), &alpha = pb.eq_masse.inconnue().valeurs(),
                   &rho = equation().milieu().masse_volumique().valeurs(),
                    &p = pb.eq_qdm.pression().valeurs();
  const SETS *sch = sub_type(Schema_Implicite_base, pb.eq_qdm.schema_temps()) && sub_type(SETS, ref_cast(Schema_Implicite_base, pb.eq_qdm.schema_temps()).solveur().valeur())
                    ? &ref_cast(SETS, ref_cast(Schema_Implicite_base, pb.eq_qdm.schema_temps()).solveur().valeur()) : NULL;

  int e, i, j, k, n, N = inco.line_size(), m, M = p.line_size(), is_m = ch.le_nom() == "alpha", cR = (rho.dimension_tot(0) == 1),
                     iter = sch ? sch->iteration : 0, p_degen = is_m && sch ? sch->p_degen : 0;
  if (N == 1 || p_degen || (is_m && !iter)) return; //pas d'evanescence en simple phase ou si p est degenere

  double a_eps = alpha_res_, a_eps_min = alpha_res_min_, a_m, a_max; //seuil de declenchement du traitement de l'evanescence

  /* recherche de phases evanescentes et traitement des seconds membres */
  IntTrav maj(inco.dimension_tot(0)); //maj(i) : phase majoritaire de la ligne i
  DoubleTrav coeff(inco.dimension_tot(0), inco.line_size(), 2); //coeff(i, n, 0/1) : coeff a appliquer a l'equation existante / a l'eq. "inco = v_maj", leurs derivees en alpha
  Matrice_Morse& mat_diag = *matrices.at(ch.le_nom().getString());
  for (e = 0; e < zone.nb_elem(); e++)
    {
      /* phase majoritaire */
      for (a_max = 0, k = -1, n = 0; n < N; n++) if ((a_m = alpha(e, n)) > a_max) k = n, a_max = a_m;
      if (k >= 0) maj(e) = k;
      else abort();

      /* coeff d'evanescence, second membre */
      for (n = 0, m = 0; n < N; n++, m += (M > 1)) if (n != k && (a_m = alpha(e, n)) < a_eps)
          {
            double val = is_m ? 0 : milc.has_saturation(n, k) ? milc.get_saturation(n, k).Tsat(p(e, m)) : inco(e, k); //valeur a laquelle on veut ramener inco(e, n)
            coeff(e, n, 0) = (a_eps == a_eps_min ? (a_m < a_eps) : min(max((a_eps - a_m) / (a_eps - a_eps_min), 0.), 1.));
            coeff(e, n, 1) = mat_diag(N * e + k, N * e + k) * coeff(e, n, 0);
            double flux = coeff(e, n, 0) * secmem(e, n) + coeff(e, n, 1) * (inco(e, n) - val);
            secmem(e, k) += (p_degen ? rho(!cR * e, k) : 1) * flux, secmem(e, n) -= (p_degen ? rho(!cR * e, n) : 1) * flux;
          }
    }

  /* lignes de matrices */
  for (auto &&n_m : matrices) if (n_m.second->nb_colonnes())
      {
        int diag = (n_m.first == ch.le_nom().getString()), press = (n_m.first == "pression"); //est-on sur le bloc diagonal, sur le bloc pression?
        Matrice_Morse& mat = *n_m.second;
        for (e = 0; e < zone.nb_elem(); e++) for (n = 0, m = 0; n < N; n++, m += (M > 1)) if (coeff(e, n, 0))
              {
                k = maj(e); //phase majoritaire
                double dval = is_m ? 0 : milc.has_saturation(n, k) ? (press ? milc.get_saturation(n, k).dP_Tsat(p(e, m)) : 0) : diag; //derivee de val (nulle si on n'est pas sur le bon bloc)
                int cval = is_m ? -1 : milc.has_saturation(n, k) ? M * e + m : N * e + k; //indice de colonne associe a cette derivee
                for (i = mat.get_tab1()(N * e + n) - 1, j = mat.get_tab1()(N * e + k) - 1; i < mat.get_tab1()(N * e + n + 1) - 1; i++, j++)
                  {
                    assert(mat.get_tab2()(j) == mat.get_tab2()(i));
                    int c = mat.get_tab2()(i) - 1; //indice de colonne (commun aux deux lignes grace au dimensionner_blocs())
                    double dflux = -coeff(e, n, 0) * mat.get_set_coeff()(i) + coeff(e, n, 1) * (diag * (c == N * e + n) - dval * (c == cval)); //derivee de flux en secmem, inco, val
                    mat.get_set_coeff()(i) += (p_degen ? rho(!cR * e, n) : 1) * dflux;
                    mat.get_set_coeff()(j) -= (p_degen ? rho(!cR * e, k) : 1) * dflux;
                  }
              }
      }
  if (!is_m && equation().schema_temps().temps_courant() > 0.68)
    e++;
}

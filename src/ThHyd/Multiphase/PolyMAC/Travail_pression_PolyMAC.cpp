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
// File:        Travail_pression_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/PolyMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Travail_pression_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Champ_Elem_PolyMAC.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Op_Conv_EF_Stab_PolyMAC_Elem.h>
#include <cfloat>

Implemente_instanciable(Travail_pression_PolyMAC,"Travail_pression_Elem_PolyMAC|Travail_pression_Elem_PolyMAC_P0", Source_base);
// XD travail_pression source_base travail_pression 0 Source term which corresponds to the additional pressure work term that appears when dealing with compressible multiphase fluids

Sortie& Travail_pression_PolyMAC::printOn(Sortie& os) const
{
  return os;
}

Entree& Travail_pression_PolyMAC::readOn(Entree& is)
{
  return is;
}

void Travail_pression_PolyMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, equation().zone_dis().valeur());
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& inco = equation().inconnue().valeurs();
  int i, j, e, eb, ne = zone.nb_elem(), f,n, N = inco.line_size(), m, M = ref_cast(Pb_Multiphase, equation().probleme()).eq_qdm.pression().valeurs().line_size();

  for (auto &&n_m : matrices) if (n_m.first == "pression" || (n_m.first == "alpha" && !semi_impl.count("alpha")) || n_m.first == "vitesse")
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        IntTrav sten(0, 2);
        sten.set_smart_resize(1);
        if (n_m.first == "pression") /* pression : dependance locale, implicite */
          for (e = 0; e < ne; e++) for (n = 0, m = 0; n < N; n++, m += (M > 1)) sten.append_line(N * e + n, M * e + m);
        else if (n_m.first == "vitesse") for (e = 0; e < ne; e++) for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
              for (n = 0; n < N; n++) sten.append_line(N * e + n, N * f + n);
        else for (e = 0; e < ne; e++) for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                for (n = 0; n < N; n++) sten.append_line(N * e + n, N * eb + n);
        tableau_trier_retirer_doublons(sten);
        Matrix_tools::allocate_morse_matrix(inco.size_totale(), equation().probleme().get_champ(n_m.first).valeurs().size_totale(), sten, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }
}

void Travail_pression_PolyMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Zone_PolyMAC& zone = ref_cast(Zone_PolyMAC, equation().zone_dis().valeur());
  const DoubleVect& pe = zone.porosite_elem(), &pf = zone.porosite_face(), &fs = zone.face_surfaces(), &ve = zone.volumes();
  const Champ_Inc_base& ch_a = pbm.eq_masse.inconnue().valeur(), &ch_v = pbm.eq_qdm.inconnue().valeur(), &ch_p = pbm.eq_qdm.pression().valeur();
  /* trois tableaux de alpha : present / passe et champ convecte (peut etre semi-implicite) */
  const DoubleTab& alpha = ch_a.valeurs(), &c_alpha = semi_impl.count("alpha") ? semi_impl.at("alpha") : alpha, &p_alpha = ch_a.passe(), &press = ch_p.valeurs(), &vit = ch_v.valeurs();
  const IntTab& fcl = ref_cast(Champ_Elem_PolyMAC, ch_a).fcl(), &f_e = zone.face_voisins();
  DoubleTab b_alpha = ch_a.valeur_aux_bords();
  Matrice_Morse *Mp = matrices.count("pression") ? matrices.at("pression") : NULL,
                 *Ma = matrices.count("alpha") && !semi_impl.count("alpha") ? matrices.at("alpha") : NULL,
                  *Mv = matrices.count("vitesse") ? matrices.at("vitesse") : NULL;

  int i, j, e, eb, f, n, N = alpha.line_size(), m, M = press.line_size();
  const Op_Conv_EF_Stab_PolyMAC_Elem *op_conv = sub_type(Op_Conv_EF_Stab_PolyMAC_Elem, equation().operateur(1).l_op_base()) ? &ref_cast(Op_Conv_EF_Stab_PolyMAC_Elem, equation().operateur(1).l_op_base()) : NULL;
  double dt = equation().schema_temps().pas_de_temps(), alp = op_conv ? op_conv->alpha : 1; /* meme decentrement que l'operateur de convection si il existe, amont sinon */

  //partie -p d alpha_k / dt et ses derivees
  for (e = 0; e < zone.nb_elem(); e++)
    {
      for (n = 0, m = 0; n < N; n++, m += (M > 1)) secmem(e, n) -= pe(e) * ve(e) * press(e, m) * (alpha(e, n) - p_alpha(e, n)) / dt;
      if (Mp) for (n = 0, m = 0; n < N; n++, m += (M > 1)) (*Mp)(N * e + n, M * e + m) += pe(e) * ve(e) * (alpha(e, n) - p_alpha(e, n)) / dt;
      if (Ma) for (n = 0, m = 0; n < N; n++, m += (M > 1)) (*Ma)(N * e + n, N * e + n) += pe(e) * ve(e) * press(e, m) / dt;
    }

  //partie -p div (alpha_k v_k)
  DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval
  /* convection aux faces internes (fcl(f, 0) == 0), de Neumann_val_ext ou de Dirichlet */
  for (f = 0; f < zone.nb_faces(); f++) if (!fcl(f, 0) || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
      {
        for (dv_flux = 0, dc_flux = 0, i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++)
            {
              double v = vit(f, n) ? vit(f, n) : DBL_MIN, fac = pf(f) * fs(f) * (1. + (v * (i ? -1 : 1) > 0 ? 1. : -1) * alp) / 2;
              dv_flux(n) += fac * (e >= 0 ? c_alpha(e, n) : b_alpha(f, n)); //f est reelle -> indice trivial dans b_alpha
              dc_flux(i, n) = e >= 0 ? fac * vit(f, n) : 0;
            }

        //second membre
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0, m = 0; n < N; n++, m += (M > 1))
              secmem(e, n) -= (i ? -1 : 1) * press(e, m) * dv_flux(n) * vit(f, n);
        //derivees : vitesse
        if (Mv) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0, m = 0; n < N; n++, m += (M > 1))
                (*Mv)(N * e + n, N * f + n) += (i ? -1 : 1) * press(e, m) * dv_flux(n);
        //derivees : pression
        if (Mp) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0, m = 0; n < N; n++, m += (M > 1))
                (*Mp)(N * e + n, M * e + m) += (i ? -1 : 1) * dv_flux(n) * vit(f, n);
        //derivees : alpha
        if (Ma) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                for (n = 0, m = 0; n < N; n++, m += (M > 1)) (*Ma)(N * e + n, N * eb + n) += (i ? -1 : 1) * press(e, m) * dc_flux(j, n);
      }
}

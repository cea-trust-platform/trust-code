/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Source_Travail_pression_Elem_base.h>
#include <Champ_Inc_P0_base.h>
#include <Champ_Face_base.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Domaine_VF.h>
#include <cfloat>

Implemente_base(Source_Travail_pression_Elem_base, "Source_Travail_pression_Elem_base", Sources_Multiphase_base);
// XD travail_pression source_base travail_pression 0 Source term which corresponds to the additional pressure work term that appears when dealing with compressible multiphase fluids

Sortie& Source_Travail_pression_Elem_base::printOn(Sortie& os) const { return os; }
Entree& Source_Travail_pression_Elem_base::readOn(Entree& is) { return is; }

void Source_Travail_pression_Elem_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const DoubleTab& inco = equation().inconnue()->valeurs();
  int i, j, e, eb, ne = domaine.nb_elem(), f,n, N = inco.line_size(), m,
                   M = ref_cast(QDM_Multiphase, ref_cast(Pb_Multiphase, equation().probleme()).equation_qdm()).pression()->valeurs().line_size();

  for (auto &&n_m : matrices)
    if (n_m.first == "pression" || (n_m.first == "alpha" && !semi_impl.count("alpha")) || n_m.first == "vitesse")
      {
        Matrice_Morse& mat = *n_m.second, mat2;
        IntTrav sten(0, 2);

        if (n_m.first == "pression") /* pression : dependance locale, implicite */
          for (e = 0; e < ne; e++)
            for (n = 0, m = 0; n < N; n++, m += (M > 1)) sten.append_line(N * e + n, M * e + m);
        else if (n_m.first == "vitesse")
          for (e = 0; e < ne; e++)
            for (i = 0; i < e_f.dimension(1); i++)
              {
                if ((f = e_f(e, i)) >= 0)
                  for (n = 0; n < N; n++) sten.append_line(N * e + n, N * f + n);
              }
        else for (e = 0; e < ne; e++)
            for (i = 0; i < e_f.dimension(1); i++)
              if ((f = e_f(e, i)) >= 0)
                for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                  for (n = 0; n < N; n++) sten.append_line(N * e + n, N * eb + n);
        tableau_trier_retirer_doublons(sten);
        Matrix_tools::allocate_morse_matrix(inco.size_totale(), equation().probleme().get_champ(n_m.first).valeurs().size_totale(), sten, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }
}

void Source_Travail_pression_Elem_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Domaine_VF& domaine = ref_cast(Domaine_VF, equation().domaine_dis());
  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(),
                    &fs = domaine.face_surfaces(), &ve = domaine.volumes();

  const Champ_Inc_base& ch_a = pbm.equation_masse().inconnue().valeur(),
                        &ch_v = pbm.equation_qdm().inconnue().valeur(),
                         &ch_p = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression().valeur();

  /* trois tableaux de alpha : present / passe et champ convecte (peut etre semi-implicite) */
  const DoubleTab& alpha = ch_a.valeurs(), &c_alpha = semi_impl.count("alpha") ? semi_impl.at("alpha") : alpha,
                   &p_alpha = ch_a.passe(), &press = ch_p.valeurs(), &vit = ch_v.valeurs();

  const IntTab& fcl = ref_cast(Champ_Inc_P0_base, ch_a).fcl(), &f_e = domaine.face_voisins(),
                &fcl_v = ref_cast(Champ_Face_base, ch_v).fcl();

  DoubleTab b_alpha = ch_a.valeur_aux_bords();
  Matrice_Morse *Mp = matrices.count("pression") ? matrices.at("pression") : nullptr,
                 *Ma = matrices.count("alpha") && !semi_impl.count("alpha") ? matrices.at("alpha") : nullptr,
                  *Mv = matrices.count("vitesse") ? matrices.at("vitesse") : nullptr;

  int i, j, e, eb, f, n, N = alpha.line_size(), m, M = press.line_size();
  double dt = equation().schema_temps().pas_de_temps();

  //partie -p d alpha_k / dt et ses derivees
  for (e = 0; e < domaine.nb_elem(); e++)
    {
      for (n = 0, m = 0; n < N; n++, m += (M > 1))
        secmem(e, n) -= pe(e) * ve(e) * press(e, m) * (alpha(e, n) - p_alpha(e, n)) / dt;
      if (Mp)
        for (n = 0, m = 0; n < N; n++, m += (M > 1))
          (*Mp)(N * e + n, M * e + m) += pe(e) * ve(e) * (alpha(e, n) - p_alpha(e, n)) / dt;
      if (Ma)
        for (n = 0, m = 0; n < N; n++, m += (M > 1))
          (*Ma)(N * e + n, N * e + n) += pe(e) * ve(e) * press(e, m) / dt;
    }

  //partie -p div (alpha_k v_k)
  DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval
  /* convection aux faces internes (fcl(f, 0) == 0), de Neumann_val_ext ou de Dirichlet */
  for (f = 0; f < domaine.nb_faces(); f++)
    if (!fcl(f, 0) || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
      {
        for (dv_flux = 0, dc_flux = 0, i = 0; i < 2; i++)
          for (e = f_e(f, i), n = 0; n < N; n++)
            {
              const double v = vit(f, n) ? vit(f, n) : DBL_MIN,
                           fac = pf(f) * fs(f) * (1. + (v * (i ? -1 : 1) > 0 ? 1. : -1) * alp) / 2;

              dv_flux(n) += fac * (e >= 0 ? c_alpha(e, n) : b_alpha(f, n)); //f est reelle -> indice trivial dans b_alpha
              dc_flux(i, n) = e >= 0 ? fac * vit(f, n) : 0;
            }

        //second membre
        for (i = 0; i < 2; i++)
          if ((e = f_e(f, i)) >= 0)
            if (e < domaine.nb_elem())
              for (n = 0, m = 0; n < N; n++, m += (M > 1))
                secmem(e, n) -= (i ? -1 : 1) * press(e, m) * dv_flux(n) * vit(f, n);
        //derivees : vitesse
        if (Mv && (fcl_v(f, 0) < 2 || fcl_v(f, 0) == 5)) // XXX : pas pour CL Dirichlet !
          for (i = 0; i < 2; i++)
            if ((e = f_e(f, i)) >= 0)
              if (e < domaine.nb_elem())
                for (n = 0, m = 0; n < N; n++, m += (M > 1))
                  (*Mv)(N * e + n, N * f + n) += (i ? -1 : 1) * press(e, m) * dv_flux(n);
        //derivees : pression
        if (Mp)
          for (i = 0; i < 2; i++)
            if ((e = f_e(f, i)) >= 0)
              if (e < domaine.nb_elem())
                for (n = 0, m = 0; n < N; n++, m += (M > 1))
                  (*Mp)(N * e + n, M * e + m) += (i ? -1 : 1) * dv_flux(n) * vit(f, n);
        //derivees : alpha
        if (Ma)
          for (i = 0; i < 2; i++)
            if ((e = f_e(f, i)) >= 0)
              if (e < domaine.nb_elem())
                for (j = 0; j < 2; j++)
                  if ((eb = f_e(f, j)) >= 0)
                    for (n = 0, m = 0; n < N; n++, m += (M > 1))
                      (*Ma)(N * e + n, N * eb + n) += (i ? -1 : 1) * press(e, m) * dc_flux(j, n);
      }
}

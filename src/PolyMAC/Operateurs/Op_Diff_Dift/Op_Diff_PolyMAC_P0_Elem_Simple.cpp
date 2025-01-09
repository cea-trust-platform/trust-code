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

#include <Convection_Diffusion_Temperature.h>
#include <Flux_interfacial_PolyMAC_P0P1NC.h>
#include <Echange_contact_PolyMAC_P0.h>
#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Echange_externe_impose.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Flux_parietal_base.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Milieu_composite.h>
#include <Option_PolyMAC.h>
#include <Neumann_paroi.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <functional>
#include <cmath>

void Op_Diff_PolyMAC_P0_Elem::init_op_ext_simple() const
{
  if (som_ext_init_)
    return; //deja fait

  /* remplissage de op_ext : de proche en proche */
  op_ext = { this };

  som_ext_init_ = 1;
}

void Op_Diff_PolyMAC_P0_Elem::dimensionner_blocs_simple(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string nom_inco = equation().inconnue().le_nom().getString();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const IntTab& f_e = domaine.face_voisins();

  Matrice_Morse* mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  const int N = equation().inconnue().valeurs().line_size(); //nombre de composantes

  IntTab stencil; //stencils par matrice
  stencil.resize(0, 2);


  IntTrav tpfa(0, N); //pour suivre quels flux sont a deux points
  domaine.creer_tableau_faces(tpfa);
  tpfa = 1;

  Cerr << "Op_Diff_PolyMAC_P0_Elem::dimensionner() : ";

  //avec fgrad : parties hors Echange_contact (ne melange ni les problemes, ni les composantes)
  for (int f = 0; f < domaine.nb_faces(); f++)
    for (int i = 0; i < 2; i++)
      {
        const int e = f_e(f, i);
        if (e < 0) continue;

        if (e < domaine.nb_elem()) //stencil a l'element e
          for (int j = phif_d(f); j < phif_d(f + 1); j++)
            {
              const int e_s = phif_e(j);

              if (e_s < domaine.nb_elem_tot())
                for (int n = 0; n < N; n++)
                  {
                    stencil.append_line(N * e + n, N * e_s + n);

                    const int tmp = (e_s == f_e(f, 0)) || (e_s == f_e(f, 1)) || (phif_c(j, n) == 0);
                    tpfa(f, n) &= tmp;
                  }
            }
      }

  if (mat)
    {
      tableau_trier_retirer_doublons(stencil);
      Matrice_Morse mat2;
      Matrix_tools::allocate_morse_matrix(N * domaine.nb_elem_tot(), N * equation().domaine_dis().nb_elem_tot(), stencil, mat2);

      if (mat->nb_colonnes())
        *mat += mat2;
      else
        *mat = mat2;
    }

  int n_sten = stencil.dimension(0);

  const double elem_t = static_cast<double>(domaine.domaine().md_vector_elements()->nb_items_seq_tot()),
               face_t = static_cast<double>(domaine.md_vector_faces()->nb_items_seq_tot());
  Cerr << "width " << mp_sum_as_double(n_sten) / (N[0] * elem_t) << " "
       << mp_somme_vect_as_double(tpfa) * 100. / (N[0] * face_t) << "% TPFA " << finl;
}

void Op_Diff_PolyMAC_P0_Elem::ajouter_blocs_simple(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, equation().domaine_dis());
  const Champ_Inc_base& ch_inc = has_champ_inco() ? mon_inconnue() : equation().inconnue();
  const Champ_Elem_PolyMAC_P0& ch = ref_cast(Champ_Elem_PolyMAC_P0, ch_inc);

  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs();
  const DoubleVect& fs = domaine.face_surfaces();

  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();

  const Conds_lim& cls = equation().domaine_Cl_dis().les_conditions_limites();

  const int N = inco.line_size();

  Matrice_Morse* mat = !semi_impl.count(nom_inco) && matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  /* avec phif : flux hors Echange_contact -> mat[0] seulement */
  DoubleTrav flux(N);

  for (int f = 0; f < domaine.nb_faces(); f++)
    {
      flux = 0.;

      for (int i = phif_d(f); i < phif_d(f + 1); i++)
        {
          const int eb = phif_e(i);
          const int fb = eb - domaine.nb_elem_tot();

          if (fb < 0) //element
            {
              for (int n = 0; n < N; n++)
                flux(n) += phif_c(i, n) * fs(f) * inco(eb, n);

              if (mat)
                for (int j = 0; j < 2; j++)
                  {
                    const int e = f_e(f, j);
                    if (e < 0) continue;

                    if (e < domaine.nb_elem())
                      for (int n = 0; n < N; n++) //derivees
                        (*mat)(N * e + n, N * eb + n) += (j ? 1 : -1) * phif_c(i, n) * fs(f);
                  }
            }
          else if (fcl(fb, 0) == 1 || fcl(fb, 0) == 2)
            {
              for (int n = 0; n < N; n++) //Echange_impose_base
                flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs(f) *
                            ref_cast(Echange_impose_base, cls[fcl(fb, 1)].valeur()).T_ext(fcl(fb, 2), n) : 0);
            }
          else if (fcl(fb, 0) == 4)
            {
              for (int n = 0; n < N; n++) //Neumann non homogene
                flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs(f) *
                            ref_cast(Neumann_paroi, cls[fcl(fb, 1)].valeur()).flux_impose(fcl(fb, 2), n) : 0);
            }
          else if (fcl(fb, 0) == 6)
            {
              for (int n = 0; n < N; n++) //Dirichlet
                flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs(f) *
                            ref_cast(Dirichlet, cls[fcl(fb, 1)].valeur()).val_imp(fcl(fb, 2), n) : 0);
            }
        }

      for (int j = 0; j < 2; j++)
        {
          const int e = f_e(f, j);
          if (e < 0) continue;

          if (e < domaine.nb_elem())
            for (int n = 0; n < N; n++) //second membre -> amont/aval
              secmem(e, n) += (j ? -1 : 1) * flux(n);
        }

      if (f < domaine.premiere_face_int())
        for (int n = 0; n < N; n++)
          flux_bords_(f, n) = flux(n); //flux aux bords
    }
}

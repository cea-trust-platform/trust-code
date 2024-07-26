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

#ifndef TRUSTChamp_Divers_generique_included
#define TRUSTChamp_Divers_generique_included

#include <Champ_Don_base.h>

enum class Champ_Divers_Type { INUTILE , UNIFORME , CANAL };

template <Champ_Divers_Type _TYPE_>
class TRUSTChamp_Divers_generique : public Champ_Don_base
{
public:
  Champ_base& affecter(const Champ_base& ch)
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME), IS_CANAL = (_TYPE_ == Champ_Divers_Type::CANAL);
    if (IS_UNIFORME) return Champ_Don_base::affecter(ch);
    if (IS_CANAL) Champ_base::affecter_erreur();

    return *this;
  }

  DoubleVect& valeur_a(const DoubleVect&, DoubleVect& tab_valeurs) const override
  {
    assert (_TYPE_ != Champ_Divers_Type::CANAL);
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME);
    if (IS_UNIFORME)
      {
        for (int j = 0; j < nb_comp(); j++) tab_valeurs(j) = valeurs_(0, j);
        return tab_valeurs;
      }
    else
      return not_implemented_champ_<DoubleVect&>(__func__); /* Cas INUTILE : methode sur-chargee pour CANAL */
  }

  double valeur_a_compo(const DoubleVect& x, int ncomp) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME), IS_CANAL = (_TYPE_ == Champ_Divers_Type::CANAL);
    if (IS_UNIFORME) return valeurs_(0,ncomp);
    else if (IS_CANAL) return Champ_Don_base::valeur_a_compo(x,ncomp);
    else return not_implemented_champ_<double>(__func__); /* INUTILE */
  }

  DoubleVect& valeur_a_elem(const DoubleVect& positions, DoubleVect& tab_valeurs, int) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME), IS_CANAL = (_TYPE_ == Champ_Divers_Type::CANAL);
    if (IS_UNIFORME)
      {
        for (int j = 0; j < nb_comp(); j++) tab_valeurs(j) = valeurs_(0, j);
        return tab_valeurs;
      }

    else if (IS_CANAL) return valeur_a(positions, tab_valeurs); // from VTABLE

    else return not_implemented_champ_<DoubleVect&>(__func__); /* INUTILE */
  }

  double valeur_a_elem_compo(const DoubleVect&, int, int ncomp) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME);
    return IS_UNIFORME ? valeurs_(0,ncomp) : not_implemented_champ_<double>(__func__); /* methode sur-chargee pour CANAL */
  }

  DoubleTab& valeur_aux(const DoubleTab& , DoubleTab& tab_valeurs) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME);
    if (IS_UNIFORME)
      {
        int size = tab_valeurs.dimension_tot(0); // GF dimension_tot pour que la ligne soit valide pour les champs P1B
        int nb_comp = tab_valeurs.line_size();
        start_gpu_timer(__KERNEL_NAME__);
        bool kernelOnDevice = tab_valeurs.checkDataOnDevice();
        if (kernelOnDevice)
          {
            CDoubleTabView valeurs = valeurs_.view_ro();
            DoubleTabView tab_valeurs_v = tab_valeurs.view_rw();
            Kokkos::parallel_for(__KERNEL_NAME__, Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {size, nb_comp }),
                                 KOKKOS_LAMBDA(const int i, const int j)
            {
              tab_valeurs_v(i, j) = valeurs(0, j);
            });
          }
        else
          {
            for (int i = 0; i < size; i++)
              for (int j = 0; j < nb_comp; j++)
                tab_valeurs(i, j) = valeurs_(0, j);
          }
        end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
        return tab_valeurs;
      }
    else
      return not_implemented_champ_<DoubleTab&>(__func__); // voir classes filles ...
  }

  DoubleVect& valeur_aux_compo(const DoubleTab&, DoubleVect& tab_valeurs, int ncomp) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME);
    if (IS_UNIFORME) return tab_valeurs = valeurs_(0, ncomp);
    else return not_implemented_champ_<DoubleVect&>(__func__); // BOOM
  }

  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect&, DoubleTab& tab_valeurs) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME);
    if (IS_UNIFORME)
      {
        int size = tab_valeurs.dimension(0);
        int nb_comp = tab_valeurs.line_size();
        start_gpu_timer(__KERNEL_NAME__);
        bool kernelOnDevice = tab_valeurs.checkDataOnDevice();
        if (kernelOnDevice)
          {
            CDoubleTabView valeurs = valeurs_.view_ro();
            DoubleTabView tab_valeurs_v = tab_valeurs.view_rw();
            Kokkos::parallel_for(__KERNEL_NAME__, Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0, 0}, {size, nb_comp}),
                                 KOKKOS_LAMBDA(const int i, const int j)
            {
              tab_valeurs_v(i, j) = valeurs(0, j);
            });
          }
        else
          {
            for (int i = 0; i < size; i++)
              for (int j = 0; j < nb_comp; j++)
                tab_valeurs(i, j) = valeurs_(0, j);
          }
        end_gpu_timer(kernelOnDevice, __KERNEL_NAME__);
        return tab_valeurs;
      }

    else return valeur_aux(positions, tab_valeurs); // from VTABLE
  }

  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect&, DoubleVect& tab_valeurs, int ncomp) const override
  {
    static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Divers_Type::UNIFORME);
    if (IS_UNIFORME) return tab_valeurs = valeurs_(0, ncomp);

    else return valeur_aux_compo(positions, tab_valeurs, ncomp); // from VTABLE
  }
};

#endif /* TRUSTChamp_Divers_generique_included */

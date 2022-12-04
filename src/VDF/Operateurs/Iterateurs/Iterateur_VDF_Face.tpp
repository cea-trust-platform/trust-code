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

#ifndef Iterateur_VDF_Face_TPP_included
#define Iterateur_VDF_Face_TPP_included

#include <Champ_Uniforme.h>
#include <communications.h>
#include <Pb_Multiphase.h>
#include <TRUSTSingle.h>

template<class _TYPE_>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(op_base->equation().inconnue().valeurs().nb_dim() < 3);
  const int ncomp = op_base->equation().inconnue().valeurs().line_size();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  tab_flux_bords.resize(la_zone->nb_faces_bord(), dimension);
  tab_flux_bords = 0.;

  if (ncomp == 1)
    {
      ajouter_blocs_aretes_bords<SingleDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_aretes_coins<SingleDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_aretes_internes<SingleDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_aretes_mixtes<SingleDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_fa7_sortie_libre<SingleDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_fa7_elem<SingleDouble>(ncomp, mats, secmem, semi_impl);
    }
  else
    {
      ajouter_blocs_aretes_bords<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_aretes_coins<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_aretes_internes<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_aretes_mixtes<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_fa7_sortie_libre<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
      ajouter_blocs_fa7_elem<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
    }

  if (!incompressible_ )
    {
      assert (is_pb_multi && is_conv_op_);
      if (ncomp == 1)
        ajouter_pour_compressible<SingleDouble>(ncomp, mats, secmem, semi_impl);
      else
        ajouter_pour_compressible<ArrOfDouble>(ncomp, mats, secmem, semi_impl);
    }

  // On multiplie les flux au bord par rho en hydraulique (sert uniquement a la sortie)
  if (!is_pb_multi) multiply_by_rho_if_hydraulique(tab_flux_bords);
}

/* ================== *
 * ====== BORDS =====
 * ================== */
template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_bords(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (!_TYPE_::CALC_ARR_BORD) return; /* do nothing */

  for (int n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_PAR, Type_Flux_Arete::PAROI, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_SYMM, Type_Flux_Arete::SYMETRIE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR, Type_Flux_Arete::SYMETRIE_PAROI, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_FL, Type_Flux_Arete::FLUIDE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL, Type_Flux_Arete::PAROI_FLUIDE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_PERIO, Type_Flux_Arete::PERIODICITE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_blocs_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL, Type_Flux_Arete::SYMETRIE_FLUIDE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        default:
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_bords_(const int n_arete, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
      Type_Double flux(ncomp), aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int n = la_zone->nb_faces_bord(), fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), signe = Qdm(n_arete, 3);
      DoubleTab& tab_flux_bords = op_base->flux_bords();
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();

      const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                             &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();

      // second membre
      flux_evaluateur.template flux_arete < Arete_Type > (inco, a_r, fac1, fac2, fac3, signe, flux);
      for (int k = 0; k < ncomp; k++)
        {
          secmem(fac3, k) += signe * flux[k];
          if (is_PAROI)
            {
              if (fac1 < n) tab_flux_bords(fac1, orientation(fac3)) -= 0.5 * signe * flux[k];
              if (fac2 < n) tab_flux_bords(fac2, orientation(fac3)) -= 0.5 * signe * flux[k];
            }
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        {
          flux_evaluateur.template coeffs_arete < Arete_Type > (a_r, fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
          for (int i = 0; i < ncomp; i++)
            fill_coeff_matrice_morse < Type_Double > (fac3, i, ncomp, signe, aii3_4, *matrice);
        }
    }
}

template <class _TYPE_>  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_bords_(const int n_arete, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (should_calc_flux)
    {
      constexpr bool is_FLUIDE = (Arete_Type == Type_Flux_Arete::FLUIDE), is_PAROI_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
      Type_Double flux3(ncomp), flux1_2(ncomp), aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int n = la_zone->nb_faces_bord(), fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), signe = Qdm(n_arete, 3);
      DoubleTab& tab_flux_bords = op_base->flux_bords();
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();

      const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                             &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();

      // second membre
      flux_evaluateur.template flux_arete < Arete_Type > (inco, a_r, fac1, fac2, fac3, signe, flux3, flux1_2);
      for (int k = 0; k < ncomp; k++)
        secmem(fac3, k) += signe * flux3[k];

      fill_resu_tab < Type_Double > (fac1, fac2, ncomp, flux1_2, secmem);

      if (is_FLUIDE || is_PAROI_FL)
        {
          if (fac1 < n)
            for (int k = 0; k < ncomp; k++) tab_flux_bords(fac1, orientation(fac3)) -= 0.5 * signe * flux3[k];

          if (fac2 < n)
            for (int k = 0; k < ncomp; k++) tab_flux_bords(fac2, orientation(fac3)) -= 0.5 * signe * flux3[k];
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        {
          flux_evaluateur.template coeffs_arete < Arete_Type > (a_r, fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
          for (int i = 0; i < ncomp; i++)
            {
              fill_coeff_matrice_morse < Type_Double > (fac3, i, ncomp, signe, aii3_4, *matrice);
              fill_coeff_matrice_morse < Type_Double > (fac1, fac2, i, ncomp, aii1_2, ajj1_2, *matrice);
            }
        }
    }
}

template <class _TYPE_>  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_bords_(const int n_arete, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (should_calc_flux)
    {
      Type_Double flux3_4(ncomp), flux1_2(ncomp), aii(ncomp), ajj(ncomp);
      const int fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), fac4 = Qdm(n_arete, 3);
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();



      // second membre
      flux_evaluateur.template flux_arete < Arete_Type > (inco, nullptr, fac1, fac2, fac3, fac4, flux3_4, flux1_2);
      for (int k = 0; k < ncomp; k++)
        {
          secmem(fac3, k) += 0.5 * flux3_4[k];
          secmem(fac4, k) -= 0.5 * flux3_4[k];
        }

      fill_resu_tab < Type_Double > (fac1, fac2, ncomp, flux1_2, secmem);

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        {
          flux_evaluateur.template coeffs_arete < Arete_Type > (nullptr, fac3, fac4, fac1, fac2, aii, ajj);
          for (int i = 0; i < ncomp; i++)
            fill_coeff_matrice_morse(fac1, fac2, i, ncomp, aii, ajj, *matrice);

          flux_evaluateur.template coeffs_arete < Arete_Type > (nullptr, fac1, fac2, fac3, fac4, aii, ajj);
          for (int i = 0; i < ncomp; i++)
            {
              aii[i] *= 0.5;
              ajj[i] *= 0.5;
              fill_coeff_matrice_morse < Type_Double > (fac3, fac4, i, ncomp, aii, ajj, *matrice);
            }
        }
    }
}

/* ================== *
 * ====== COINS =====
 * ================== */
template<class _TYPE_> template <typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_coins(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      const int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PAROI_FLUIDE:
          ajouter_blocs_aretes_coins_<_TYPE_::CALC_ARR_PAR, Type_Flux_Arete::PAROI, TypeAreteCoinVDF::PAROI_FLUIDE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          ajouter_blocs_aretes_coins_<_TYPE_::CALC_ARR_PAR, Type_Flux_Arete::PAROI, TypeAreteCoinVDF::FLUIDE_PAROI, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          ajouter_blocs_aretes_coins_<_TYPE_::CALC_ARR_PAR, Type_Flux_Arete::PAROI, TypeAreteCoinVDF::PERIO_PAROI, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          ajouter_blocs_aretes_coins_<_TYPE_::CALC_ARR_COIN_FL, Type_Flux_Arete::COIN_FLUIDE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        case TypeAreteCoinVDF::PERIO_PERIO:
          ajouter_blocs_aretes_coins_<_TYPE_::CALC_ARR_PERIO, Type_Flux_Arete::PERIODICITE, Type_Double>(n_arete, ncomp, mats, secmem, semi_impl);
          break;
        default:
          break;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin, typename Type_Double>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_coins_(const int n_arete, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (should_calc_flux)
    {
      constexpr bool is_PERIO_PAROI = (Arete_Type_Coin == TypeAreteCoinVDF::PERIO_PAROI);
      Type_Double flux(ncomp), aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), signe = Qdm(n_arete, 3);
      DoubleTab& tab_flux_bords = op_base->flux_bords();
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();

      const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                             &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();

      // second membre
      flux_evaluateur.template flux_arete < Arete_Type > (inco, a_r, fac1, fac2, fac3, signe, flux);
      for (int k = 0; k < ncomp; k++)
        {
          secmem(fac3, k) += signe * flux[k];
          if (is_PERIO_PAROI) /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */
            {
              tab_flux_bords(fac1, orientation(fac3)) -= 0.25 * signe * flux[k];
              tab_flux_bords(fac2, orientation(fac3)) -= 0.25 * signe * flux[k];
            }
          else
            tab_flux_bords(fac1, orientation(fac3)) -= 0.5 * signe * flux[k];
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        {
          flux_evaluateur.template coeffs_arete < Arete_Type > (a_r, fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
          for (int i = 0; i < ncomp; i++)
            fill_coeff_matrice_morse(fac3, i, ncomp, signe, aii3_4, *matrice);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
enable_if_t<Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_coins_(const int n_arete, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (should_calc_flux)
    {
      Type_Double flux3(ncomp), flux1_2(ncomp), aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
      const int n = la_zone->nb_faces_bord(), fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), signe = Qdm(n_arete, 3);
      DoubleTab& tab_flux_bords = op_base->flux_bords();
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();

      const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                             &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();

      // second membre
      flux_evaluateur.template flux_arete < Arete_Type > (inco, a_r, fac1, fac2, fac3, signe, flux3, flux1_2);
      for (int k = 0; k < ncomp; k++)
        {
          secmem(fac3, k) += signe * flux3[k];
          secmem(fac1, k) += flux1_2[k];
          if (fac1 < n) tab_flux_bords(fac1, orientation(fac3)) -= 0.5 * signe * flux3[k];
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        {
          flux_evaluateur.template coeffs_arete < Arete_Type > (a_r, fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);

          for (int i = 0; i < ncomp; i++)
            {
              fill_coeff_matrice_morse(fac3, i, ncomp, signe, aii3_4, *matrice);
              fill_coeff_matrice_morse < Type_Double > (fac1, i, ncomp, 1, aii1_2, *matrice);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_coins_(const int n_arete, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (should_calc_flux)
    {
      Type_Double flux3_4(ncomp), flux1_2(ncomp), aii(ncomp), ajj(ncomp);
      const int fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), fac4 = Qdm(n_arete, 3);
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();

      // second membre
      flux_evaluateur.template flux_arete < Arete_Type > (inco, nullptr, fac1, fac2, fac3, fac4, flux3_4, flux1_2);
      for (int k = 0; k < ncomp; k++)
        {
          secmem(fac3, k) += 0.5 * flux3_4[k];
          secmem(fac4, k) -= 0.5 * flux3_4[k];
          secmem(fac1, k) += 0.5 * flux1_2[k];
          secmem(fac2, k) -= 0.5 * flux1_2[k];
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        {
          flux_evaluateur.template coeffs_arete < Arete_Type > (nullptr, fac3, fac4, fac1, fac2, aii, ajj);
          for (int i = 0; i < ncomp; i++)
            fill_coeff_matrice_morse < Type_Double > (fac1, fac2, i, ncomp, aii, ajj, *matrice);

          flux_evaluateur.template coeffs_arete < Arete_Type > (nullptr, fac1, fac2, fac3, fac4, aii, ajj);
          for (int i = 0; i < ncomp; i++)
            fill_coeff_matrice_morse < Type_Double > (fac3, fac4, i, ncomp, aii, ajj, *matrice);
        }
    }
}

/* =============================== *
 * ====== INTERNES  & MIXTES =====
 * =============================== */
template<class _TYPE_> template <typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_internes(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if(!_TYPE_::CALC_ARR_INT) return; /* do nothing */

  ajouter_blocs_aretes_generique_<true, Type_Flux_Arete::INTERNE, Type_Double>(premiere_arete_interne, derniere_arete_interne, ncomp, mats, secmem, semi_impl);
}

template<class _TYPE_> template <typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_mixtes(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if(!_TYPE_::CALC_ARR_MIXTE) return; /* do nothing */

  ajouter_blocs_aretes_generique_<true, Type_Flux_Arete::MIXTE, Type_Double>(premiere_arete_mixte, derniere_arete_mixte, ncomp, mats, secmem, semi_impl);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
enable_if_t<Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_aretes_generique_(const int debut, const int fin, const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  // XXX : tab_flux_bords rempli seulement si MIXTE ... ie pas INTERNE !
  if (should_calc_flux)
    {
      constexpr bool is_MIXTE = (Arete_Type == Type_Flux_Arete::MIXTE);
      Type_Double flux(ncomp), aii(ncomp), ajj(ncomp);
      DoubleTab& tab_flux_bords = op_base->flux_bords();
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();

      const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                             &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();

      // second membre
      for (int n_arete = debut; n_arete < fin; n_arete++)
        {
          const int fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), fac4 = Qdm(n_arete, 3);
          const int n = la_zone->nb_faces_bord(), n2 = la_zone->nb_faces_tot(); /* GF pour assurer bilan seq = para */
          flux_evaluateur.template flux_arete < Arete_Type > (inco, a_r, fac1, fac2, fac3, fac4, flux);
          fill_resu_tab < Type_Double > (fac3, fac4, ncomp, flux, secmem);

          if (is_MIXTE)
            {
              if (fac4 < n2)
                {
                  if (fac1 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac1, orientation(fac3)) -= flux[k];

                  if (fac2 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac2, orientation(fac4)) -= flux[k];
                }
              if (fac3 < n2)
                {
                  if (fac1 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac1, orientation(fac3)) += flux[k];

                  if (fac2 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac2, orientation(fac4)) += flux[k];
                }
            }

          flux_evaluateur.template flux_arete < Arete_Type > (inco, a_r, fac3, fac4, fac1, fac2, flux);
          fill_resu_tab < Type_Double > (fac1, fac2, ncomp, flux, secmem);
          if (is_MIXTE)
            {
              if (fac2 < n2)
                {
                  if (fac3 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac3, orientation(fac1)) -= flux[k];

                  if (fac4 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac4, orientation(fac2)) -= flux[k];
                }
              if (fac1 < n2)
                {
                  if (fac3 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac3, orientation(fac1)) += flux[k];

                  if (fac4 < n)
                    for (int k = 0; k < ncomp; k++) tab_flux_bords(fac4, orientation(fac2)) += flux[k];
                }
            }
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        for (int n_arete = debut; n_arete < fin; n_arete++)
          {
            const int fac1 = Qdm(n_arete, 0), fac2 = Qdm(n_arete, 1), fac3 = Qdm(n_arete, 2), fac4 = Qdm(n_arete, 3);

            flux_evaluateur.template coeffs_arete < Arete_Type > (a_r, fac3, fac4, fac1, fac2, aii, ajj);
            for (int i = 0; i < ncomp; i++)
              fill_coeff_matrice_morse < Type_Double > (fac1, fac2, i, ncomp, aii, ajj, *matrice);

            flux_evaluateur.template coeffs_arete < Arete_Type > (a_r, fac1, fac2, fac3, fac4, aii, ajj);
            for (int i = 0; i < ncomp; i++)
              fill_coeff_matrice_morse < Type_Double > (fac3, fac4, i, ncomp, aii, ajj, *matrice);
          }
    }
}

/* ============================= *
 * ====== FA7 SORTIE LIBRE =====
 * ============================= */
template<class _TYPE_> template <typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_fa7_sortie_libre(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      switch(type_cl(la_cl))
        {
        case sortie_libre:
          ajouter_blocs_fa7_sortie_libre_<_TYPE_::CALC_FA7_SORTIE_LIB, Type_Flux_Fa7::SORTIE_LIBRE, Type_Double>(num_cl, ncomp, mats, secmem, semi_impl);
          break;
        case symetrie: /* fall through */
        case entree_fluide:
        case paroi_fixe:
        case paroi_defilante:
        case paroi_adiabatique:
        case paroi:
        case echange_externe_impose:
        case echange_global_impose:
        case periodique:
          break;
        default:
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_fa7_sortie_libre_(const int num_cl, const int ncomp , matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  // TODO : FIXME : tab_flux_bords pas rempli ...
  if (should_calc_flux)
    {
      Type_Double flux(ncomp), aii(ncomp), ajj(ncomp);
      const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
      const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();

      const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                             &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();

      // second membre
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.template flux_fa7 < Fa7_Type > (inco, a_r, face, (const Neumann_sortie_libre&) la_cl.valeur(), ndeb, flux);
          if ((elem(face, 0)) > -1)
            for (int k = 0; k < ncomp; k++) secmem(face, k) += flux[k];

          if ((elem(face, 1)) > -1)
            for (int k = 0; k < ncomp; k++) secmem(face, k) -= flux[k];
        }

      // derivees : champ convecte
      Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
      if (matrice)
        for (int face = ndeb; face < nfin; face++)
          {
            flux_evaluateur.template coeffs_fa7 < Fa7_Type > (a_r, face, (const Neumann_sortie_libre&) la_cl.valeur(), aii, ajj);
            if ((elem(face, 0)) > -1)
              for (int i = 0; i < ncomp; i++) fill_coeff_matrice_morse < Type_Double > (face, i, ncomp, 1, aii, *matrice);

            if ((elem(face, 1)) > -1)
              for (int i = 0; i < ncomp; i++) fill_coeff_matrice_morse < Type_Double > (face, i, ncomp, 1, ajj, *matrice);
          }
    }
}

/* ===================== *
 * ====== FA7 ELEM =====
 * ===================== */
template<class _TYPE_> template <typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_blocs_fa7_elem(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
  const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();
  Type_Double flux(ncomp), aii(ncomp), ajj(ncomp), flux_c(ncomp) /* partie compressible */;
  const int n_fc_bd = la_zone->nb_faces_bord();

  const DoubleTab* a_r = (!is_pb_multi || !is_conv_op_) ? nullptr : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") :
                         &ref_cast(Pb_Multiphase,op_base->equation().probleme()).eq_masse.champ_conserve().valeurs();
//  const IntTab& f_e = la_zone->face_voisins();
  // second membre
  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    for (int fa7 = 0; fa7 < dimension; fa7++)
      {
        int fac1 = elem_faces(num_elem, fa7), fac2 = elem_faces(num_elem, fa7 + dimension);
        flux_evaluateur.template flux_fa7 < Type_Flux_Fa7::ELEM > (inco, a_r, num_elem, fac1, fac2, flux);
        fill_resu_tab < Type_Double > (fac1, fac2, ncomp, flux, secmem);

        if (fac1 < n_fc_bd)
          for (int k = 0; k < ncomp; k++) tab_flux_bords(fac1, orientation(fac1)) += flux[k];

        if (fac2 < n_fc_bd)
          for (int k = 0; k < ncomp; k++) tab_flux_bords(fac2, orientation(fac2)) -= flux[k];
      }

  // derivees : champ convecte
  Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);
  if (matrice)
    {
      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
        for (int fa7 = 0; fa7 < dimension; fa7++)
          {
            const int fac1 = elem_faces(num_elem, fa7), fac2 = elem_faces(num_elem, fa7 + dimension);
            flux_evaluateur.template coeffs_fa7 < Type_Flux_Fa7::ELEM > (a_r, num_elem, fac1, fac2, aii, ajj);
            for (int i = 0; i < ncomp; i++)
              fill_coeff_matrice_morse < Type_Double > (fac1, fac2, i, ncomp, aii, ajj, *matrice);
          }
    }

  // On corrige si cl periodique ...
  corriger_fa7_elem_periodicite<Type_Double>(ncomp, mats, secmem, semi_impl);
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::corriger_fa7_elem_periodicite(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Type_Double flux(ncomp), aii(ncomp), ajj(ncomp);
  const std::string& nom_ch = le_champ_convecte_ou_inc->le_nom().getString();
  const DoubleTab& inco = semi_impl.count(nom_ch) ? semi_impl.at(nom_ch) : le_champ_convecte_ou_inc->valeurs();
  Matrice_Morse *matrice = (is_pb_multi && is_conv_op_) ? (mats.count(nom_ch) && !semi_impl.count(nom_ch) ? mats.at(nom_ch) : nullptr) : (mats.count(nom_ch) ? mats.at(nom_ch) : nullptr);

  for (int num_cl = 0; num_cl < la_zone->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl_perio.frontiere_dis());
          int num_elem, signe, fac1, fac2, ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

          // second membre
          for (int face = ndeb; face < nfin; face++)
            {
              corriger_fa7_elem_periodicite__(face, num_elem, signe, fac1, fac2);

              flux_evaluateur.template flux_fa7 < Type_Flux_Fa7::ELEM > (inco, nullptr, num_elem, fac1, fac2, flux);
              for (int k = 0; k < ncomp; k++) secmem(face, k) += signe * flux[k];
            }

          // derivees : champ convecte
          if (matrice)
            for (int face = ndeb; face < nfin; face++)
              {
                corriger_fa7_elem_periodicite__(face, num_elem, signe, fac1, fac2);

                flux_evaluateur.template coeffs_fa7 < Type_Flux_Fa7::ELEM > (nullptr, num_elem, fac1, fac2, aii, ajj);
                const IntVect& tab1 = (*matrice).get_set_tab1(), &tab2 = (*matrice).get_set_tab2();
                DoubleVect& coeff = (*matrice).get_set_coeff();
                if (signe > 0) /* on a oublie a droite  la contribution de la gauche */
                  {
                    for (int i = 0; i < ncomp; i++)
                      for (int k = tab1[face * ncomp + i] - 1; k < tab1[face * ncomp + 1 + i] - 1; k++)
                        if (tab2[k] - 1 == face * ncomp + i) coeff[k] += aii[i];

                    for (int i = 0; i < ncomp; i++)
                      for (int k = tab1[face * ncomp + i] - 1; k < tab1[face * ncomp + 1 + i] - 1; k++)
                        if (tab2[k] - 1 == fac2 * ncomp + i) coeff[k] -= ajj[i];
                  }
                else /* on a oublie a gauche  la contribution de la droite */
                  {
                    for (int i = 0; i < ncomp; i++)
                      for (int k = tab1[face * ncomp + i] - 1; k < tab1[face * ncomp + 1 + i] - 1; k++)
                        if (tab2[k] - 1 == fac1 * ncomp + i) coeff[k] -= aii[i];

                    for (int i = 0; i < ncomp; i++)
                      for (int k = tab1[face * ncomp + i] - 1; k < tab1[face * ncomp + 1 + i] - 1; k++)
                        if (tab2[k] - 1 == face * ncomp + i) coeff[k] += ajj[i];
                  }
              }
        }
    }
}

template<class _TYPE_>
void Iterateur_VDF_Face<_TYPE_>::corriger_fa7_elem_periodicite__(const int face, int& num_elem, int& signe, int& fac1, int& fac2) const
{
  const int elem1 = elem(face, 0), elem2 = elem(face, 1), ori = orientation(face);
  if ((face == elem_faces(elem1, ori)) || (face == elem_faces(elem1, ori + dimension)))
    {
      num_elem = elem2;
      signe = 1;
    }
  else
    {
      num_elem = elem1;
      signe = -1;
    }
  fac1 = elem_faces(num_elem, ori), fac2 = elem_faces(num_elem, ori + dimension);
}

/* ========================= *
 * ====== Compressible =====
 * ========================= */

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::ajouter_pour_compressible(const int ncomp, matrices_t mats, DoubleTab& secmem, const tabs_t& ) const
{
  // on a secmem calcule comme : div(alpha rho v x v)
  const DoubleTab& vit = le_champ_convecte_ou_inc->valeurs();

  // etape 1 : calcule de la terme div(alpha rho v)
  DoubleTrav unite(secmem), resu(secmem);
  unite  = 1.;
  resu = 0.;

  const tabs_t tabsT = {{ le_champ_convecte_ou_inc->le_nom().getString(), unite}};

  ajouter_blocs_aretes_bords<Type_Double>(ncomp, mats, resu, tabsT);
  ajouter_blocs_aretes_coins<Type_Double>(ncomp, mats, resu, tabsT);
  ajouter_blocs_aretes_internes<Type_Double>(ncomp, mats, resu, tabsT);
  ajouter_blocs_aretes_mixtes<Type_Double>(ncomp, mats, resu, tabsT);
  ajouter_blocs_fa7_sortie_libre<Type_Double>( ncomp, mats, resu, tabsT );
  ajouter_blocs_fa7_elem<Type_Double>( ncomp, mats, resu, tabsT);

  // etape 2 : on multiplie par v pour avoir : v . div(alpha rho v)
  resu *= vit;

  // etape 3 : finalement,  alpha rho v grad v = div(alpha rho v x v) - v . div(alpha rho v)
  secmem -= resu;
}

// ===================================================================================================

template<class _TYPE_> template<typename Type_Double>
inline void Iterateur_VDF_Face<_TYPE_>::fill_resu_tab(const int fac1, const int fac2, const int ncomp, const Type_Double& flux, DoubleTab& resu) const
{
  for (int k = 0; k < ncomp; k++)
    {
      resu(fac1, k) += flux[k];
      resu(fac2, k) -= flux[k];
    }
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::fill_coeff_matrice_morse(const int face, const int i, const int ncomp, const int signe, const Type_Double& A, Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int k = tab1[face * ncomp + i] - 1; k < tab1[face * ncomp + 1 + i] - 1; k++)
    if (tab2[k] - 1 == face * ncomp + i) coeff[k] += signe * A[i]; // equivalent a matrice(face,face) += signe*A(i)
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Face<_TYPE_>::fill_coeff_matrice_morse(const int fac1, const int fac2, const int i, const int ncomp, const Type_Double& A, const Type_Double& B, Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int k = tab1[fac1 * ncomp + i] - 1; k < tab1[fac1 * ncomp + 1 + i] - 1; k++)
    {
      if (tab2[k] - 1 == fac1 * ncomp + i) coeff[k] += A[i]; // equivalent a matrice(fac1,fac1) += A(i)
      if (tab2[k] - 1 == fac2 * ncomp + i) coeff[k] -= B[i]; // equivalent a matrice(fac1,fac2) -= B(i)
    }
  for (int k = tab1[fac2 * ncomp + i] - 1; k < tab1[fac2 * ncomp + 1 + i] - 1; k++)
    {
      if (tab2[k] - 1 == fac1 * ncomp + i) coeff[k] -= A[i]; // equivalent a matrice(fac2,fac1) -= A(i)
      if (tab2[k] - 1 == fac2 * ncomp + i) coeff[k] += B[i]; // equivalent a matrice(fac2,fac2) += B(i)
    }
}

#include <Iterateur_VDF_Face_bis.tpp>

#endif /* Iterateur_VDF_Face_TPP_included */

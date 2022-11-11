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

#ifndef Iterateur_VDF_Face_included
#define Iterateur_VDF_Face_included

#include <CL_Types_Aretes_enum.h>
#include <Schema_Temps_base.h>
#include <Op_Conv_VDF_base.h>
#include <EcrFicPartage.h>

template <class _TYPE_>
class Iterateur_VDF_Face : public Iterateur_VDF_base
{
  inline unsigned taille_memoire() const override { throw; }
  inline int duplique() const override
  {
    Iterateur_VDF_Face* xxx = new  Iterateur_VDF_Face(*this);
    if(!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

public:
  Iterateur_VDF_Face() { }
  Iterateur_VDF_Face(const Iterateur_VDF_Face<_TYPE_>& );

  inline Evaluateur_VDF& evaluateur() override { return static_cast<Evaluateur_VDF&> (flux_evaluateur); }
  inline const Evaluateur_VDF& evaluateur() const override { return static_cast<const Evaluateur_VDF&> (flux_evaluateur); }

  int impr(Sortie& os) const override;
  void completer_() override;

  // INTERFACE  BLOCS
  void ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const override;

protected:
  _TYPE_ flux_evaluateur;
  int nb_elem = -100, premiere_arete_interne = -100, derniere_arete_interne = -100, premiere_arete_mixte = -100, derniere_arete_mixte = -100;
  int premiere_arete_bord = -100, derniere_arete_bord = -100, premiere_arete_coin = -100, derniere_arete_coin = -100;
  mutable SFichier Flux, Flux_moment, Flux_sum;
  IntTab Qdm, elem, elem_faces;
  IntVect orientation, type_arete_bord, type_arete_coin;

private:
  void multiply_by_rho_if_hydraulique(DoubleTab&) const;
  template<typename Type_Double> void fill_resu_tab(const int, const int, const int, const Type_Double&, DoubleTab&) const;
  template<typename Type_Double> void fill_coeff_matrice_morse(const int, const int, const int, const int, const Type_Double&, Matrice_Morse&) const;
  template<typename Type_Double> void fill_coeff_matrice_morse(const int, const int, const int, const int, const Type_Double&, const Type_Double&, Matrice_Morse&) const;

  /* ************************************** *
   * *********  INTERFACE BLOCS  ********** *
   * ********* SFINAE  TEMPLATES ********** *
   * ************************************** */

  /* ====== BORDS ===== */
  template<typename Type_Double>
  void ajouter_blocs_aretes_bords(const int, matrices_t, DoubleTab&, const tabs_t&) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_blocs_aretes_bords_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
  enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  ajouter_blocs_aretes_bords_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
  enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  ajouter_blocs_aretes_bords_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  /* ====== COINS ===== */
  template<typename Type_Double>
  void ajouter_blocs_aretes_coins(const int, matrices_t, DoubleTab&, const tabs_t&) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin, typename Type_Double>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  ajouter_blocs_aretes_coins_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
  enable_if_t<Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  ajouter_blocs_aretes_coins_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  ajouter_blocs_aretes_coins_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  /* ====== INTERNES  & MIXTES ===== */
  template<typename Type_Double>
  void ajouter_blocs_aretes_internes(const int, matrices_t, DoubleTab&, const tabs_t&) const;

  template<typename Type_Double>
  void ajouter_blocs_aretes_mixtes(const int, matrices_t, DoubleTab&, const tabs_t&) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, typename Type_Double>
  enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
  ajouter_blocs_aretes_generique_(const int , const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  /* ====== FA7 SORTIE LIBRE ===== */
  template<typename Type_Double>
  void ajouter_blocs_fa7_sortie_libre(const int, matrices_t, DoubleTab&, const tabs_t&) const;

  template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, typename Type_Double>
  void ajouter_blocs_fa7_sortie_libre_(const int , const int , matrices_t , DoubleTab& , const tabs_t& ) const;

  /* ====== FA7 ELEM ===== */
  template<typename Type_Double>
  void ajouter_blocs_fa7_elem(const int, matrices_t, DoubleTab&, const tabs_t&) const;

  void corriger_fa7_elem_periodicite__(const int, int&, int&, int&, int&) const;
  template<typename Type_Double> void corriger_fa7_elem_periodicite(const int, matrices_t, DoubleTab&, const tabs_t& ) const;
};

#include <Iterateur_VDF_Face.tpp> // templates specializations ici ;)

#endif /* Iterateur_VDF_Face_included */

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
// File:        T_It_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////

#ifndef T_It_VDF_Face_included
#define T_It_VDF_Face_included

#include <CL_Types_Aretes_enum.h>
#include <Iterateur_VDF_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Operateur_base.h>
#include <communications.h>
#include <EcrFicPartage.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <DoubleTrav.h>

template <class _TYPE_>
class T_It_VDF_Face : public Iterateur_VDF_base
{
  inline unsigned taille_memoire() const { throw; }
  inline int duplique() const
  {
    T_It_VDF_Face* xxx = new  T_It_VDF_Face(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

public:
  inline T_It_VDF_Face() { }
  inline T_It_VDF_Face(const T_It_VDF_Face<_TYPE_>& );

  int impr(Sortie& os) const;
  void contribuer_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;

  inline void completer_();
  inline Evaluateur_VDF& evaluateur() { return (Evaluateur_VDF&) flux_evaluateur; }
  inline const Evaluateur_VDF& evaluateur() const { return (Evaluateur_VDF&) flux_evaluateur; }

protected:
  _TYPE_ flux_evaluateur;
  int nb_elem, premiere_arete_interne, derniere_arete_interne, premiere_arete_mixte, derniere_arete_mixte;
  int premiere_arete_bord, derniere_arete_bord, premiere_arete_coin, derniere_arete_coin;
  mutable SFichier Flux, Flux_moment, Flux_sum;
  IntTab Qdm, elem, elem_faces;
  IntVect orientation, type_arete_bord, type_arete_coin;

private:

  /* ************************************** *
   * *********  POUR L'EXPLICITE ********** *
   * ************************************** */

  template <Type_Champ Field_Type> DoubleTab& ajouter_aretes_bords(const DoubleTab&, DoubleTab&, int) const;
  template <Type_Champ Field_Type> DoubleTab& ajouter_aretes_coins(const DoubleTab&, DoubleTab&, int) const;
  template <Type_Champ Field_Type> DoubleTab& ajouter_aretes_internes(const DoubleTab&, DoubleTab&, int) const;
  template <Type_Champ Field_Type> DoubleTab& ajouter_aretes_mixtes(const DoubleTab&, DoubleTab&, int) const;
  template <Type_Champ Field_Type> DoubleTab& ajouter_fa7_sortie_libre(const DoubleTab&, DoubleTab&, int) const;
  template <Type_Champ Field_Type> DoubleTab& ajouter_fa7_elem(const DoubleTab&, DoubleTab&, int) const;
  template <Type_Champ Field_Type> DoubleTab& corriger_flux_fa7_elem_periodicite(const DoubleTab&, DoubleTab& , int ) const;
  template <Type_Champ Field_Type> void corriger_flux_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , const DoubleTab& , DoubleTab& ) const;

  /* Private SFINAE templates */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_aretes_bords_(const int , const int , const int , const DoubleTab& , DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  ajouter_aretes_bords_(const int , const int , const int , const DoubleTab& , DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  ajouter_aretes_coins_(const int , const int , const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  ajouter_aretes_coins_(const int , const int , const int , const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type> void ajouter_aretes_internes_(const int , const int , const DoubleTab& , DoubleTab& ) const;
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type> void ajouter_aretes_mixtes_(const int , const int , const int , const int , const DoubleTab& , DoubleTab& , DoubleTab& ) const;
  template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> void ajouter_fa7_sortie_libre_(const int , const int , const int , const int  , const Neumann_sortie_libre& , const DoubleTab& , DoubleTab& , DoubleTab& ) const;
  template <Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> void ajouter_fa7_elem_(const int , const int , const int , const DoubleTab& , DoubleTab& , DoubleTab& ) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <Type_Champ Field_Type> void contribuer_au_second_membre_aretes_bords(DoubleTab&, int) const;
  template <Type_Champ Field_Type> void contribuer_au_second_membre_aretes_coins(DoubleTab&, int) const;
  template <Type_Champ Field_Type> void contribuer_au_second_membre_aretes_internes(DoubleTab&, int) const;
  template <Type_Champ Field_Type> void contribuer_au_second_membre_aretes_mixtes(DoubleTab&, int) const;
  template <Type_Champ Field_Type> void contribuer_au_second_membre_fa7_sortie_libre(DoubleTab&, int) const;
  template <Type_Champ Field_Type> void contribuer_au_second_membre_fa7_elem(DoubleTab&, int) const;
  template <Type_Champ Field_Type> void corriger_secmem_fa7_elem_periodicite(DoubleTab& , int ) const;
  template <Type_Champ Field_Type> void corriger_secmem_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , DoubleTab& ) const;

  /* Private SFINAE templates */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  contribuer_au_second_membre_aretes_bords_(const int , const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  contribuer_au_second_membre_aretes_bords_(const int , const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  contribuer_au_second_membre_aretes_coins_(const int , const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  contribuer_au_second_membre_aretes_coins_(const int , const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type> void contribuer_au_second_membre_aretes_internes_(const int , const int , DoubleTab& ) const;
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type> void contribuer_au_second_membre_aretes_mixtes_(const int , const int , DoubleTab& ) const;
  template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> void contribuer_au_second_membre_fa7_sortie_libre_(const int , const int , const int , const Neumann_sortie_libre& , DoubleTab& , DoubleTab& ) const;
  template <Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> void contribuer_au_second_membre_fa7_elem_(const int , const int , DoubleTab& ) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <Type_Champ Field_Type> void ajouter_contribution_aretes_bords(const DoubleTab&, Matrice_Morse& , int ) const;
  template <Type_Champ Field_Type> void ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse& , int) const ;
  template <Type_Champ Field_Type> void ajouter_contribution_aretes_internes(const DoubleTab&, Matrice_Morse& , int ) const;
  template <Type_Champ Field_Type> void ajouter_contribution_aretes_mixtes(const DoubleTab&, Matrice_Morse& , int ) const;
  template <Type_Champ Field_Type> void ajouter_contribution_fa7_sortie_libre(const DoubleTab&, Matrice_Morse& , int ) const;
  template <Type_Champ Field_Type> void ajouter_contribution_fa7_elem(const DoubleTab&, Matrice_Morse& , int ) const;
  template <Type_Champ Field_Type> void corriger_coeffs_fa7_elem_periodicite(const DoubleTab&, Matrice_Morse& , int ) const;
  template <Type_Champ Field_Type> void corriger_coeffs_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , const IntVect& , const IntVect& , Matrice_Morse& , DoubleVect& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_contribution_aretes_bords_(const int , const int , const IntVect& , const IntVect& , Matrice_Morse& , DoubleVect& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  ajouter_contribution_aretes_bords_(const int , const int , const IntVect& , const IntVect& , Matrice_Morse& , DoubleVect& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE, void>
  ajouter_contribution_aretes_bords_(const int , const int , const int , const IntVect& , const IntVect& ,  Matrice_Morse& , DoubleVect& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  ajouter_contribution_aretes_coins_(const int , Matrice_Morse& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
  ajouter_contribution_aretes_coins_(const int , Matrice_Morse& ) const;

  template <Type_Flux_Arete Arete_Type, Type_Champ Field_Type> void ajouter_contribution_aretes_internes_(const int , const int , const IntVect& , const IntVect& , Matrice_Morse& , DoubleVect& ) const;
  template <Type_Flux_Arete Arete_Type, Type_Champ Field_Type> void ajouter_contribution_aretes_mixtes_(const int , const int , const IntVect& , const IntVect& , Matrice_Morse& , DoubleVect& ) const;
  template <bool should_calc_flux, Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> void ajouter_contribution_fa7_sortie_libre_(const int , const int , const int , const Neumann_sortie_libre& , const IntVect& , const IntVect& , DoubleVect& ) const;
  template <Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> void ajouter_contribution_fa7_elem_(const int , const int , const IntVect& , const IntVect& , Matrice_Morse& , DoubleVect& ) const;
};

#include <T_It_VDF_Face.tpp> // templates specializations ici ;)

#endif /* T_It_VDF_Face_included */

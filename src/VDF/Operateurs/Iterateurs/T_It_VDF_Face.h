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
  inline T_It_VDF_Face(const T_It_VDF_Face<_TYPE_>&);

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

  void contribuer_au_second_membre_fa7_elem(DoubleTab& ) const;
  void contribuer_au_second_membre_fa7_elem(DoubleTab& , int ) const;
  void contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& ) const;
  void contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& , int ) const;
  void contribuer_au_second_membre_aretes_internes(DoubleTab& ) const;
  void contribuer_au_second_membre_aretes_internes(DoubleTab& , int ) const;
  void contribuer_au_second_membre_aretes_mixtes(DoubleTab& ) const;
  void contribuer_au_second_membre_aretes_mixtes(DoubleTab& , int ) const;
  void contribuer_au_second_membre_aretes_bords(DoubleTab& ) const;
  void contribuer_au_second_membre_aretes_bords(DoubleTab& , int ) const;
  void contribuer_au_second_membre_aretes_coins(DoubleTab& ) const;
  void contribuer_au_second_membre_aretes_coins(DoubleTab&, int) const;
  void ajouter_contribution_fa7_elem(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_fa7_elem(const DoubleTab&, Matrice_Morse& , int ) const;
  void ajouter_contribution_fa7_sortie_libre(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_fa7_sortie_libre(const DoubleTab&, Matrice_Morse& , int ) const;
  void ajouter_contribution_aretes_internes(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_aretes_internes(const DoubleTab&, Matrice_Morse& , int ) const;
  void ajouter_contribution_aretes_mixtes(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_aretes_mixtes(const DoubleTab&, Matrice_Morse& , int ) const;
  void ajouter_contribution_aretes_bords(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_aretes_bords(const DoubleTab&, Matrice_Morse& , int ) const;
  void ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse& ) const ;
  void ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse& , int) const ;
  DoubleTab& ajouter_fa7_elem(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_fa7_elem(const DoubleTab&, DoubleTab& , int ) const;
  DoubleTab& ajouter_fa7_sortie_libre(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_fa7_sortie_libre(const DoubleTab&, DoubleTab& , int ) const;
  DoubleTab& ajouter_aretes_internes(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_aretes_internes(const DoubleTab&, DoubleTab& , int ) const;
  DoubleTab& ajouter_aretes_mixtes(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_aretes_mixtes(const DoubleTab&, DoubleTab& , int ) const;
  DoubleTab& ajouter_aretes_bords(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_aretes_bords(const DoubleTab&, DoubleTab& , int ) const;
  DoubleTab& ajouter_aretes_coins(const DoubleTab&, DoubleTab& ) const;
  DoubleTab& ajouter_aretes_coins(const DoubleTab&, DoubleTab& , int ) const;

private:
  DoubleTab& corriger_flux_fa7_elem_periodicite(const DoubleTab&, DoubleTab& , int ) const;

  template <bool is_VECT> enable_if_t<is_VECT == true, void>
  corriger_flux_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , const DoubleTab& , DoubleTab& ) const;

  template <bool is_VECT> enable_if_t<is_VECT == false, void>
  corriger_flux_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , const DoubleTab& , DoubleTab& ) const;

  void corriger_secmem_fa7_elem_periodicite(DoubleTab& , int ) const;

  template <bool is_VECT> enable_if_t<is_VECT == true, void>
  corriger_secmem_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , DoubleTab& ) const;

  template <bool is_VECT> enable_if_t<is_VECT == false, void>
  corriger_secmem_fa7_elem_periodicite_(const int , const int , const int , const int , const int , const int , DoubleTab& ) const;

  void corriger_coeffs_fa7_elem_periodicite(const DoubleTab&, Matrice_Morse& ) const;
  void corriger_coeffs_fa7_elem_periodicite(const DoubleTab&, Matrice_Morse& , int ) const;

  /* Private SFINAE templates called from ajouter_aretes_bords */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_aretes_bords_(const int , const int , const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  ajouter_aretes_bords_(const int , const int , const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  /* Private SFINAE templates called from ajouter_aretes_bords VECTORIEL version */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_aretes_bords_(const int , const int , const int , const DoubleTab& , DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  ajouter_aretes_bords_(const int , const int , const int , const DoubleTab& , DoubleTab& , DoubleTab& ) const;

  /* Private SFINAE templates called from ajouter_aretes_coins */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  ajouter_aretes_coins_(const int , const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  ajouter_aretes_coins_(const int , const int ,const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  /* Private SFINAE templates called from ajouter_aretes_coins VECTORIEL version */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, bool is_VECT> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI && is_VECT == true, void>
  ajouter_aretes_coins_(const int , const int, const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type, bool is_VECT> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE && is_VECT == true, void>
  ajouter_aretes_coins_(const int , const int , const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  /* Private SFINAE templates called from contribuer_au_second_membre_aretes_bords */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  contribuer_au_second_membre_aretes_bords_(const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  contribuer_au_second_membre_aretes_bords_(const int , DoubleTab& ) const;

  /* Private SFINAE templates called from contribuer_au_second_membre_aretes_bords VECTORIEL version */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  contribuer_au_second_membre_aretes_bords_(const int , const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  contribuer_au_second_membre_aretes_bords_(const int , const int , DoubleTab& ) const;

  /* Private SFINAE templates called from contribuer_au_second_membre_aretes_coins */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  contribuer_au_second_membre_aretes_coins_(const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  contribuer_au_second_membre_aretes_coins_(const int , DoubleTab& ) const;

  /* Private SFINAE templates called from contribuer_au_second_membre_aretes_coins VECTORIEL version */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
  contribuer_au_second_membre_aretes_coins_(const int , const int , DoubleTab& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  contribuer_au_second_membre_aretes_coins_(const int , const int , DoubleTab& ) const;

  /* Private SFINAE templates called from ajouter_contribution_aretes_bords */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_contribution_aretes_bords_(const int , Matrice_Morse& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  ajouter_contribution_aretes_bords_(const int , Matrice_Morse& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  ajouter_contribution_aretes_bords_(const int , const int , Matrice_Morse& ) const;

  /* Private SFINAE templates called from ajouter_contribution_aretes_bords VECTORIEL version */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  ajouter_contribution_aretes_bords_(const int , const int , const IntVect& , const IntVect& , DoubleVect& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
  enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
  ajouter_contribution_aretes_bords_(const int , const int , const IntVect& , const IntVect& , DoubleVect& ) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE, void>
  ajouter_contribution_aretes_bords_(const int , const int , const IntVect& , const IntVect& , DoubleVect& ) const;

  /* Private SFINAE templates called from ajouter_contribution_aretes_coins */
  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  ajouter_contribution_aretes_coins_(const int n_arete, Matrice_Morse& matrice) const;

  template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
  ajouter_contribution_aretes_coins_(const int n_arete, Matrice_Morse& matrice) const;
};

template <class _TYPE_>
inline T_It_VDF_Face<_TYPE_>::T_It_VDF_Face(const T_It_VDF_Face<_TYPE_>& iter) :
  Iterateur_VDF_base(iter), flux_evaluateur(iter.flux_evaluateur), nb_elem(iter.nb_elem),
  premiere_arete_interne(iter.premiere_arete_interne), derniere_arete_interne(iter.derniere_arete_interne),
  premiere_arete_mixte(iter.premiere_arete_mixte), derniere_arete_mixte(iter.derniere_arete_mixte),
  premiere_arete_bord(iter.premiere_arete_bord), derniere_arete_bord(iter.derniere_arete_bord),
  premiere_arete_coin(iter.premiere_arete_coin), derniere_arete_coin(iter.derniere_arete_coin)
{
  orientation.ref(iter.orientation);
  Qdm.ref(iter.Qdm);
  elem.ref(iter.elem);
  elem_faces.ref(iter.elem_faces);
  type_arete_bord.ref(iter.type_arete_bord);
  type_arete_coin.ref(iter.type_arete_coin);
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3);
  const int ncomp = inco.line_size() ;
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  tab_flux_bords.resize(la_zone->nb_faces_bord(),dimension);
  tab_flux_bords=0;
  if( ncomp == 1) // TODO : FIXME : a virer ! plus besoin de cas scalire !
    {
      ajouter_aretes_bords(inco, resu) ;
      ajouter_aretes_coins(inco, resu) ;
      ajouter_aretes_internes(inco,resu);
      ajouter_aretes_mixtes(inco,resu);
      ajouter_fa7_sortie_libre(inco,resu);
      ajouter_fa7_elem(inco,resu);
    }
  else
    {
      ajouter_aretes_bords(inco, resu, ncomp) ;
      ajouter_aretes_coins(inco, resu, ncomp) ;
      ajouter_aretes_internes(inco, resu, ncomp) ;
      ajouter_aretes_mixtes(inco,resu, ncomp);
      ajouter_fa7_sortie_libre(inco,resu, ncomp);
      ajouter_fa7_elem(inco,resu, ncomp);
    }
  /* On multiplie les flux au bord par rho en hydraulique (sert uniquement a la sortie) */
  Nom nom_eqn=la_zcl->equation().que_suis_je();
  int face,k;
  /* Modif B.Mathieu pour front-tracking: masse_volumique() invalide en f.t.*/
  if (nom_eqn.debute_par("Navier_Stokes") && nom_eqn!="Navier_Stokes_Melange" && nom_eqn!="Navier_Stokes_FT_Disc")
    {
      const Champ_base& rho = la_zcl->equation().milieu().masse_volumique();
      if (sub_type(Champ_Uniforme,rho))
        {
          double coef = rho(0,0);
          int nb_faces_bord=la_zone->nb_faces_bord();
          for (face=0; face<nb_faces_bord; face++)
            for(k=0; k<tab_flux_bords.line_size(); k++) tab_flux_bords(face,k) *= coef;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords(const DoubleTab& inco, DoubleTab& resu) const
{
  if (!_TYPE_::CALC_ARR_BORD) return resu;
  int n_arete, n_type, n = la_zone->nb_faces_bord();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for (n_arete=premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++)
    {
      n_type = type_arete_bord(n_arete-premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords(const DoubleTab& inco, DoubleTab& resu,int ncomp) const
{
  if (!_TYPE_::CALC_ARR_BORD) return resu;
  int n_arete, n_type, n = la_zone->nb_faces_bord();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  for (n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,n,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE>(n_arete,n,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE>(n_arete,n,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins(const DoubleTab& inco, DoubleTab& resu) const
{
  int n_arete, n_type, n = la_zone->nb_faces_bord();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  for (n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PERIO_PAROI:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,TypeAreteCoinVDF::PERIO_PAROI>(n_arete,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::PAROI_FLUIDE:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,TypeAreteCoinVDF::PAROI_FLUIDE>(n_arete,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,TypeAreteCoinVDF::FLUIDE_PAROI>(n_arete,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::PERIO_PERIO:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_COIN_FL,Type_Flux_Arete::COIN_FLUIDE>(n_arete,n,inco,resu,tab_flux_bords);
          break;
        default :
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins(const DoubleTab& inco, DoubleTab& resu,int ncomp) const
{
  int n_arete, n_type;
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  for (n_arete=premiere_arete_coin; n_arete<derniere_arete_coin; n_arete++)
    {
      n_type=type_arete_coin(n_arete-premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PERIO_PERIO:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE,true>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          ajouter_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI,true>(n_arete,ncomp,inco,resu,tab_flux_bords);
          break;
        default :
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_internes(const DoubleTab& inco, DoubleTab& resu) const
{
  if(!_TYPE_::CALC_ARR_INT) return resu;
  double flux;
  int n_arete;
  for (n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux = flux_evaluateur.template flux_arete<Type_Flux_Arete::INTERNE>(inco, fac1, fac2, fac3, fac4);
      resu(fac3) += flux;
      resu(fac4) -= flux;
      flux = flux_evaluateur.template flux_arete<Type_Flux_Arete::INTERNE>(inco, fac3, fac4, fac1, fac2);
      resu(fac1) += flux;
      resu(fac2) -= flux;
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_internes(const DoubleTab& inco, DoubleTab& resu,int ncomp) const
{
  if(!_TYPE_::CALC_ARR_INT) return resu;
  DoubleVect flux(ncomp);
  int n_arete,k;
  for (n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Type_Flux_Arete::INTERNE>(inco, fac1, fac2, fac3, fac4,flux);
      for (k=0; k<ncomp; k++)
        {
          resu(fac3,k) += flux(k);
          resu(fac4,k) -= flux(k);
        }
      flux_evaluateur.template flux_arete<Type_Flux_Arete::INTERNE>(inco, fac3, fac4, fac1, fac2, flux);
      for (k=0; k<ncomp; k++)
        {
          resu(fac1,k) += flux(k);
          resu(fac2,k) -= flux(k);
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_mixtes(const DoubleTab& inco, DoubleTab& resu) const
{
  if(!_TYPE_::CALC_ARR_MIXTE) return resu;
  double flux;
  int n_arete, n=la_zone->nb_faces_bord(), n2=la_zone->nb_faces_tot();  /* GF pour assurer bilan seq = para */
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for (n_arete = premiere_arete_mixte; n_arete < derniere_arete_mixte; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux = flux_evaluateur.template flux_arete<Type_Flux_Arete::MIXTE>(inco, fac1, fac2, fac3, fac4);
      resu(fac3) += flux;
      resu(fac4) -= flux;
      if (fac4 < n2)
        {
          if (fac1 < n) tab_flux_bords(fac1,orientation(fac3)) -= flux;
          if (fac2 < n) tab_flux_bords(fac2,orientation(fac4)) -= flux;
        }
      if (fac3 < n2)
        {
          if (fac1 < n) tab_flux_bords(fac1,orientation(fac3)) += flux;
          if (fac2 < n) tab_flux_bords(fac2,orientation(fac4)) += flux;
        }
      flux = flux_evaluateur.template flux_arete<Type_Flux_Arete::MIXTE>(inco, fac3, fac4, fac1, fac2);
      resu(fac1) += flux;
      resu(fac2) -= flux;
      if (fac2 < n2)
        {
          if (fac3 < n) tab_flux_bords(fac3,orientation(fac1)) -= flux;
          if (fac4 < n) tab_flux_bords(fac4,orientation(fac2)) -= flux;
        }
      if (fac1 < n2)
        {
          if (fac3 < n) tab_flux_bords(fac3,orientation(fac1)) += flux;
          if (fac4 < n) tab_flux_bords(fac4,orientation(fac2)) += flux;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_aretes_mixtes(const DoubleTab& inco, DoubleTab& resu,int ncomp) const
{
  if(!_TYPE_::CALC_ARR_MIXTE) return resu;
  DoubleVect flux(ncomp);
  int n_arete,k;
  for (n_arete = premiere_arete_mixte; n_arete < derniere_arete_mixte; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Type_Flux_Arete::MIXTE>(inco, fac1, fac2, fac3, fac4, flux);
      for (k=0; k<ncomp; k++)
        {
          resu(fac3,k) += flux(k);
          resu(fac4,k) -= flux(k);
        }
      flux_evaluateur.template flux_arete<Type_Flux_Arete::MIXTE>(inco, fac3, fac4, fac1, fac2, flux);
      for (k=0; k<ncomp; k++)
        {
          resu(fac1,k) += flux(k);
          resu(fac2,k) -= flux(k);
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_fa7_sortie_libre(const DoubleTab& inco, DoubleTab& resu) const
{
  const int nb_front_Cl=la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          if (_TYPE_::CALC_FA7_SORTIE_LIB)
            {
              for (int face = ndeb; face < nfin; face++)
                {
                  double flux = flux_evaluateur.template flux_fa7<Type_Flux_Fa7::SORTIE_LIBRE>(inco, face, (const Neumann_sortie_libre&) la_cl.valeur(), ndeb);
                  if ( (elem(face,0)) > -1) resu(face) += flux;
                  if ( (elem(face,1)) > -1) resu(face) -= flux;
                }
            }
          break;
        case symetrie : /* fall through */
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_fa7_sortie_libre(const DoubleTab& inco,DoubleTab& resu,int ncomp) const
{
  const int nb_front_Cl = la_zone->nb_front_Cl(), n = la_zone->nb_faces_bord();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  DoubleVect flux(ncomp);
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          if (_TYPE_::CALC_FA7_SORTIE_LIB)
            {
              for (int face = ndeb; face < nfin; face++)
                {
                  flux_evaluateur.template flux_fa7<Type_Flux_Fa7::SORTIE_LIBRE>(inco, face,(const Neumann_sortie_libre&) la_cl.valeur() , ndeb, flux);
                  if ( (elem(face,0)) > -1) for (int k=0; k<ncomp; k++) resu(face,k) += flux(k);
                  if ( (elem(face,1)) > -1) for (int k=0; k<ncomp; k++) resu(face,k) -= flux(k);

                  for (int k=0; k<ncomp; k++) if (face<n) tab_flux_bords(face,k)-=flux(k);
                }
            }
          break;
        case symetrie :
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_fa7_elem(const DoubleTab& inco, DoubleTab& resu) const
{
  const int n = la_zone->nb_faces_bord();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for(int num_elem = 0 ; num_elem < nb_elem; num_elem++)
    {
      for (int fa7 = 0; fa7 < dimension; fa7++)
        {
          int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
          double flux = flux_evaluateur.template flux_fa7<Type_Flux_Fa7::ELEM>(inco, num_elem, fac1, fac2);
          resu(fac1) += flux;
          resu(fac2) -= flux;
          if (fac1<n) tab_flux_bords(fac1,orientation(fac1)) += flux;
          if (fac2<n) tab_flux_bords(fac2,orientation(fac2)) -= flux;
        }
    }
  corriger_flux_fa7_elem_periodicite(inco,resu, -1 /* Scalaire */); // TODO : FIXME : a virer ...
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::ajouter_fa7_elem(const DoubleTab& inco, DoubleTab& resu,int ncomp) const
{
  int fa7, fac1, fac2;
  int num_elem;
  int k;
  DoubleVect flux(ncomp);
  for(num_elem=0; num_elem<nb_elem; num_elem++)
    {
      for (fa7=0; fa7<dimension; fa7++)
        {
          fac1=elem_faces(num_elem,fa7);
          fac2=elem_faces(num_elem,fa7+dimension);
          flux_evaluateur.template flux_fa7<Type_Flux_Fa7::ELEM>(inco, num_elem, fac1, fac2, flux);
          for (k=0; k<ncomp; k++)
            {
              resu(fac1,k) += flux(k);
              resu(fac2,k) -= flux(k);
            }
        }
    }
  corriger_flux_fa7_elem_periodicite(inco,resu,ncomp);
  return resu;
}

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::completer_()
{
  nb_elem=la_zone->nb_elem_tot();
  orientation.ref(la_zone->orientation());
  Qdm.ref(la_zone->Qdm());
  elem.ref(la_zone->face_voisins());
  elem_faces.ref(la_zone->elem_faces());
  type_arete_bord.ref(la_zcl->type_arete_bord());
  type_arete_coin.ref(la_zcl->type_arete_coin());
  premiere_arete_interne=la_zone->premiere_arete_interne();
  derniere_arete_interne=premiere_arete_interne+la_zone->nb_aretes_internes();
  premiere_arete_mixte=la_zone->premiere_arete_mixte();
  derniere_arete_mixte=premiere_arete_mixte+la_zone->nb_aretes_mixtes();
  premiere_arete_bord=la_zone->premiere_arete_bord();
  derniere_arete_bord=premiere_arete_bord+la_zone->nb_aretes_bord();
  premiere_arete_coin=la_zone->premiere_arete_coin();
  derniere_arete_coin=premiere_arete_coin+la_zone->nb_aretes_coin();
}

template <class _TYPE_>
int T_It_VDF_Face<_TYPE_>::impr(Sortie& os) const
{
  const Zone& ma_zone=la_zone->zone();
  const int& impr_mom=ma_zone.Moments_a_imprimer();
  const int impr_sum=(ma_zone.Bords_a_imprimer_sum().est_vide() ? 0:1), impr_bord=(ma_zone.Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = la_zcl->equation().probleme().schema_temps();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  DoubleVect bilan(tab_flux_bords.dimension(1));
  const int nb_faces = la_zone->nb_faces_tot();
  DoubleTab xgr(nb_faces,dimension);
  xgr=0.;
  if (impr_mom)
    {
      const DoubleTab& xgrav = la_zone->xv();
      const ArrOfDouble& c_grav=ma_zone.cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }
  int k,face, nb_front_Cl=la_zone->nb_front_Cl();
  DoubleTrav flux_bords2( 5, nb_front_Cl , tab_flux_bords.dimension(1)) ;
  flux_bords2=0;
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces(), periodicite = (type_cl(la_cl)==periodique?1:0);
      for (face=ndeb; face<nfin; face++)
        {
          for(k=0; k<tab_flux_bords.dimension(1); k++)
            {
              flux_bords2(0,num_cl,k)+=tab_flux_bords(face, k);
              if(periodicite)
                {
                  if( face < (ndeb+frontiere_dis.nb_faces()/2) ) flux_bords2(1,num_cl,k)+=tab_flux_bords(face, k);
                  else flux_bords2(2,num_cl,k)+=tab_flux_bords(face, k);
                }
              if (ma_zone.Bords_a_imprimer_sum().contient(frontiere_dis.le_nom())) flux_bords2(3,num_cl,k)+=tab_flux_bords(face, k);
            }  /* fin for k */
          if (impr_mom)
            {
              if (dimension==2) flux_bords2(4,num_cl,0)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1);
              else
                {
                  flux_bords2(4,num_cl,0)+=tab_flux_bords(face,2)*xgr(face,1)-tab_flux_bords(face,1)*xgr(face,2);
                  flux_bords2(4,num_cl,1)+=tab_flux_bords(face,0)*xgr(face,2)-tab_flux_bords(face,2)*xgr(face,0);
                  flux_bords2(4,num_cl,2)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1);
                }
            }
        } /* fin for face */
    }
  mp_sum_for_each_item(flux_bords2);
  if (je_suis_maitre())
    {
      //SFichier Flux;
      if (!Flux.is_open()) op_base->ouvrir_fichier(Flux,"",1);
      //SFichier Flux_moment;
      if (!Flux_moment.is_open()) op_base->ouvrir_fichier(Flux_moment,"moment",impr_mom);
      //SFichier Flux_sum;
      if (!Flux_sum.is_open()) op_base->ouvrir_fichier(Flux_sum,"sum",impr_sum);
      Flux.add_col(sch.temps_courant());
      if (impr_mom) Flux_moment.add_col(sch.temps_courant());
      if (impr_sum) Flux_sum.add_col(sch.temps_courant());
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          int periodicite = (type_cl(la_cl)==periodique?1:0);
          for(k=0; k<tab_flux_bords.dimension(1); k++)
            {
              if(periodicite)
                {
                  Flux.add_col(flux_bords2(1,num_cl,k));
                  Flux.add_col(flux_bords2(2,num_cl,k));
                }
              else Flux.add_col(flux_bords2(0,num_cl,k));

              if (impr_sum) Flux_sum.add_col(flux_bords2(3,num_cl,k));
              bilan(k)+=flux_bords2(0,num_cl,k);
            }
          if (dimension==3)
            {
              for (k=0; k<tab_flux_bords.dimension(1); k++) if (impr_mom) Flux_moment.add_col(flux_bords2(4,num_cl,k));
            }
          else if (impr_mom) Flux_moment.add_col(flux_bords2(4,num_cl,0));
        } /* fin for num_cl */
      for(k=0; k<tab_flux_bords.dimension(1); k++) Flux.add_col(bilan(k));
      Flux << finl;
      if (impr_sum) Flux_sum << finl;
      if (impr_mom) Flux_moment << finl;
    }
  const LIST(Nom)& Liste_Bords_a_imprimer = la_zone->zone().Bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_face;
      op_base->ouvrir_fichier_partage(Flux_face,"",impr_bord);
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = la_zcl->les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
          if (ma_zone.Bords_a_imprimer().contient(la_fr.le_nom()))
            {
              if(je_suis_maitre())
                {
                  Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_face);
                  Flux_face << " : " << finl;
                }
              for (face=ndeb; face<nfin; face++)
                {
                  if (dimension == 2) Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " : ";
                  else if (dimension == 3) Flux_face << "# Face a x= " << la_zone->xv(face,0) << " y= " << la_zone->xv(face,1) << " z= " << la_zone->xv(face,2) << " : ";
                  for(k=0; k<tab_flux_bords.dimension(1); k++) Flux_face << tab_flux_bords(face, k) << " ";
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre(DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(resu.nb_dim() < 3);
  const int ncomp = resu.line_size();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  if (la_zone->nb_faces_bord() >0)
    {
      tab_flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
      tab_flux_bords=0;
    }
  if( ncomp == 1) // TODO : FIXME : a virer :-)
    {
      contribuer_au_second_membre_aretes_bords(resu) ;
      contribuer_au_second_membre_aretes_coins(resu) ;
      contribuer_au_second_membre_aretes_internes(resu);
      contribuer_au_second_membre_aretes_mixtes(resu);
      contribuer_au_second_membre_fa7_sortie_libre(resu);
      contribuer_au_second_membre_fa7_elem(resu);
    }
  else
    {
      contribuer_au_second_membre_aretes_bords(resu, ncomp) ;
      contribuer_au_second_membre_aretes_coins(resu,ncomp) ;
      contribuer_au_second_membre_aretes_internes(resu, ncomp) ;
      contribuer_au_second_membre_aretes_mixtes(resu, ncomp);
      contribuer_au_second_membre_fa7_sortie_libre(resu, ncomp);
      contribuer_au_second_membre_fa7_elem(resu, ncomp);
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords(DoubleTab& resu) const
{
  for (int n_arete = premiere_arete_bord; n_arete<derniere_arete_bord; n_arete++)
    {
      int n_type=type_arete_bord(n_arete-premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,resu);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE>(n_arete,resu);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI>(n_arete,resu);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE>(n_arete,resu);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE>(n_arete,resu);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,resu);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE>(n_arete,resu);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords(DoubleTab& resu,int ncomp) const
{
  for (int n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,ncomp,resu);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          contribuer_au_second_membre_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE>(n_arete,ncomp,resu);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins( DoubleTab& resu) const
{
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PERIO_PAROI:
        case TypeAreteCoinVDF::PAROI_FLUIDE:
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,resu);
          break;
        case TypeAreteCoinVDF::PERIO_PERIO:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,resu);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_COIN_FL,Type_Flux_Arete::COIN_FLUIDE>(n_arete,resu);
          break;
        default :
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins(
  DoubleTab& resu,int ncomp) const
{
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PERIO_PERIO:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,ncomp,resu);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          contribuer_au_second_membre_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,ncomp,resu);
          break;
        default :
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_internes(DoubleTab& resu) const
{
  double flux;
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux = flux_evaluateur.template secmem_arete<Type_Flux_Arete::INTERNE>(fac1, fac2, fac3, fac4);
      resu(fac3) += flux;
      resu(fac4) -= flux;
      flux = flux_evaluateur.template secmem_arete<Type_Flux_Arete::INTERNE>(fac3, fac4, fac1, fac2);
      resu(fac1) += flux;
      resu(fac2) -= flux;
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_internes(DoubleTab& resu,int ncomp) const
{
  DoubleVect flux(ncomp);
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Type_Flux_Arete::INTERNE>(fac1, fac2, fac3, fac4,flux);
      for (int k=0; k<ncomp; k++)
        {
          resu(fac3,k) += flux(k);
          resu(fac4,k) -= flux(k);
        }
      flux_evaluateur.template secmem_arete<Type_Flux_Arete::INTERNE>(fac3, fac4, fac1, fac2, flux);
      for (int k=0; k<ncomp; k++)
        {
          resu(fac1,k) += flux(k);
          resu(fac2,k) -= flux(k);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_mixtes(DoubleTab& resu) const
{
  double flux;
  for (int n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux = flux_evaluateur.template secmem_arete<Type_Flux_Arete::MIXTE>(fac1, fac2, fac3, fac4);
      resu(fac3) += flux;
      resu(fac4) -= flux;
      flux = flux_evaluateur.template secmem_arete<Type_Flux_Arete::MIXTE>(fac3, fac4, fac1, fac2);
      resu(fac1) += flux;
      resu(fac2) -= flux;
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_mixtes(DoubleTab& resu,int ncomp) const
{
  DoubleVect flux(ncomp);
  for (int n_arete=premiere_arete_mixte; n_arete<derniere_arete_mixte; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Type_Flux_Arete::MIXTE>(fac1, fac2, fac3, fac4, flux);
      for (int k=0; k<ncomp; k++)
        {
          resu(fac3,k) += flux(k);
          resu(fac4,k) -= flux(k);
        }
      flux_evaluateur.template secmem_arete<Type_Flux_Arete::MIXTE>(fac3, fac4, fac1, fac2, flux);
      for (int k=0; k<ncomp; k++)
        {
          resu(fac1,k) += flux(k);
          resu(fac2,k) -= flux(k);
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& resu) const
{
  const int nb_front_Cl=la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          if (_TYPE_::CALC_FA7_SORTIE_LIB)
            {
              for (int face = ndeb; face < nfin; face++)
                {
                  double flux = flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::SORTIE_LIBRE>(face, (const Neumann_sortie_libre&) la_cl.valeur(), ndeb);
                  if ( (elem(face,0)) > -1) resu(face) += flux;
                  if ( (elem(face,1)) > -1) resu(face) -= flux;
                }
            }
          break;
        case symetrie :
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_sortie_libre(DoubleTab& resu, int ncomp) const
{
  DoubleVect flux(ncomp);
  int nb_front_Cl = la_zone->nb_front_Cl();
  DoubleTab& tab_flux_bords = op_base->flux_bords();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          if (_TYPE_::CALC_FA7_SORTIE_LIB)
            {
              for (int face = ndeb; face < nfin; face++)
                {
                  flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::SORTIE_LIBRE>(face, (const Neumann_sortie_libre&) la_cl.valeur(), ndeb, flux);
                  if ( (elem(face,0)) > -1) for (int k=0; k<ncomp; k++) resu(face,k) += flux(k);
                  if ( (elem(face,1)) > -1) for (int k=0; k<ncomp; k++) resu(face,k) -= flux(k);
                  for (int k=0; k<ncomp; k++) tab_flux_bords(face,k) -= flux(k);
                }
            }
          break;
        case symetrie :
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_elem(DoubleTab& resu) const
{
  for( int num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      for (int fa7 = 0; fa7 < dimension; fa7++)
        {
          const int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
          double flux = flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::ELEM>(num_elem, fac1, fac2);
          resu(fac1) += flux;
          resu(fac2) -= flux;
        }
    }
  corriger_secmem_fa7_elem_periodicite(resu, -1 /* Scalaire */); // TODO : FIXME : a virer ...
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_fa7_elem(DoubleTab& resu, int ncomp) const
{
  DoubleVect flux(ncomp);
  for(int num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      for (int fa7 = 0; fa7 < dimension; fa7++)
        {
          const int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
          flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::ELEM>(num_elem, fac1, fac2, flux);
          for (int k = 0; k < ncomp; k++)
            {
              resu(fac1,k) += flux(k);
              resu(fac2,k) -= flux(k);
            }
        }
    }
  corriger_secmem_fa7_elem_periodicite(resu,ncomp);
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3);
  const int ncomp = inco.line_size();
  DoubleTab& tab_flux_bords=op_base->flux_bords();
  tab_flux_bords.resize(la_zone->nb_faces_bord(),ncomp);
  tab_flux_bords = 0;
  if( ncomp == 1)
    {
      ajouter_contribution_aretes_bords(inco, matrice) ;
      ajouter_contribution_aretes_coins(inco, matrice) ;
      ajouter_contribution_aretes_internes(inco, matrice );
      ajouter_contribution_aretes_mixtes(inco, matrice);
      ajouter_contribution_fa7_sortie_libre(inco, matrice);
      ajouter_contribution_fa7_elem(inco, matrice);
    }
  else
    {
      ajouter_contribution_aretes_bords(inco, matrice, ncomp) ;
      ajouter_contribution_aretes_coins(inco, matrice, ncomp) ;
      ajouter_contribution_aretes_internes(inco, matrice, ncomp) ;
      ajouter_contribution_aretes_mixtes(inco, matrice, ncomp);
      ajouter_contribution_fa7_sortie_libre(inco, matrice, ncomp);
      ajouter_contribution_fa7_elem(inco, matrice, ncomp);
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  Cerr << "ajouter_contribution_vitesse should not be called from " <<  op_base.que_suis_je() << finl;
  abort();
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  const int nb_face_reelle = la_zone->nb_faces();
  for (int n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,matrice);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE>(n_arete,matrice);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI>(n_arete,matrice);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE>(n_arete,matrice);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE>(n_arete,matrice);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE>(n_arete,matrice);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,nb_face_reelle,matrice);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
        }
    }
}
template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords(const DoubleTab& inco, Matrice_Morse& matrice ,int ncomp) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int n_arete = premiere_arete_bord; n_arete < derniere_arete_bord; n_arete++)
    {
      int n_type = type_arete_bord(n_arete - premiere_arete_bord);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_FL,Type_Flux_Arete::FLUIDE>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        case TypeAreteBordVDF::PAROI_FLUIDE:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PAR_FL,Type_Flux_Arete::PAROI_FLUIDE>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        case TypeAreteBordVDF::SYM_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM,Type_Flux_Arete::SYMETRIE>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        case TypeAreteBordVDF::PERIO_PERIO:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        case TypeAreteBordVDF::PAROI_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM_PAR,Type_Flux_Arete::SYMETRIE_PAROI>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        case TypeAreteBordVDF::FLUIDE_SYM:
          ajouter_contribution_aretes_bords_<_TYPE_::CALC_ARR_SYMM_FL,Type_Flux_Arete::SYMETRIE_FLUIDE>(n_arete,ncomp,tab1,tab2,coeff);
          break;
        default :
          Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
          Process::exit();
        }
    }
}
template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins(const DoubleTab& inco , Matrice_Morse& matrice ) const
{
  for (int n_arete = premiere_arete_coin; n_arete < derniere_arete_coin; n_arete++)
    {
      int n_type = type_arete_coin(n_arete - premiere_arete_coin);
      switch(n_type)
        {
        case TypeAreteCoinVDF::PERIO_PERIO:
          ajouter_contribution_aretes_coins_<_TYPE_::CALC_ARR_PERIO,Type_Flux_Arete::PERIODICITE>(n_arete,matrice);
          break;
        case TypeAreteCoinVDF::PERIO_PAROI:
          ajouter_contribution_aretes_coins_<_TYPE_::CALC_ARR_PAR,Type_Flux_Arete::PAROI>(n_arete,matrice);
          break;
        case TypeAreteCoinVDF::FLUIDE_FLUIDE:
          ajouter_contribution_aretes_coins_<true,Type_Flux_Arete::COIN_FLUIDE>(n_arete,matrice);
          break;
        case TypeAreteCoinVDF::PAROI_FLUIDE:
        case TypeAreteCoinVDF::FLUIDE_PAROI:
          Process::exit(); /* pas fini */
          break;
        default :
          break;
        }
    }
}

template <class _TYPE_>
void   T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins(const DoubleTab&, Matrice_Morse&, int ) const
{
  Cerr<<"non code "<<finl;
  Process::exit();
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_internes(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if(!_TYPE_::CALC_ARR_INT) return;
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++)
    ajouter_contribution_aretes_coins_<true,Type_Flux_Arete::INTERNE>(n_arete,matrice); /* meme codage que le cas PERIODICITE */
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_internes(const DoubleTab& inco, Matrice_Morse& matrice, int ncomp) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int n_arete = premiere_arete_interne; n_arete < derniere_arete_interne; n_arete++)
    ajouter_contribution_aretes_bords_<true,Type_Flux_Arete::INTERNE>(n_arete,ncomp,tab1,tab2,coeff); /* meme codage que le cas PERIODICITE */
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_mixtes(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if(!_TYPE_::CALC_ARR_MIXTE) return ;
  for (int n_arete = premiere_arete_mixte; n_arete < derniere_arete_mixte; n_arete++)
    ajouter_contribution_aretes_coins_<true,Type_Flux_Arete::MIXTE>(n_arete,matrice); /* meme codage que le cas PERIODICITE */
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_mixtes(const DoubleTab& inco, Matrice_Morse& matrice, int ncomp) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  DoubleVect aii(ncomp), ajj(ncomp);
  for (int n_arete = premiere_arete_mixte; n_arete < derniere_arete_mixte; n_arete++)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Type_Flux_Arete::MIXTE>(fac1, fac2, fac3, fac4, aii, ajj); /* XXX : codage pas identique a ce qu'on a dans ajouter_contribution_aretes_bords_ PERIODICITE */
      for (int i = 0; i < ncomp; i++ )
        {
          for (int k = tab1[fac1*ncomp+i]-1; k < tab1[fac1*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac1*ncomp+i) coeff[k] += aii(i);
              if (tab2[k]-1 == fac2*ncomp+i) coeff[k] -= ajj(i);
            }
          for (int k = tab1[fac2*ncomp+i]-1; k < tab1[fac2*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac1*ncomp+i) coeff[k] -= aii(i);
              if (tab2[k]-1 == fac2*ncomp+i) coeff[k] += ajj(i);
            }
        }
      flux_evaluateur.template coeffs_arete<Type_Flux_Arete::MIXTE>(fac3, fac4, fac1, fac2, aii, ajj);
      for (int i = 0; i < ncomp; i++ )
        {
          for (int k = tab1[fac3*ncomp+i]-1; k < tab1[fac3*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac3*ncomp+i) coeff[k] += aii(i);
              if (tab2[k]-1 == fac4*ncomp+i) coeff[k*i] -= ajj(i); /* XXX : BUG peut etre ? on a pas ca dans ajouter_contribution_aretes_bords_ PERIODICITE */
            }
          for (int k = tab1[fac4*ncomp+i]-1; k < tab1[fac4*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac3*ncomp+i) coeff[k] -= aii(i);
              if (tab2[k]-1 == fac4*ncomp+i) coeff[k] += ajj(i);
            }
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_sortie_libre(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  const int nb_front_Cl = la_zone->nb_front_Cl();
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  double aii=0, ajj=0;
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          if (_TYPE_::CALC_FA7_SORTIE_LIB)
            {
              for (int face = ndeb; face < nfin; face++)
                {
                  flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::SORTIE_LIBRE>(face, (const Neumann_sortie_libre&) la_cl.valeur(), aii, ajj);
                  if ( (elem(face,0)) > -1) for (int k = tab1[face]-1; k < tab1[face+1]-1; k++) if (tab2[k]-1 == face) coeff[k] += aii;
                  if ( (elem(face,1)) > -1) for (int k = tab1[face]-1; k < tab1[face+1]-1; k++) if (tab2[k]-1 == face) coeff[k] += ajj;
                }
            }
          break;
        case symetrie :
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi:
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_sortie_libre(const DoubleTab& inco, Matrice_Morse& matrice , int ncomp) const
{
  const int nb_front_Cl = la_zone->nb_front_Cl();
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  DoubleVect aii(ncomp), ajj(ncomp);
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case sortie_libre :
          if (_TYPE_::CALC_FA7_SORTIE_LIB)
            {
              for (int face = ndeb; face < nfin; face++)
                {
                  flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::SORTIE_LIBRE>(face, (const Neumann_sortie_libre&) la_cl.valeur(), aii, ajj);
                  if ( (elem(face,0)) > -1) for (int i = 0; i < ncomp; i++ )
                      for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1 == face*ncomp+i) coeff[k] += aii(i);

                  if ( (elem(face,1)) > -1) for (int i = 0; i < ncomp; i++ )
                      for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1 == face*ncomp+i) coeff[k] += ajj(i);
                }
            }
          break;
        case symetrie :
        case entree_fluide :
        case paroi_fixe :
        case paroi_defilante :
        case paroi_adiabatique :
        case paroi :
        case echange_externe_impose :
        case echange_global_impose :
        case periodique :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Process::exit();
          break;
        }
    }
}
template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_elem(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  double aii=0, ajj=0;
  for(int num_elem = 0 ; num_elem < nb_elem; num_elem++)
    {
      for (int fa7 = 0; fa7 < dimension; fa7++)
        {
          const int fac1=elem_faces(num_elem,fa7), fac2=elem_faces(num_elem,fa7+dimension);
          flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::ELEM>(num_elem, fac1, fac2, aii, ajj);
          matrice(fac1,fac1) += aii;
          matrice(fac1,fac2) -= ajj;
          matrice(fac2,fac1) -= aii;
          matrice(fac2,fac2) += ajj;
        }
    }
  corriger_coeffs_fa7_elem_periodicite(inco, matrice);
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::ajouter_contribution_fa7_elem(const DoubleTab& inco, Matrice_Morse& matrice, int ncomp ) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  DoubleVect aii(ncomp), ajj(ncomp);
  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      for (int fa7 = 0; fa7 < dimension; fa7++)
        {
          const int fac1 = elem_faces(num_elem,fa7), fac2 = elem_faces(num_elem,fa7+dimension);
          flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::ELEM>(num_elem, fac1, fac2, aii, ajj);
          for (int i = 0; i < ncomp; i++ )
            {
              for (int k = tab1[fac1*ncomp+i]-1; k < tab1[fac1*ncomp+1+i]-1; k++)
                {
                  if (tab2[k]-1 == fac1*ncomp+i) coeff[k] += aii(i);
                  if (tab2[k]-1 == fac2*ncomp+i) coeff[k] -= ajj(i);
                }
              for (int k = tab1[fac2*ncomp+i]-1; k < tab1[fac2*ncomp+1+i]-1; k++)
                {
                  if (tab2[k]-1 == fac1*ncomp+i) coeff[k] -= aii(i);
                  if (tab2[k]-1 == fac2*ncomp+i) coeff[k] += ajj(i);
                }
            }
        }
    }
  corriger_coeffs_fa7_elem_periodicite(inco, matrice, ncomp);
}

/* ************************ *
 * Internal private methods *
 * ************************ */

template <class _TYPE_>
DoubleTab& T_It_VDF_Face<_TYPE_>::corriger_flux_fa7_elem_periodicite(const DoubleTab& inco, DoubleTab& resu, int ncomp) const
{
  const int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          int num_elem, signe;
          for (int face=ndeb; face < nfin; face++)
            {
              int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              // TODO : FIXME : a virer ce truc ...
              (ncomp == -1) ? corriger_flux_fa7_elem_periodicite_<false>(ncomp,num_elem,fac1,fac2,face,signe,inco,resu) :
              corriger_flux_fa7_elem_periodicite_<true>(ncomp,num_elem,fac1,fac2,face,signe,inco,resu);
            }
        }
    }
  return resu;
}

template <class _TYPE_> template <bool is_VECT> enable_if_t<is_VECT == true, void>
T_It_VDF_Face<_TYPE_>::corriger_flux_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, const DoubleTab& inco, DoubleTab& resu) const
{
  DoubleVect flux(ncomp);
  flux_evaluateur.template flux_fa7<Type_Flux_Fa7::ELEM>(inco,num_elem,fac1,fac2,flux);
  for (int k=0; k<ncomp; k++) resu(face,k) += signe*flux(k);
}

template <class _TYPE_> template <bool is_VECT> enable_if_t<is_VECT == false, void>
T_It_VDF_Face<_TYPE_>::corriger_flux_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, const DoubleTab& inco, DoubleTab& resu) const
{
  assert (ncomp == -1);
  double flux=flux_evaluateur.template flux_fa7<Type_Flux_Fa7::ELEM>(inco,num_elem,fac1,fac2);
  resu(face) += signe*flux;
}

// TODO : FIXME : a factorizer avec corriger_flux_fa7_elem_periodicite !!
template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::corriger_secmem_fa7_elem_periodicite(DoubleTab& resu, int ncomp) const
{
  const int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl = 0; num_cl < nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          int num_elem, signe;
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            {
              int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              // TODO : FIXME : a virer ce truc ...
              (ncomp == -1) ? corriger_secmem_fa7_elem_periodicite_<false>(ncomp,num_elem,fac1,fac2,face,signe,resu) :
              corriger_secmem_fa7_elem_periodicite_<true>(ncomp,num_elem,fac1,fac2,face,signe,resu);
            }
        }
    }
}

template <class _TYPE_> template <bool is_VECT> enable_if_t<is_VECT == true, void>
T_It_VDF_Face<_TYPE_>::corriger_secmem_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, DoubleTab& resu) const
{
  DoubleVect flux(ncomp);
  flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::ELEM>(num_elem,fac1,fac2,flux);
  for (int k=0; k<ncomp; k++) resu(face,k) += signe*flux(k);
}

template <class _TYPE_> template <bool is_VECT> enable_if_t<is_VECT == false, void>
T_It_VDF_Face<_TYPE_>::corriger_secmem_fa7_elem_periodicite_(const int ncomp, const int num_elem, const int fac1, const int fac2, const int face, const int signe, DoubleTab& resu) const
{
  assert (ncomp == -1);
  double flux = flux_evaluateur.template secmem_fa7<Type_Flux_Fa7::ELEM>(num_elem,fac1,fac2);
  resu(face) += signe*flux;
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::corriger_coeffs_fa7_elem_periodicite(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          int num_elem, signe, ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          double aii=0, ajj=0;
          for (int face = ndeb; face < nfin; face++)
            {
              const int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::ELEM>(num_elem, fac1, fac2, aii, ajj);
              {
                if (signe>0) /* on a oublie a droite  la contribution de la gche */
                  {
                    matrice(face,face)+=aii ;
                    matrice(face,fac2)-=ajj;
                  }
                else /* on a oublie a gauche  la contribution de la droite */
                  {
                    matrice(face,fac1)-=aii; ;
                    matrice(face,face)+=ajj;
                  }
              }
            }
        }
    }
}

template <class _TYPE_>
void T_It_VDF_Face<_TYPE_>::corriger_coeffs_fa7_elem_periodicite(const DoubleTab& inco, Matrice_Morse& matrice, int ncomp) const
{
  int nb_front_Cl = la_zone->nb_front_Cl();
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl_perio.frontiere_dis());
          IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
          DoubleVect& coeff = matrice.get_set_coeff();
          DoubleVect aii(ncomp), ajj(ncomp);
          int num_elem, signe, ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            {
              const int elem1 = elem(face,0), elem2 = elem(face,1), ori = orientation(face);
              if ( (face == elem_faces(elem1,ori)) || (face == elem_faces(elem1,ori+dimension)) )
                {
                  num_elem = elem2;
                  signe = 1;
                }
              else
                {
                  num_elem = elem1;
                  signe = -1;
                }
              const int fac1 = elem_faces(num_elem,ori), fac2 = elem_faces(num_elem,ori+dimension);
              flux_evaluateur.template coeffs_fa7<Type_Flux_Fa7::ELEM>(num_elem, fac1, fac2, aii, ajj);
              for (int i = 0; i < ncomp; i++ ) for (int k = tab1[face*ncomp+i]-1; k < tab1[face*ncomp+1+i]-1; k++) if (tab2[k]-1==face*ncomp+i) coeff[k]+=signe*aii(i);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords_(const int n_arete, const int n, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  constexpr bool is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      double flux = flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe);
      resu(fac3) += signe*flux;
      if (is_PAROI)
        {
          if (fac1<n) tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux;
          if (fac2<n) tab_flux_bords(fac2,orientation(fac3)) -= 0.5*signe*flux;
        }
    }
}

template <class _TYPE_>  template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords_(const int n_arete, const int n, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE), is_FLUIDE = (Arete_Type == Type_Flux_Arete::FLUIDE), is_PAROI_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
  double flux3 = 0, flux1_2 = 0, flux3_4 = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      if (is_PERIO)
        {
          int fac4 = signe;
          flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, fac4, flux3_4, flux1_2);
          resu(fac3) += 0.5*flux3_4;
          resu(fac4) -= 0.5*flux3_4;
        }
      else
        {
          flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe, flux3, flux1_2);
          resu(fac3) += signe*flux3;
        }
      resu(fac1) += flux1_2;
      resu(fac2) -= flux1_2;
      if (is_FLUIDE || is_PAROI_FL)
        {
          if (fac1 < n) tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux3;
          if (fac2 < n) tab_flux_bords(fac2,orientation(fac3)) -= 0.5*signe*flux3;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords_(const int n_arete, const int n, const int ncomp, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  constexpr bool is_PAROI = (Arete_Type == Type_Flux_Arete::PAROI);
  DoubleVect flux(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe, flux);
      for (int k=0; k < ncomp; k++)
        {
          resu(fac3,k) += signe*flux(k);
          if (is_PAROI)
            {
              if (fac1 < n) tab_flux_bords(fac1,k) -= 0.5*signe*flux(k);
              if (fac2 < n) tab_flux_bords(fac2,k) -= 0.5*signe*flux(k);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_bords_(const int n_arete, const int n, const int ncomp, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE), is_FLUIDE = (Arete_Type == Type_Flux_Arete::FLUIDE), is_PAROI_FL = (Arete_Type == Type_Flux_Arete::PAROI_FLUIDE);
  DoubleVect flux3(ncomp), flux1_2(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          DoubleVect flux3_4 = flux3;
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += 0.5*flux3_4(k);
              resu(fac4,k) -= 0.5*flux3_4(k);
            }
        }
      else for (int k=0; k<ncomp; k++) resu(fac3,k) += signe*flux3(k); // les autres Type_Flux_Arete ...

      for (int k=0; k<ncomp; k++) // pour tous les types !
        {
          resu(fac1,k) += flux1_2(k);
          resu(fac2,k) -= flux1_2(k);
          if (is_FLUIDE || is_PAROI_FL)
            {
              if (fac1 < n) tab_flux_bords(fac1,k) -= 0.5*signe*flux3(k);
              if (fac2 < n) tab_flux_bords(fac2,k) -= 0.5*signe*flux3(k);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, TypeAreteCoinVDF::type_arete Arete_Type_Coin> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins_(const int n_arete, const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  constexpr bool is_PERIO_PAROI = (Arete_Type_Coin == TypeAreteCoinVDF::PERIO_PAROI);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      double flux = flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe);
      resu(fac3) += signe*flux;
      if (is_PERIO_PAROI) /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */
        {
          tab_flux_bords(fac1,orientation(fac3)) -= 0.25*signe*flux;
          tab_flux_bords(fac2,orientation(fac3)) -= 0.25*signe*flux;
        }
      else tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux;
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins_(const int n_arete, const int n,const DoubleTab& inco,DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
  double flux3 = 0, flux1_2 = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          double flux3_4 = flux3;
          resu(fac3) += 0.5*flux3_4;
          resu(fac4) -= 0.5*flux3_4;
          resu(fac1) += 0.5*flux1_2;
          resu(fac2) -= 0.5*flux1_2;
        }
      else
        {
          resu(fac3) += signe*flux3;
          resu(fac1) += flux1_2;
          if (fac1 < n) tab_flux_bords(fac1,orientation(fac3)) -= 0.5*signe*flux3;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, bool is_VECT> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI && is_VECT == true, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins_(const int n_arete, const int ncomp, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  DoubleVect flux(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template flux_arete<Arete_Type>(inco, fac1, fac2, fac3, signe, flux);
      for (int k = 0; k < ncomp; k++)  /* on met 0.25 sur les deux faces (car on  ajoutera deux fois la contrib) */
        {
          resu(fac3,k)+=signe*flux(k);
          tab_flux_bords(fac1,k) -= 0.25*signe*flux(k);
          tab_flux_bords(fac2,k) -= 0.25*signe*flux(k);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type, bool is_VECT> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE && is_VECT == true, void>
T_It_VDF_Face<_TYPE_>::ajouter_aretes_coins_(const int n_arete, const int ncomp, const DoubleTab& inco, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  ajouter_aretes_bords_<should_calc_flux,Arete_Type>(n_arete, -1 /* unused */, ncomp,inco,resu,tab_flux_bords);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords_(const int n_arete, DoubleTab& resu) const
{
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      double flux = flux_evaluateur.template secmem_arete<Arete_Type>(fac1, fac2, fac3, signe);
      resu(fac3) += signe*flux;
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords_(const int n_arete, DoubleTab& resu) const
{
  constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
  double flux3 = 0, flux1_2 = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type>(fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          double flux3_4 = flux3;
          resu(fac3) += 0.5*flux3_4;
          resu(fac4) -= 0.5*flux3_4;
        }
      else resu(fac3) += signe*flux3;
      resu(fac1) += flux1_2;
      resu(fac2) -= flux1_2;
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  DoubleVect flux(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type>(fac1, fac2, fac3, signe, flux);
      for (int k = 0; k < ncomp; k++) resu(fac3,k) += signe*flux(k);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_bords_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
  DoubleVect flux3(ncomp), flux1_2(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type>(fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          DoubleVect flux3_4 = flux3;
          for (int k=0; k<ncomp; k++)
            {
              resu(fac3,k) += 0.5*flux3_4(k);
              resu(fac4,k) -= 0.5*flux3_4(k);
            }
        }
      else for (int k=0; k<ncomp; k++) resu(fac3,k) += signe*flux3(k);
      for (int k=0; k<ncomp; k++)
        {
          resu(fac1,k) += flux1_2(k);
          resu(fac2,k) -= flux1_2(k);
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins_(const int n_arete, DoubleTab& resu) const
{
  contribuer_au_second_membre_aretes_bords_<should_calc_flux,Arete_Type>(n_arete,resu);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins_(const int n_arete, DoubleTab& resu) const
{
  constexpr bool is_PERIO = (Arete_Type == Type_Flux_Arete::PERIODICITE);
  double flux3 = 0, flux1_2 = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template secmem_arete<Arete_Type>(fac1, fac2, fac3, signe, flux3, flux1_2);
      if (is_PERIO)
        {
          int fac4 = signe;
          double flux3_4 = flux3;
          resu(fac3) += 0.5*flux3_4;
          resu(fac4) -= 0.5*flux3_4;
          resu(fac1) += 0.5*flux1_2;
          resu(fac2) -= 0.5*flux1_2;
        }
      else
        {
          resu(fac3) += signe*flux3;
          resu(fac1) += flux1_2;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  contribuer_au_second_membre_aretes_bords_<should_calc_flux,Arete_Type>(n_arete,ncomp,resu);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
T_It_VDF_Face<_TYPE_>::contribuer_au_second_membre_aretes_coins_(const int n_arete, const int ncomp, DoubleTab& resu) const
{
  contribuer_au_second_membre_aretes_bords_<should_calc_flux,Arete_Type>(n_arete,ncomp,resu);
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, Matrice_Morse& matrice) const
{
  double aii1_2=0, aii3_4=0, ajj1_2=0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      matrice(fac3,fac3) += signe*aii3_4;
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, Matrice_Morse& matrice) const
{
  double aii1_2 = 0, aii3_4 = 0, ajj1_2 = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      matrice(fac3,fac3) += signe*aii3_4;
      matrice(fac1,fac1) += aii1_2;
      matrice(fac1,fac2) -= ajj1_2;
      matrice(fac2,fac1) -= aii1_2;
      matrice(fac2,fac2) += ajj1_2;
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t<Arete_Type == Type_Flux_Arete::PERIODICITE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int nb_face_reelle, Matrice_Morse& matrice) const
{
  double aii = 0, ajj = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac3, fac4, fac1, fac2, aii, ajj);
      if (fac1 < nb_face_reelle)
        {
          matrice(fac1,fac1) += aii;
          matrice(fac1,fac2) -= ajj;
        }
      if (fac2<nb_face_reelle)
        {
          matrice(fac2,fac1) -= aii;
          matrice(fac2,fac2) += ajj;
        }
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, fac4, aii, ajj);
      aii *= 0.5;
      ajj *= 0.5;
      if (fac3 < nb_face_reelle)
        {
          matrice(fac3,fac3) += aii;
          matrice(fac3,fac4) -= ajj;
        }
      if (fac4<nb_face_reelle)
        {
          matrice(fac4,fac3) -= aii;
          matrice(fac4,fac4) += ajj;
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int ncomp, const IntVect& tab1, const IntVect& tab2, DoubleVect& coeff) const
{
  DoubleVect aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      for (int i = 0; i < ncomp; i++)
        for (int k = tab1[fac3*ncomp+i]-1; k < tab1[fac3*ncomp+1+i]-1; k++)
          if (tab2[k]-1 == fac3*ncomp+i) coeff[k] += signe*aii3_4(i);
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int ncomp, const IntVect& tab1, const IntVect& tab2, DoubleVect& coeff) const
{
  constexpr bool is_FLUIDE = (Arete_Type == Type_Flux_Arete::FLUIDE), is_SYM_FLUIDE = (Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE);
  DoubleVect aii1_2(ncomp), aii3_4(ncomp), ajj1_2(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      for (int i = 0; i < ncomp; i++ )
        {
          for (int k = tab1[fac3*ncomp+i]-1; k < tab1[fac3*ncomp+1+i]-1; k++)
            if (tab2[k]-1 == fac3*ncomp+i) coeff[k] += signe*aii3_4(i);

          for (int k = tab1[fac1*ncomp+i]-1; k < tab1[fac1*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac1*ncomp+i)
                {
                  if (is_FLUIDE || is_SYM_FLUIDE) /* correction de coef[k,i] on fait planter :-) */
                    {
                      assert(0);
                      Process::exit();
                    }
                  coeff[k] += aii1_2(i);
                }
              if (tab2[k]-1 == fac2*ncomp+i) coeff[k] -= ajj1_2(i);
            }
          for (int k = tab1[fac2*ncomp+i]-1; k < tab1[fac2*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac1*ncomp+i) coeff[k] -= aii1_2(i);
              if (tab2[k]-1 == fac2*ncomp+i) coeff[k] += ajj1_2(i);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_bords_(const int n_arete, const int ncomp, const IntVect& tab1, const IntVect& tab2, DoubleVect& coeff) const
{
  DoubleVect aii(ncomp), ajj(ncomp);
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac3, fac4, fac1, fac2, aii, ajj);
      for (int i = 0; i < ncomp; i++ )
        {
          for (int k = tab1[fac1*ncomp+i]-1; k < tab1[fac1*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac1*ncomp+i) coeff[k] += aii(i);
              if (tab2[k]-1 == fac2*ncomp+i) coeff[k] -= ajj(i);
            }
          for (int k = tab1[fac2*ncomp+i]-1; k < tab1[fac2*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac1*ncomp+i) coeff[k] -= aii(i);
              if (tab2[k]-1 == fac2*ncomp+i) coeff[k] += ajj(i);
            }
        }
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, fac4, aii, ajj);
      for (int i = 0; i < ncomp; i++ )
        {
          for (int k = tab1[fac3*ncomp+i]-1; k < tab1[fac3*ncomp+i+1]-1; k++)
            {
              if (tab2[k]-1 == fac3*ncomp+i) coeff[k] += aii(i);
              if (tab2[k]-1 == fac4*ncomp+i) coeff[k] -= ajj(i);
            }
          for (int k = tab1[fac4*ncomp+i]-1; k < tab1[fac4*ncomp+1+i]-1; k++)
            {
              if (tab2[k]-1 == fac3*ncomp+i) coeff[k] -= aii(i);
              if (tab2[k]-1 == fac4*ncomp+i) coeff[k] += ajj(i);
            }
        }
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type> enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins_(const int n_arete, Matrice_Morse& matrice) const
{
  constexpr bool is_COIN_FL = (Arete_Type == Type_Flux_Arete::COIN_FLUIDE);
  double aii1_2 = 0, aii3_4 = 0, ajj1_2 = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, signe, aii1_2, aii3_4, ajj1_2);
      matrice(fac3,fac3) += signe*aii3_4;
      if(is_COIN_FL) matrice(fac1,fac1) += aii1_2;
    }
}

template <class _TYPE_> template <bool should_calc_flux, Type_Flux_Arete Arete_Type>
enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE, void>
T_It_VDF_Face<_TYPE_>::ajouter_contribution_aretes_coins_(const int n_arete, Matrice_Morse& matrice) const
{
  double aii = 0, ajj = 0;
  if (should_calc_flux)
    {
      int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3);
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac3, fac4, fac1, fac2, aii, ajj);
      matrice(fac1,fac1) += aii;
      matrice(fac1,fac2) -= ajj;
      matrice(fac2,fac1) -= aii;
      matrice(fac2,fac2) += ajj;
      flux_evaluateur.template coeffs_arete<Arete_Type>(fac1, fac2, fac3, fac4, aii, ajj);
      matrice(fac3,fac3) += aii;
      matrice(fac3,fac4) -= ajj;
      matrice(fac4,fac3) -= aii;
      matrice(fac4,fac4) += ajj;
    }
}

#endif /* T_It_VDF_Face_included */

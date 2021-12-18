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
// File:        T_It_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/43
//
//////////////////////////////////////////////////////////////////////////////

#ifndef T_It_VDF_Elem_included
#define T_It_VDF_Elem_included

#include <Operateur_Diff_base.h>
#include <Schema_Temps_base.h>
#include <Op_Conv_VDF_base.h>
#include <Champ_Uniforme.h>
#include <Operateur_base.h>
#include <communications.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Milieu_base.h>
#include <DoubleTrav.h>
#include <Debog.h>

template <class _TYPE_>
class T_It_VDF_Elem : public Iterateur_VDF_base
{
  inline unsigned taille_memoire() const { throw; }
  inline int duplique() const
  {
    T_It_VDF_Elem* xxx = new  T_It_VDF_Elem(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  };

public:
  inline T_It_VDF_Elem() { }
  inline T_It_VDF_Elem(const T_It_VDF_Elem<_TYPE_>& iter) :
    Iterateur_VDF_base(iter), flux_evaluateur(iter.flux_evaluateur) { elem.ref(iter.elem); }

  int impr(Sortie& os) const;
  void modifier_flux() const;
  void calculer_flux_bord(const DoubleTab&) const;
  void contribuer_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;

  inline Evaluateur_VDF& evaluateur() { return (Evaluateur_VDF&) flux_evaluateur; }
  inline const Evaluateur_VDF& evaluateur() const { return (Evaluateur_VDF&) flux_evaluateur; }
  inline void completer_() { elem.ref(la_zone->face_voisins()); }

protected:
  _TYPE_ flux_evaluateur;
  IntTab elem;
  mutable SFichier Flux, Flux_moment, Flux_sum;

  void contribuer_au_second_membre_bords(DoubleTab& ) const;
  void contribuer_au_second_membre_interne(DoubleTab& ) const;
  void ajouter_contribution_bords(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_interne(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_interne_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_bords_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>&) const;
  DoubleTab& ajouter_bords(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter_interne(const DoubleTab& , DoubleTab& ) const;
  inline const Milieu_base& milieu() const { return (la_zcl->equation()).milieu(); }

private:
  inline void fill_flux_tables_(int, int , const DoubleVect& , DoubleTab& , DoubleTab& ) const;

  /* Private methods called from ajouter_bords */
  template <bool should_calc_flux, typename BC>
  void ajouter_bords_(const BC& , int ,int ,const DoubleTab& ,DoubleTab& , DoubleTab& ) const;

  void ajouter_bords_(const Periodique& , int ,int ,const DoubleTab&,const Front_VF& ,DoubleTab& , DoubleTab& ) const;
  void ajouter_bords_(const Echange_externe_impose& , int ,int ,int, const DoubleTab&,const Front_VF& ,DoubleTab& , DoubleTab& ) const;

  /* Private methods called from calculer_flux_bord */
  template <bool should_calc_flux, typename BC>
  void calculer_flux_bord_(const BC& , int ,int ,const DoubleTab& ,DoubleTab& ) const;

  void calculer_flux_bord_(const Periodique& , int ,int ,const DoubleTab& ,const Front_VF& , DoubleTab& ) const;
  void calculer_flux_bord_(const Echange_externe_impose& , int ,int ,int, const DoubleTab&,const Front_VF& ,DoubleTab& ) const;

  /* Private methods called from contribuer_au_second_membre_bords */
  template <bool should_calc_flux, typename BC>
  void contribuer_au_second_membre_bords_(const BC& , int ,int ,DoubleTab& , DoubleTab& ) const;

  void contribuer_au_second_membre_bords_(const Periodique& , int ,int ,DoubleTab& , DoubleTab& ) const;
  void contribuer_au_second_membre_bords_(const Echange_externe_impose& , int ,int , int,const Front_VF& ,DoubleTab& , DoubleTab& ) const;

  /* Private methods called from ajouter_contribution_bords */
  template <bool should_calc_flux, typename BC>
  void ajouter_contribution_bords_(const BC& , int ,int ,const DoubleTab& , Matrice_Morse& ) const;

  void ajouter_contribution_bords_(const Periodique& , int ,int ,const DoubleTab& , Matrice_Morse& ) const;
  void ajouter_contribution_bords_(const Echange_externe_impose& , int ,int ,int,const DoubleTab& ,const Front_VF&, Matrice_Morse& ) const;

  /* Private methods called from ajouter_contribution_bords */
  template <bool should_calc_flux, typename BC>
  void ajouter_contribution_bords_vitesse_(const BC& , int ,int ,const DoubleTab& , Matrice_Morse& ) const;

  void ajouter_contribution_bords_vitesse_(const Periodique& , int ,int ,const DoubleTab& ,const Front_VF&, Matrice_Morse& ) const;
  void ajouter_contribution_bords_vitesse_(const Echange_externe_impose& , int ,int ,int ,const DoubleTab& ,const Front_VF&, Matrice_Morse& ) const;
};

#include <T_It_VDF_Elem.tpp> // templates specializations ici ;)

#endif /* T_It_VDF_Elem_included */

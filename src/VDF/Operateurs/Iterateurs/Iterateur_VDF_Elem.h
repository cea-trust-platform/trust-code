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

#ifndef Iterateur_VDF_Elem_included
#define Iterateur_VDF_Elem_included

#include <Schema_Temps_base.h>
#include <Op_Conv_VDF_base.h>
#include <EcrFicPartage.h>

template <class _TYPE_>
class Iterateur_VDF_Elem : public Iterateur_VDF_base
{
  inline unsigned taille_memoire() const override { throw; }
  inline int duplique() const override
  {
    Iterateur_VDF_Elem* xxx = new Iterateur_VDF_Elem(*this);
    if(!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

public:
  Iterateur_VDF_Elem() { }
  Iterateur_VDF_Elem(const Iterateur_VDF_Elem<_TYPE_>& iter) : Iterateur_VDF_base(iter), flux_evaluateur(iter.flux_evaluateur) { elem.ref(iter.elem); }

  inline Evaluateur_VDF& evaluateur() override { return static_cast<Evaluateur_VDF&> (flux_evaluateur); }
  inline const Evaluateur_VDF& evaluateur() const override { return static_cast<const Evaluateur_VDF&> (flux_evaluateur); }

  int impr(Sortie& os) const override;
  void completer_() override { elem.ref(le_dom->face_voisins()); }
  void ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>&) const override;
  void contribuer_au_second_membre(DoubleTab& ) const override;

  // INTERFACE  BLOCS
  void ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const override;

protected:
  _TYPE_ flux_evaluateur;
  IntTab elem;
  mutable SFichier Flux, Flux_moment, Flux_sum;
  inline const Milieu_base& milieu() const { return (la_zcl->equation()).milieu(); }

private:
  template <typename Type_Double> void ajouter_blocs_bords(const int , matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const;
  template <typename Type_Double> void ajouter_blocs_interne(const int , matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const;

  template <bool should_calc_flux, typename Type_Double, typename BC> void ajouter_blocs_bords_(const BC& , const int , const int , const int , matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const;
  template <typename Type_Double> void ajouter_blocs_bords_(const Periodique& , const int , const int , const int , const Front_VF& , matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const;
  template <typename Type_Double> void ajouter_blocs_bords_(const Echange_externe_impose& , const int , const int , const int , const int , const Front_VF& , matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const;
  template <typename Type_Double> void ajouter_blocs_bords_(const Echange_externe_radiatif& , const int , const int , const int , const int , const Front_VF& , matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const;

  void modifier_flux() const;
  template <typename Type_Double> inline void fill_flux_tables_(const int, const int , const double , const Type_Double& , DoubleTab& ) const;

  using VectorDeriv = std::vector<std::tuple<const DoubleTab *, Matrice_Morse *, int>>;
  void fill_derivee_cc(matrices_t mats, const tabs_t& semi_impl, VectorDeriv& d_cc) const;
  template<typename Type_Double> void fill_coeffs_matrices(const int, Type_Double&, Type_Double&, Matrice_Morse*, VectorDeriv&) const;
  template<typename Type_Double> void fill_coeffs_matrices(const int, const double, Type_Double&, Type_Double&, Matrice_Morse*, VectorDeriv&) const;

  // method implementee dans FT (trio) pour TCL model. Dans TRUST, la methode return false ...
  template <typename Type_Double> bool ajouter_blocs_bords_echange_ext_FT_TCL(const Echange_externe_impose& , const int , const int , const int , const int , const Front_VF& , matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const;

  // A virer un jour .. voir avec le baltik Rayonnement
  template <typename Type_Double> void contribuer_au_second_membre_bords(const int , DoubleTab& ) const;
  template <typename Type_Double> void contribuer_au_second_membre_interne(const int , DoubleTab& ) const;
  template <bool should_calc_flux, typename Type_Double, typename BC> void contribuer_au_second_membre_bords_(const BC& , const int , const int , const int , DoubleTab& ) const;
  template <typename Type_Double> void contribuer_au_second_membre_bords_(const Echange_externe_impose& , const int , const int , const int, const int , const Front_VF& , DoubleTab& ) const;
};

#include <Iterateur_VDF_Elem.tpp> // templates specializations ici ;)

#endif /* Iterateur_VDF_Elem_included */

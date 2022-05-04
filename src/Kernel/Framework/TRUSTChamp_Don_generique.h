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
// File:        TRUSTChamp_Don_generique.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTChamp_Don_generique_included
#define TRUSTChamp_Don_generique_included

#include <Champ_Don_base.h>
#include <Champ_Uniforme.h>
#include <Ref_Domaine.h>
#include <Parser_Eval.h>
#include <Domaine.h>

enum class Champ_Don_Type { XYZ , TXYZ , LU };

template <Champ_Don_Type _TYPE_>
class TRUSTChamp_Don_generique : public Champ_Don_base, public Parser_Eval
{
public:

  Sortie& printOn(Sortie& os) const override;
  void mettre_a_jour(double) override;
  double valeur_a_compo(const DoubleVect& position, int ncomp) const override;

  Domaine& interprete_get_domaine(const Nom& nom)
  {
    mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
    return mon_domaine.valeur();
  }

  // ERREUR : cela n'a pas de sens de vouloir effectuer une affectation avec un Champ_Don_Fonc_xyz/txyz.
  Champ_base& affecter(const Champ_base& ch)
  {
    Champ_base::affecter_erreur();
    return *this;
  }

  Champ_base& affecter_(const Champ_base& ch) override
  {
    static constexpr bool IS_LU = (_TYPE_ == Champ_Don_Type::LU);
    if (IS_LU) return Champ_Don_base::affecter_(ch); // throw

    if (sub_type(Champ_Uniforme, ch)) valeurs() = ch.valeurs()(0, 0);
    else valeurs() = ch.valeurs();
    return *this;
  }

  DoubleVect& valeur_a(const DoubleVect& position, DoubleVect& vals) const override
  {
    return valeur_a_<_TYPE_>(position, vals);
  }

  DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& vals, int le_poly) const override
  {
    return valeur_a_elem_<_TYPE_>(position, vals, le_poly);
  }

  double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override
  {
    return valeur_a_elem_compo_<_TYPE_>(position, le_poly, ncomp);
  }

  DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& vals) const override
  {
    return valeur_aux_<_TYPE_>(positions, vals);
  }

  DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& vals, int ncomp) const override
  {
    return valeur_aux_compo_<_TYPE_>(positions, vals, ncomp);
  }

  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& vals) const override
  {
    return valeur_aux_elems_<_TYPE_>(positions, les_polys, vals);
  }

  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& vals, int ncomp) const override
  {
    return valeur_aux_elems_compo_<_TYPE_>(positions,les_polys,vals,ncomp);
  }

protected:
  REF(Domaine) mon_domaine;
  void mettre_a_jour_positions(DoubleTab& );

private:
  /*
   * SFINAE template functions : can not be implemented directly on overrided functions ==> methodes internes ;-)
   */
  template<Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
  valeur_a_(const DoubleVect& position, DoubleVect& valeurs) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
  valeur_a_(const DoubleVect& position, DoubleVect& valeurs) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
  valeur_a_elem_(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
  valeur_a_elem_(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const;

  template <Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::TXYZ, double>
  valeur_a_elem_compo_(const DoubleVect& position, int le_poly, int ncomp) const;

  template <Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::TXYZ, double>
  valeur_a_elem_compo_(const DoubleVect& position, int le_poly, int ncomp) const;

  template <Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::LU, DoubleTab&>
  valeur_aux_(const DoubleTab& positions, DoubleTab& valeurs) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::LU, DoubleTab&>
  valeur_aux_(const DoubleTab& positions, DoubleTab& valeurs) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
  valeur_aux_compo_(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
  valeur_aux_compo_(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::LU, DoubleTab&>
  valeur_aux_elems_(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::LU, DoubleTab&>
  valeur_aux_elems_(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
  valeur_aux_elems_compo_(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const;

  template<Champ_Don_Type T = _TYPE_> enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
  valeur_aux_elems_compo_(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const;
};

#include <TRUSTChamp_Don_generique.tpp>

#endif /* TRUSTChamp_Don_generique_included */

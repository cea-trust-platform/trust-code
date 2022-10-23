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

#ifndef Champ_Fonc_Face_VDF_included
#define Champ_Fonc_Face_VDF_included

#include <Champ_Face_VDF_implementation.h>
#include <Champ_Fonc_base.h>
#include <Zone_VDF.h>

/*! @brief classe Champ_Fonc_Face_VDF
 *
 * @sa Champ_Fonc_base Champ_Face_VDF
 */
class Champ_Fonc_Face_VDF : public Champ_Fonc_base, public Champ_Face_VDF_implementation
{
  Declare_instanciable(Champ_Fonc_Face_VDF);
public:
  int fixer_nb_valeurs_nodales(int) override;
  int imprime(Sortie& os, int ncomp) const override;
  void mettre_a_jour(double) override;
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;
  Champ_base& affecter_(const Champ_base&) override;
  DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& tab_valeurs, int ncomp) const override;

  // Methodes inlines
  inline const Zone_VDF& zone_vdf() const override { return ref_cast(Zone_VDF, la_zone_VF.valeur()); }

  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& val, int le_poly) const override
  {
    return Champ_Face_VDF_implementation::valeur_a_elem(position, val, le_poly);
  }

  inline double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override
  {
    return Champ_Face_VDF_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
  }

  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& tab_valeurs) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_elems(positions, les_polys, tab_valeurs);
  }

  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& tab_valeurs, int ncomp) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
  }

  inline DoubleTab& valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_sommets(dom, val);
  }

  inline DoubleVect& valeur_aux_sommets_compo(const Domaine& dom, DoubleVect& val, int comp) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_sommets_compo(dom, val, comp);
  }

  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override
  {
    return Champ_Face_VDF_implementation::remplir_coord_noeuds(positions);
  }

  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override
  {
    return Champ_Face_VDF_implementation::remplir_coord_noeuds_et_polys(positions, polys);
  }

private :
  inline const Champ_base& le_champ() const override { return *this; }
  inline Champ_base& le_champ() override { return *this; }
};

#endif /* Champ_Fonc_Face_VDF_included */

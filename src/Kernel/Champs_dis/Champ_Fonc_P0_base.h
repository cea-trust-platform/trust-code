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

#ifndef Champ_Fonc_P0_base_included
#define Champ_Fonc_P0_base_included

#include <Champ_implementation_P0.h>
#include <Champ_Fonc_base.h>

class Champ_Fonc_P0_base: public Champ_Fonc_base, public Champ_implementation_P0
{
  Declare_base(Champ_Fonc_P0_base);
public:
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;
  Champ_base& affecter_(const Champ_base&) override;
  int fixer_nb_valeurs_nodales(int n) override;
  double moyenne(const DoubleVect& porosite_elem, int) const;
  DoubleVect moyenne(const DoubleVect& porosite_elem) const;
  int imprime(Sortie&, int) const override;
  void mettre_a_jour(double) override;
  double valeur_au_bord(int face) const;

  // Methodes inlines
  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const override
  {
    return Champ_implementation_P0::valeur_a_elem(position, result, poly);
  }

  inline double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const override
  {
    return Champ_implementation_P0::valeur_a_elem_compo(position, poly, ncomp);
  }

  inline DoubleTab& valeur_aux_centres_de_gravite(const Domaine& dom, DoubleTab& tab_valeurs) const override
  {
    return Champ_implementation_P0::valeur_aux_centres_de_gravite(dom, tab_valeurs);
  }

  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override
  {
    return Champ_implementation_P0::valeur_aux_elems(positions, polys, result);
  }

  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const override
  {
    return Champ_implementation_P0::valeur_aux_elems_compo(positions, polys, result, ncomp);
  }

  inline DoubleTab& valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const override
  {
    return Champ_implementation_P0::valeur_aux_sommets(domain, result);
  }

  inline DoubleVect& valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const override
  {
    return Champ_implementation_P0::valeur_aux_sommets_compo(domain, result, ncomp);
  }

  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override
  {
    return Champ_implementation_P0::remplir_coord_noeuds(positions);
  }

  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override
  {
    return Champ_implementation_P0::remplir_coord_noeuds_et_polys(positions, polys);
  }

protected:
  Champ_base& le_champ() override { return *this; }
  const Champ_base& le_champ() const override { return *this; }
};

#endif /* Champ_Fonc_P0_base_inclus*/

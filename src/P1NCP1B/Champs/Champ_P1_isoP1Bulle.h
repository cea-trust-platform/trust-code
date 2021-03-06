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

#ifndef Champ_P1_isoP1Bulle_included
#define Champ_P1_isoP1Bulle_included

#include <Champ_Inc_base.h>
#include <Champ_P1iP1B_implementation.h>
#include <Zone_VEF_PreP1b.h>
#include <Ref_Zone_VEF_PreP1b.h>


class Champ_P1_isoP1Bulle : public Champ_Inc_base,
  public Champ_P1iP1B_implementation
{

  Declare_instanciable(Champ_P1_isoP1Bulle);

public :
  int fixer_nb_valeurs_nodales(int) override;
  const Zone_dis_base& zone_dis_base() const override;
  void associer_zone_dis_base(const Zone_dis_base&) override;
  DoubleTab& remplir_coord_noeuds(DoubleTab& ) const override;
  DoubleVect& valeur_a_elem(const DoubleVect& position,
                            DoubleVect& val,
                            int le_poly) const override;

  double valeur_a_elem_compo(const DoubleVect& position,
                             int le_poly, int ncomp) const override;
  DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleTab& valeurs) const override;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleVect& valeurs
                                     ,int ncomp) const override ;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                              DoubleVect&, int) const override;

  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double ,int distant) const override;
  inline const Zone_VEF_PreP1b& zone_vef() const override;
  double valeur_au_bord(int face) const;
  Champ_base& affecter_(const Champ_base& ) override ;
  double norme_L2(const Domaine& dom) const;

protected :

  REF(Zone_VEF_PreP1b) la_Zone_VEF_PreP1b;

  void completer(const Zone_Cl_dis_base& zcl) override;

private :
  inline const Champ_base& le_champ() const override;
  inline Champ_base& le_champ() override;
};

inline const Zone_VEF_PreP1b& Champ_P1_isoP1Bulle::zone_vef() const
{
  return la_Zone_VEF_PreP1b.valeur();
}

inline const Champ_base& Champ_P1_isoP1Bulle::le_champ() const
{
  return *this;
}

inline Champ_base& Champ_P1_isoP1Bulle::le_champ()
{
  return *this;
}

inline DoubleTab& Champ_P1_isoP1Bulle::valeur_aux_sommets(const Domaine& dom, DoubleTab& sommets) const
{
  return Champ_P1iP1B_implementation::valeur_aux_sommets(dom, sommets);
}

inline DoubleVect& Champ_P1_isoP1Bulle::valeur_aux_sommets_compo(const Domaine& dom,
                                                                 DoubleVect& sommets, int compo) const
{
  return Champ_P1iP1B_implementation::valeur_aux_sommets_compo(dom, sommets, compo);
}

#endif

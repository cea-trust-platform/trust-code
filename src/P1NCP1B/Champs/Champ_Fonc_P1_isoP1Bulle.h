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


#ifndef Champ_Fonc_P1_isoP1Bulle_included
#define Champ_Fonc_P1_isoP1Bulle_included

#include <Champ_Fonc_base.h>
#include <Champ_P1iP1B_implementation.h>
#include <Ref_Zone_VEF_PreP1b.h>

//.DESCRIPTION classe Champ_Fonc_P1_isoP1Bulle
//
//.SECTION voir aussi
// Champ_Fonc_base Champ P1NC

class Champ_Fonc_P1_isoP1Bulle: public Champ_Fonc_base, public Champ_P1iP1B_implementation
{
  Declare_instanciable(Champ_Fonc_P1_isoP1Bulle);

public :
  int fixer_nb_valeurs_nodales(int) override;
  const Zone_dis_base& zone_dis_base() const override;
  void associer_zone_dis_base(const Zone_dis_base&) override;
  inline const Zone_VEF_PreP1b& zone_vef() const override;
  void mettre_a_jour(double ) override;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double,int distant ) const override;

  inline DoubleVect& valeur_a_elem(const DoubleVect& position,
                                   DoubleVect& val,
                                   int le_poly) const override;
  inline double valeur_a_elem_compo(const DoubleVect& position,
                                    int le_poly, int ncomp) const override;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleTab& tab_valeurs) const override;
  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                            const IntVect& les_polys,
                                            DoubleVect& tab_valeurs,
                                            int ncomp) const override ;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                              DoubleVect&, int) const override;
  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;
  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                           IntVect& polys) const override;
protected:

  REF(Zone_VEF_PreP1b) la_zone_VEFP1B;

  void completer(const Zone_Cl_dis_base& zcl) override;

  inline const Champ_base& le_champ() const override;
  inline Champ_base& le_champ() override;
};

inline const Champ_base& Champ_Fonc_P1_isoP1Bulle::le_champ() const
{
  return *this;
}

inline Champ_base& Champ_Fonc_P1_isoP1Bulle::le_champ()
{
  return *this;
}

inline DoubleVect& Champ_Fonc_P1_isoP1Bulle::valeur_a_elem(const DoubleVect& position,
                                                           DoubleVect& val,
                                                           int le_poly) const
{
  return Champ_P1iP1B_implementation::valeur_a_elem(position, val, le_poly);
}
inline double  Champ_Fonc_P1_isoP1Bulle::valeur_a_elem_compo(const DoubleVect& position,
                                                             int le_poly, int ncomp) const
{
  return Champ_P1iP1B_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
}
inline DoubleTab&  Champ_Fonc_P1_isoP1Bulle::valeur_aux_elems(const DoubleTab& positions,
                                                              const IntVect& les_polys,
                                                              DoubleTab& tab_valeurs) const
{
  return Champ_P1iP1B_implementation::valeur_aux_elems(positions, les_polys, tab_valeurs);
}
inline DoubleVect&  Champ_Fonc_P1_isoP1Bulle::valeur_aux_elems_compo(const DoubleTab& positions,
                                                                     const IntVect& les_polys,
                                                                     DoubleVect& tab_valeurs,
                                                                     int ncomp) const
{
  return Champ_P1iP1B_implementation::valeur_aux_elems_compo(positions, les_polys,
                                                             tab_valeurs, ncomp);
}
inline DoubleTab&  Champ_Fonc_P1_isoP1Bulle::valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const
{
  return Champ_P1iP1B_implementation::valeur_aux_sommets(dom, val);
}
inline DoubleVect&  Champ_Fonc_P1_isoP1Bulle::valeur_aux_sommets_compo(const Domaine& dom,
                                                                       DoubleVect& val, int comp) const
{
  return Champ_P1iP1B_implementation::valeur_aux_sommets_compo(dom, val, comp);
}
inline DoubleTab&  Champ_Fonc_P1_isoP1Bulle::remplir_coord_noeuds(DoubleTab& positions) const
{
  return Champ_P1iP1B_implementation::remplir_coord_noeuds(positions);
}
inline int  Champ_Fonc_P1_isoP1Bulle::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                                    IntVect& polys) const
{
  return Champ_P1iP1B_implementation::remplir_coord_noeuds_et_polys(positions,
                                                                    polys);
}

// Description :
// Renvoie la Zone_VEF
inline const Zone_VEF_PreP1b& Champ_Fonc_P1_isoP1Bulle::zone_vef() const
{
  return la_zone_VEFP1B.valeur();
}

#endif

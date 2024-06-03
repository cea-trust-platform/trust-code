/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Champ_Inc_P0_base_included
#define Champ_Inc_P0_base_included

#include <Champ_implementation_P0.h>
#include <Champ_Inc_base.h>
#include <TRUST_Ref.h>
#include <Domaine_VF.h>

/*! @brief : class Champ_Inc_P0_base
 *
 *  Decrire ici la classe Champ_Inc_P0_base
 *
 */
class Champ_Inc_P0_base : public Champ_Inc_base, public Champ_implementation_P0
{
  Declare_base(Champ_Inc_P0_base);
public :
  double valeur_au_bord(int face) const;

  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double,int distant ) const override;
  Champ_base& affecter_(const Champ_base& ) override ;
  int fixer_nb_valeurs_nodales(int n) override;

  //tableaux utilitaires sur les CLs : fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)
  //types de CL : 0 -> pas de CL
  //              1 -> Echange_externe_impose
  //              2 -> Echange_global_impose
  //              3 -> Echange_contact_PolyMAC_P0P1NC
  //              4 -> Neumann_paroi
  //              5 -> Neumann_val_ext ou Neumann_homogene ou Symetrie
  //              6 -> Dirichlet
  //              7 -> Dirichlet_homogene
  inline const IntTab& fcl() const
  {
    if (!fcl_init_) init_fcl();
    return fcl_;
  }

  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const override
  {
    return Champ_implementation_P0::valeur_a_elem(position,result,poly);
  }

  inline double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const override
  {
    return Champ_implementation_P0::valeur_a_elem_compo(position,poly,ncomp);
  }

  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override
  {
    return Champ_implementation_P0::valeur_aux_elems(positions,polys,result);
  }

  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const override
  {
    return Champ_implementation_P0::valeur_aux_elems_compo(positions,polys,result,ncomp);
  }

  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override
  {
    return Champ_implementation_P0::remplir_coord_noeuds(positions);
  }

  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override
  {
    return Champ_implementation_P0::remplir_coord_noeuds_et_polys(positions,polys);
  }

  inline DoubleTab& valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const override
  {
    return Champ_implementation::valeur_aux_sommets(domain,result);
  }

  inline DoubleVect& valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const override
  {
    return Champ_implementation::valeur_aux_sommets_compo(domain,result,ncomp);
  }

protected:
  Champ_base& le_champ() override { return *this; }
  const Champ_base& le_champ() const override { return *this; }

  virtual void init_fcl() const;
  mutable IntTab fcl_;
  mutable int fcl_init_ = 0; // fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)
};

#endif /* Champ_Inc_P0_base_included */

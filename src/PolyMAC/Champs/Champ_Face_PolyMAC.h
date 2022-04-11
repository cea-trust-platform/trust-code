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
// File:        Champ_Face_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Face_PolyMAC_included
#define Champ_Face_PolyMAC_included

#include <Champ_Inc_base.h>
#include <Zone_VF.h>
#include <Ref_Zone_VF.h>
#include <SolveurSys.h>
#include <Zone_PolyMAC.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Champ_Face_PolyMAC
// .DESCRIPTION : class Champ_Face_PolyMAC
//
// Champ correspondant a une inconnue decrite par ses flux aux faces (type vitesse)
// Degres de libertes : composante normale aux faces + composante tangentielle aux aretes de la vorticite
/////////////////////////////////////////////////////////////////////////////

class Champ_Face_PolyMAC : public Champ_Inc_base
{

  Declare_instanciable(Champ_Face_PolyMAC) ;

protected :
  virtual       Champ_base& le_champ(void);
  virtual const Champ_base& le_champ(void) const;

public :

  void    associer_zone_dis_base(const Zone_dis_base&) override;
  const Zone_dis_base& zone_dis_base() const override
  {
    return ref_zone_vf_.valeur();
  } ;

  DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const override;
  double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const override;
  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const override;

  DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;
  int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override;

  DoubleTab& valeur_aux_faces(DoubleTab& result) const override;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double, int distant ) const override;

  Champ_base& affecter_(const Champ_base& ) override;
  int nb_valeurs_nodales() const override;

  const Zone_VF& zone_vf() const
  {
    return ref_zone_vf_.valeur();
  };

  int fixer_nb_valeurs_nodales(int n) override;

  virtual void init_auxiliary_variables(); /* demande l'ajout des variables auxiliaires ( [lambda rot u] aux aretes )*/
  int reprendre(Entree& fich) override;

  //tableaux de correspondance lies aux CLs : fcl(f, .) = { type de CL, num de la CL, indice de la face dans la CL }
  //types de CL : 0 -> pas de CL
  //              1 -> Neumann ou Neumann_homogene
  //              2 -> Navier
  //              3 -> Dirichlet
  //              4 -> Dirichlet_homogene
  inline const IntTab& fcl() const
  {
    if (!fcl_init_) init_fcl();
    return fcl_;
  }

  //interpolations aux elements : vitesse val(e, i) = v_i, gradient vals(e, i, j) = dv_i / dx_j
  void interp_ve (const DoubleTab& inco, DoubleTab& val, bool is_vit=true) const;
  void interp_ve (const DoubleTab& inco, const IntVect&, DoubleTab& val, bool is_vit=true) const;

protected:
  REF(Zone_VF) ref_zone_vf_;

  void init_fcl() const;
  mutable IntTab fcl_;
  mutable int fcl_init_ = 0;
};


#endif /* Champ_Face_PolyMAC_included */

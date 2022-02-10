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
// File:        Champ_Fonc_Face_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_Face_CoviMAC_included
#define Champ_Fonc_Face_CoviMAC_included

#include <Champ_Fonc_base.h>
#include <Zone_VF.h>
#include <Ref_Zone_VF.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Champ_Fonc_Face_CoviMAC
// .HEADER      : Trio_U_Kernel_Fixes Trio_U_Kernel_Fixes/src/Champs_dis
// .LIBRARY     : libTrio_U_Kernel_Fixes
// .DESCRIPTION : class Champ_Fonc_Face_CoviMAC
//
// <Description of class Champ_Fonc_Face_CoviMAC>
//
/////////////////////////////////////////////////////////////////////////////

class Champ_Fonc_Face_CoviMAC : public Champ_Fonc_base
{

  Declare_instanciable(Champ_Fonc_Face_CoviMAC) ;
public:
  void mettre_a_jour(double) override;

protected :
  virtual       Champ_base& le_champ(void)      ;
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

  DoubleTab& valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const override;
  DoubleVect& valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const override;

  Champ_base& affecter_(const Champ_base& ) override;

  const Zone_VF& zone_vf() const
  {
    return ref_zone_vf_.valeur();
  };


public :
  int fixer_nb_valeurs_nodales(int n) override;
protected:
  REF(Zone_VF) ref_zone_vf_;

};


#endif /* Champ_Fonc_Face_CoviMAC_included */

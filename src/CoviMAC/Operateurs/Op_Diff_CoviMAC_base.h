/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_CoviMAC_base.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Op_Diff_CoviMAC_base_included
#define Op_Diff_CoviMAC_base_included

#include <Operateur_Diff_base.h>
#include <Op_Diff_Turbulent_base.h>
#include <Ref_Zone_CoviMAC.h>
#include <Ref_Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
class Champ_Fonc;


//
// .DESCRIPTION class Op_Diff_CoviMAC_base
//
// Classe de base des operateurs de diffusion CoviMAC

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_CoviMAC_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_CoviMAC_base : public Operateur_Diff_base, public Op_Diff_Turbulent_base
{


  Declare_base(Op_Diff_CoviMAC_base);

public:
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& );

  void associer_diffusivite(const Champ_base& );
  void completer();
  const Champ_base& diffusivite() const;
  void mettre_a_jour(double t);

  void update_nu() const; //met a jour nu et nu_fac; les specialisations font les interpolations
  const DoubleTab& get_nu() const
  {
    return nu_;
  }

  const DoubleTab& get_nu_fac() const
  {
    return nu_fac_;
  }

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual int impr(Sortie& os) const;
  mutable DoubleTab nu_fac_mod; //facteur multiplicatif "utilisateur" a appliquer a nu_fac

protected:
  REF(Zone_CoviMAC) la_zone_poly_;
  REF(Zone_Cl_CoviMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;
  mutable DoubleTab nu_, nu_fac_; //conductivite aux elements, facteur multiplicatif a appliquer par face

  /* interpolations de nu.grad T de chaque cote de chaque face */
  mutable IntTab phif_d, phif_j; //indices : phif_j([phif_d(f), phif_d(f + 1)[)
  mutable DoubleTab phif_c;      //coeffs :  phif_c([phif_d(f), phif_d(f + 1)[, n) pour la composante n

  mutable int nu_a_jour_; //si on doit mettre a jour nu
  int nu_constant_;       //1 si nu est constant dans le temps
};

#endif

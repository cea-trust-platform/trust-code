/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Conv_EF_base.h
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Conv_EF_base_included
#define Op_Conv_EF_base_included

#include <Operateur_Conv.h>
#include <Ref_Champ_Inc_base.h>
#include <Ref_Zone_EF.h>
#include <Ref_Zone_Cl_EF.h>
#include <Zone_EF.h>
#include <Zone_Cl_EF.h>
#include <Les_Cl.h>
#include <Domaine.h>
#include <Navier_Stokes_std.h>
#include <Op_EF_base.h>

//
// .DESCRIPTION class Op_Conv_EF_base
//
// Classe de base des operateurs de convection EF

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Conv_EF_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Conv_EF_base : public Operateur_Conv_base, public Op_EF_base
{

  Declare_base(Op_Conv_EF_base);

public:

  void associer_vitesse(const Champ_base& );
  const Champ_Inc_base& vitesse() const;
  Champ_Inc_base& vitesse();
  virtual void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& );
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual void abortTimeStep();
  double calculer_dt_stab() const ;
  void calculer_pour_post(Champ& espace_stockage,const Nom& option, int) const;

  virtual void remplir_fluent(DoubleVect& ) const;
  virtual int impr(Sortie& os) const;
  void associer_zone_cl_dis(const Zone_Cl_dis_base&);
  int  phi_u_transportant(const Equation_base& eq) const;
  void completer();

protected:

  REF(Zone_EF) la_zone_EF;
  REF(Zone_Cl_EF) la_zcl_EF;
  REF(Champ_Inc_base) vitesse_;

  mutable DoubleVect fluent;           // tableau qui sert pour le calcul du pas
  //de temps de stabilite



};

#endif

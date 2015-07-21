/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Solveur_Masse_EF.h
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solveur_Masse_EF_included
#define Solveur_Masse_EF_included


#include <Solveur_Masse.h>
#include <Ref_Zone_EF.h>
#include <Ref_Zone_Cl_EF.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Solveur_Masse_EF
//
//////////////////////////////////////////////////////////////////////////////

class Solveur_Masse_EF : public Solveur_Masse_base
{

  Declare_instanciable(Solveur_Masse_EF);

public:

  void associer_zone_dis_base(const Zone_dis_base& );
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& );

  virtual Matrice_Base& ajouter_masse(double dt, Matrice_Base& matrice, int penalisation=1) const;
  virtual DoubleTab& ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation=1) const;

  DoubleTab& appliquer_impl(DoubleTab& ) const;

private:

  REF(Zone_EF) la_zone_EF;
  REF(Zone_Cl_EF) la_zone_Cl_EF;
};

#endif

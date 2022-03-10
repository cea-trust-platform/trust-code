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
// File:        Iterateur_Source_VEF_base.h
// Directory:   $TRUST_ROOT/src/VEF/Sources/Iterateurs
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Iterateur_Source_VEF_base_included
#define Iterateur_Source_VEF_base_included


//
// .DESCRIPTION class Iterateur_Source_VEF_base
//
// .SECTION voir aussi Objet_U
//

#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Equation_base.h>
#include <Ref_Source_base.h>

#include <TRUSTTab.h>
class Evaluateur_Source_VEF;

class Iterateur_Source_VEF_base : public Objet_U
{

  Declare_base(Iterateur_Source_VEF_base);

public:

  void associer_zones(const Zone_VEF&, const Zone_Cl_VEF&);
  inline void associer(const Source_base& source);
  virtual DoubleTab& ajouter(DoubleTab& ) const=0;
  virtual DoubleTab& calculer(DoubleTab& ) const=0;
  virtual Evaluateur_Source_VEF& evaluateur() =0;
  virtual void completer_()=0;
  virtual int initialiser(double tps)=0;
  virtual inline int equation_divisee_par_rho() const;

protected:

  REF(Zone_VEF) la_zone;
  REF(Zone_Cl_VEF) la_zcl;
  REF(Source_base) so_base;

};
int Iterateur_Source_VEF_base::equation_divisee_par_rho() const
{
  Nom nom_eqn=la_zcl->equation().que_suis_je();
  if (nom_eqn.debute_par("Navier_Stokes"))
    return 1;
  else
    return 0;
}

void Iterateur_Source_VEF_base::associer(const Source_base& source)
{
  so_base = source;
}

#endif

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
// File:        Terme_Source_Th_TdivU_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Terme_Source_Th_TdivU_VEF_Face_included
#define Terme_Source_Th_TdivU_VEF_Face_included

#include <Terme_Source_Qdm.h>

#include <Operateur_Conv.h>
class Probleme_base;
#include <Equation.h>
//
// .DESCRIPTION class Terme_Source_Th_TdivU_VEF_Face
//

//
// .SECTION voir aussi Source_base
//
//

class Terme_Source_Th_TdivU_VEF_Face : public Source_base, public Terme_Source_Qdm
{

  Declare_instanciable(Terme_Source_Th_TdivU_VEF_Face);

public:

  void associer_pb(const Probleme_base& );
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  void mettre_a_jour(double );
  void completer();
  void associer_eqn_t();
protected:
  void modifier_zone_cl();


  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );
  REF(Equation_base) eqn_t;
  Zone_Cl_dis ma_zonecl_;
  REF(Zone_Cl_dis_base) zonecl_sa;
  int zone_cl_mod_;
};

#endif

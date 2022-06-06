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


#ifndef Op_Grad_P1NC_to_P0_included
#define Op_Grad_P1NC_to_P0_included

#include <Operateur_Grad.h>
#include <Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Ref_Zone_VEF.h>

//
// .DESCRIPTION class Op_Grad_P1NC_to_P0
//
//  Cette classe represente un operateur de gradient
//  La discretisation est VEF
//  On calcule le gradient d'un champ_P1NC a une composante (temperature,concentration)
//   et le champ de sortie est P0 (aux elements)


//
// .SECTION voir aussi
// Operateur_Grad_base
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Grad_P1NC_to_P0
//
//////////////////////////////////////////////////////////////////////////////

class Op_Grad_P1NC_to_P0 : public Operateur_Grad_base
{

  Declare_instanciable(Op_Grad_P1NC_to_P0);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis&,const Champ_Inc&) override;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;

protected:

  REF(Zone_VEF) la_zone_vef;
  REF(Zone_Cl_VEF) la_zcl_vef;

};

#endif

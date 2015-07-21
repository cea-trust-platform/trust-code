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
// File:        Op_Dift_standard_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Dift_standard_VEF_Face_included
#define Op_Dift_standard_VEF_Face_included

#include <Ref_Champ_Uniforme.h>
#include <Operateur_Div.h>
#include <Op_Dift_VEF_Face.h>

//
// .DESCRIPTION class Op_Dift_standard_VEF_Face
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_standard_VEF_Face : public Op_Dift_VEF_Face
{
  Declare_instanciable(Op_Dift_standard_VEF_Face);

public:

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  void calcul_divergence(DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const Zone_Cl_VEF&,const Zone_VEF&,DoubleTab&,int) const;
  void ajouter_cas_vectoriel(const DoubleTab&, DoubleTab& , DoubleTab& ,const DoubleTab& , const DoubleTab& ,const Zone_Cl_VEF& ,const Zone_VEF& ,const DoubleTab& ,const DoubleTab& ,const int ,const int ,int ) const;


protected :

  REF(Champ_Inc) divergence_U;

  int grad_Ubar;
  int nu_lu;
  int nut_lu;
  int nu_transp_lu;
  int nut_transp_lu;
  int filtrer_resu;
};



#endif


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
// File:        Op_Diff_K_Eps_QC_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Op_Diff_K_Eps_QC_VDF_Elem_included
#define Op_Diff_K_Eps_QC_VDF_Elem_included

#include <Eval_Diff_K_Eps_QC_VDF_Elem.h>
#include <Op_Diff_K_Eps_VDF_base.h>

class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc;
class Champ_Fonc;
class Champ_base;
class DoubleTab;


declare_It_VDF_Elem(Eval_Diff_K_Eps_QC_VDF_Elem)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_K_Eps_QC_VDF_Elem
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps_QC_VDF_Elem : public Op_Diff_K_Eps_VDF_base
{

  Declare_instanciable_sans_constructeur(Op_Diff_K_Eps_QC_VDF_Elem);

public:

  Op_Diff_K_Eps_QC_VDF_Elem();
  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  // void associer_diffusivite_turbulente();
  //  void mettre_a_jour_diffusivite() const;


};




#endif

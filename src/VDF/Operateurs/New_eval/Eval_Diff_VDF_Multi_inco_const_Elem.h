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
// File:        Eval_Diff_VDF_Multi_inco_const_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Eval_Diff_VDF_Multi_inco_const_Elem_included
#define Eval_Diff_VDF_Multi_inco_const_Elem_included

#include <Eval_Diff_VDF_Multi_inco_const2.h>
#include <Eval_Diff_VDF_Elem.h>

//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_const_Elem
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue
// Le champ de diffusivite associe a chaque inconnue est constant.

//
// .SECTION voir aussi Eval_Diff_VDF_Multi_inco_const
class Eval_Diff_VDF_Multi_inco_const_Elem :
      public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_const_Elem>,
      public Eval_Diff_VDF_Multi_inco_const2
{
};

//// DEBUT DES DEFINES
//#define CLASSNAME Eval_Diff_VDF_Multi_inco_const_Elem
//
//#define nu_1(i,k) diffusivite(k)
//#define nu_2(i,k) diffusivite(k)
//#define f_heq(d0,i,d1,j,k) heq=nu_2(i,k)/(d0+d1);
//// FIN DES DEFINES
//#include <Cal_std.h>
//#include <Scal_corps_base_inut.h>
//#include <Vect_corps_base.h>


#endif

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
// File:        Op_Diff_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_VDF_Elem_included
#define Op_Diff_VDF_Elem_included

#include <Op_Diff_VDF_Elem_base.h>
#include <ItVDFEl.h>
#include <Eval_Diff_VDF_const_Elem.h>
#include <Op_VDF_Elem.h>

//
// .DESCRIPTION class Op_Diff_VDF_Elem
//
//  Cette classe represente l'operateur de diffusion associe a une equation de
//  transport.
//  La discretisation est VDF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite n'est pas uniforme
//  L'iterateur associe est de type Iterateur_VDF_Elem
//  L'evaluateur associe est de type Eval_Diff_VDF_var_Elem

//
// .SECTION voir aussi
//
//


// DO NOT EDIT  THIS FILE BUT  OpDifVDFElCs.h.h
//
declare_It_VDF_Elem(Eval_Diff_VDF_const_Elem)


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_VDF_Elem
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_VDF_Elem : public Op_Diff_VDF_Elem_base
{

  Declare_instanciable_sans_constructeur(Op_Diff_VDF_Elem);

public:

  Op_Diff_VDF_Elem();
  inline Op_Diff_VDF_Elem(const Iterateur_VDF_base&);
protected:
  inline Eval_VDF_Elem& get_eval_elem();
};

// Ce constructeur permet de creer des classes filles des evalateurs
// (utilise dans le constructeur de Op_Diff_VDF_Elem_temp_FTBM)
inline Op_Diff_VDF_Elem::Op_Diff_VDF_Elem(const Iterateur_VDF_base& iterateur)
  : Op_Diff_VDF_Elem_base(iterateur)
{
}
// Description renvoit l'evaluateur caste en Ecal_VDF_Elem corretement
inline Eval_VDF_Elem& Op_Diff_VDF_Elem::get_eval_elem()
{
  Eval_Diff_VDF_const_Elem& eval_diff = (Eval_Diff_VDF_const_Elem&) iter.evaluateur();
  return (Eval_VDF_Elem&) eval_diff;
}
#endif

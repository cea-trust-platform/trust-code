/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Op_Diff_VDF_var_Elem_Axi.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_op
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_VDF_var_Elem_Axi_included
#define Op_Diff_VDF_var_Elem_Axi_included

#include <Op_Diff_VDF_Elem_base2.h>
#include <ItVDFEl.h>
#include <Op_VDF_Elem.h>
#include <Eval_Diff_VDF_leaves.h>

//
// .DESCRIPTION class Op_Diff_VDF_var_Elem_Axi
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

declare_It_VDF_Elem(Eval_Diff_VDF_var_Elem_Axi)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_VDF_var_Elem_Axi
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_VDF_var_Elem_Axi : public Op_Diff_VDF_Elem_base2
{

  Declare_instanciable_sans_constructeur(Op_Diff_VDF_var_Elem_Axi);

public:

  Op_Diff_VDF_var_Elem_Axi();
  inline Op_Diff_VDF_var_Elem_Axi(const Iterateur_VDF_base&);
protected:
  inline Eval_VDF_Elem2& get_eval_elem();
};

// Ce constructeur permet de creer des classes filles des evalateurs
// (utilise dans le constructeur de Op_Diff_VDF_var_Elem_Axi_temp_FTBM)
inline Op_Diff_VDF_var_Elem_Axi::Op_Diff_VDF_var_Elem_Axi(const Iterateur_VDF_base& iterateur)
  : Op_Diff_VDF_Elem_base2(iterateur)
{
}
// Description renvoit l'evaluateur caste en Ecal_VDF_Elem corretement
inline Eval_VDF_Elem2& Op_Diff_VDF_var_Elem_Axi::get_eval_elem()
{
  Eval_Diff_VDF_var_Elem_Axi& eval_diff = dynamic_cast<Eval_Diff_VDF_var_Elem_Axi&> (iter.evaluateur());
  return eval_diff;
}

#endif /* Op_Diff_VDF_var_Elem_Axi_included */

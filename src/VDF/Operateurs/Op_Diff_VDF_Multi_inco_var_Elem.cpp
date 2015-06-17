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
// File:        Op_Diff_VDF_Multi_inco_var_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VDF_Multi_inco_var_Elem.h>

// DO NOT EDIT  THIS FILE BUT  OpDifVDFmiElVr.h.cpp

Implemente_instanciable_sans_constructeur(Op_Diff_VDF_Multi_inco_var_Elem,"Op_Diff_VDF_Multi_inco_var_P0_VDF",Op_Diff_VDF_Elem_base);

implemente_It_VDF_Elem(Eval_Diff_VDF_Multi_inco_var_Elem)


//// printOn
//

Sortie& Op_Diff_VDF_Multi_inco_var_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_Diff_VDF_Multi_inco_var_Elem::readOn(Entree& s )
{
  return s ;
}

//
// Fonctions inline de la classe Op_Diff_VDF_Multi_inco_var_Elem
//
// Description:
// constructeur
Op_Diff_VDF_Multi_inco_var_Elem::Op_Diff_VDF_Multi_inco_var_Elem() :
  Op_Diff_VDF_Elem_base(It_VDF_Elem(Eval_Diff_VDF_Multi_inco_var_Elem)())
{
}

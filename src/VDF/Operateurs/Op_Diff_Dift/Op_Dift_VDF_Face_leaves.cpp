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
// File:        Op_Dift_VDF_Face_leaves.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Face_leaves.h>

//////////////// CONST /////////////////

Implemente_instanciable_sans_constructeur(Op_Dift_VDF_Face,"Op_Dift_VDF_Face",Op_Dift_VDF_Face_base);
implemente_It_VDF_Face(Eval_Dift_VDF_const_Face)
Sortie& Op_Dift_VDF_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_VDF_Face::readOn(Entree& s ) { return s ; }
Op_Dift_VDF_Face::Op_Dift_VDF_Face() : Op_Dift_VDF_Face_base(It_VDF_Face(Eval_Dift_VDF_const_Face)()) { }

//////////////// VAR /////////////////

Implemente_instanciable_sans_constructeur(Op_Dift_VDF_var_Face,"Op_Dift_VDF_var_Face",Op_Dift_VDF_Face_base);
implemente_It_VDF_Face(Eval_Dift_VDF_var_Face)
Sortie& Op_Dift_VDF_var_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_VDF_var_Face::readOn(Entree& s ) { return s ; }
Op_Dift_VDF_var_Face::Op_Dift_VDF_var_Face() : Op_Dift_VDF_Face_base(It_VDF_Face(Eval_Dift_VDF_var_Face)())
{
  declare_support_masse_volumique(1);
}

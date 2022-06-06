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

#include <Op_Conv_VDF_Elem_leaves.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Amont_VDF_Elem,"Op_Conv_Amont_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Amont_VDF_Elem)
Sortie& Op_Conv_Amont_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Amont_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Amont_VDF_Elem::Op_Conv_Amont_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Amont_VDF_Elem)()) { }

Implemente_instanciable_sans_constructeur(Op_Conv_Centre_VDF_Elem,"Op_Conv_Centre_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Centre_VDF_Elem)
Sortie& Op_Conv_Centre_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Centre_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Centre_VDF_Elem::Op_Conv_Centre_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Centre_VDF_Elem)()) { }

Implemente_instanciable_sans_constructeur(Op_Conv_Centre4_VDF_Elem,"Op_Conv_Centre4_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Centre4_VDF_Elem)
Sortie& Op_Conv_Centre4_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Centre4_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Centre4_VDF_Elem::Op_Conv_Centre4_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Centre4_VDF_Elem)()) { }

Implemente_instanciable_sans_constructeur(Op_Conv_Quick_VDF_Elem,"Op_Conv_Quick_VDF_P0_VDF",Op_Conv_VDF_base);
implemente_It_VDF_Elem(Eval_Quick_VDF_Elem)
Sortie& Op_Conv_Quick_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_Quick_VDF_Elem::readOn(Entree& s ) { return s ; }
Op_Conv_Quick_VDF_Elem::Op_Conv_Quick_VDF_Elem() : Op_Conv_VDF_base(It_VDF_Elem(Eval_Quick_VDF_Elem)()) { }

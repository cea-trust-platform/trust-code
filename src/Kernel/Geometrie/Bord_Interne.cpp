/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Bord_Interne.h>

Implemente_instanciable_32_64(Bord_Interne_32_64, "Bord_Interne", Frontiere_32_64<_T_>);
// XD internes bord_base internes 0 To indicate that the block has a set of internal faces (these faces will be duplicated automatically by the program and will be processed in a manner similar to edge faces). NL2 Two boundaries with the same boundary conditions may have the same name (whether or not they belong to the same block). NL2 The keyword Internes (Internal) must be used to execute a calculation with plates, followed by the equation of the surface area covered by the plates.
// XD   attr nom chaine nom 0 Name of block side.
// XD   attr defbord defbord defbord 0 Definition of block side.

template <typename _SIZE_>
Sortie& Bord_Interne_32_64<_SIZE_>::printOn(Sortie& s) const { return Frontiere_32_64<_SIZE_>::printOn(s); }

template <typename _SIZE_>
Entree& Bord_Interne_32_64<_SIZE_>::readOn(Entree& s) { return Frontiere_32_64<_SIZE_>::readOn(s); }


template class Bord_Interne_32_64<int>;
#if INT_is_64_ == 2
template class Bord_Interne_32_64<trustIdType>;
#endif


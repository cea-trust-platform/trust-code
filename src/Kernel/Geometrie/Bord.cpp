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

#include <Bord.h>

Implemente_instanciable_32_64(Bord_32_64,"Bord",Frontiere_32_64<_T_>);

// XD defbord objet_lecture nul 0 Class to define an edge.

// XD defbord_2 defbord nul -1 1-D edge (straight line) in the 2-D space.
// XD   attr dir chaine(into=["X","Y"]) dir 0 Edge is perpendicular to this direction.
// XD   attr eq chaine(into=["="]) eq 0 Equality sign.
// XD   attr pos floattant pos 0 Position value.
// XD   attr pos2_min floattant pos2_min 0 Minimal value.
// XD   attr inf1 chaine(into=["<="]) inf1 0 Less than or equal to sign.
// XD   attr dir2 chaine(into=["X","Y"]) dir2 0 Edge is parallel to this direction.
// XD   attr inf2 chaine(into=["<="]) inf2 0 Less than or equal to sign.
// XD   attr pos2_max floattant pos2_max 0 Maximal value.

// XD defbord_3 defbord nul -1 2-D edge (plane) in the 3-D space.
// XD   attr dir chaine(into=["X","Y","Z"]) dir 0 Edge is perpendicular to this direction.
// XD   attr eq chaine(into=["="]) eq 0 Equality sign.
// XD   attr pos floattant pos 0 Position value.
// XD   attr pos2_min floattant pos2_min 0 Minimal value.
// XD   attr inf1 chaine(into=["<="]) inf1 0 Less than or equal to sign.
// XD   attr dir2 chaine(into=["X","Y"]) dir2 0 Edge is parallel to this direction.
// XD   attr inf2 chaine(into=["<="]) inf2 0 Less than or equal to sign.
// XD   attr pos2_max floattant pos2_max 0 Maximal value.
// XD   attr pos3_min floattant pos3_min 0 Minimal value.
// XD   attr inf3 chaine(into=["<="]) inf3 0 Less than or equal to sign.
// XD   attr dir3 chaine(into=["Y","Z"]) dir3 0 Edge is parallel to this direction.
// XD   attr inf4 chaine(into=["<="]) inf4 0 Less than or equal to sign.
// XD   attr pos3_max floattant pos3_max 0 Maximal value.

// XD bord bord_base bord 0 The block side is not in contact with another block and boundary conditions are applied to it.
// XD attr nom chaine nom 0 Name of block side.
// XD attr defbord defbord defbord 0 Definition of block side.

/*! @brief Simple appel a: Frontiere::printOn(Sortie&)
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Bord_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return Frontiere_32_64<_SIZE_>::printOn(s) ;
}

/*! @brief Simple appel a: Frontiere::readOn(Entree&)
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entre modifie
 */
template <typename _SIZE_>
Entree& Bord_32_64<_SIZE_>::readOn(Entree& s)
{
  return Frontiere_32_64<_SIZE_>::readOn(s) ;
}


template class Bord_32_64<int>;
#if INT_is_64_ == 2
template class Bord_32_64<trustIdType>;
#endif

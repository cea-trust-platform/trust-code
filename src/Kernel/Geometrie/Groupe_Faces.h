/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Groupe_Faces_included
#define Groupe_Faces_included

#include <Frontiere.h>

/*! @brief Classe Groupe_Face La classe sert a representer une selection de faces lu dans le fichier med
 *
 * @sa Frontiere Domaine
 */
template <typename _SIZE_>
class Groupe_Faces_32_64 : public Frontiere_32_64<_SIZE_>
{
  Declare_instanciable_32_64(Groupe_Faces_32_64);

public:
  using ArrOfInt_t = AOInt_T<_SIZE_>;


  inline const ArrOfInt_t& get_indices_faces() const { return indices_faces; }
  inline ArrOfInt_t& get_indices_faces() { return indices_faces; }

private:
  ArrOfInt_t indices_faces;
};


using Groupe_Faces = Groupe_Faces_32_64<int>;
using Groupe_Faces_64 = Groupe_Faces_32_64<trustIdType>;

#endif /* Groupe_Faces_included */

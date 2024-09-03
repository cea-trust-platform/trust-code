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

#ifndef Raccords_included
#define Raccords_included

#include <Raccord_base.h>
#include <TRUST_Deriv.h>
#include <TRUST_List.h>

/*! @brief Classe Raccords Cette represente une liste d'objets de type Raccord.
 *
 * @sa Raccord
 */
template <typename _SIZE_>
class Raccords_32_64 : public LIST(OWN_PTR(Raccord_base_32_64<_SIZE_>))
{
  Declare_instanciable_32_64(Raccords_32_64);
public :
  using int_t = _SIZE_;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  void associer_domaine(const Domaine_t&);
  int_t nb_faces() const;
  /// Renvoie le nombre de raccords contenus dans la liste.
  inline int nb_raccords() const { return this->size(); }
  int_t nb_faces(Type_Face type) const;
};


using Raccords = Raccords_32_64<int>;
using Raccords_64 = Raccords_32_64<trustIdType>;

#endif /* Raccords_included */

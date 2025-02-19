/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Champs_compris_included
#define Champs_compris_included

#include <TRUST_List.h>
#include <TRUST_Ref.h>
#include <Noms.h>
#include <IJK_Field_forward.h>
#include <unordered_map>

class Champ_base;

/*! @brief classe Champs_compris Represente un champ compris par un objet de type Equation, Milieu,
 *
 *      Operateur, Source, Traitement_particulier.
 *
 * Parametrized by the type of field: typically Champ_base or IJK_Field_double
 */
template<typename FIELD_TYPE>
class Champs_compris_T
{
public :
  // Return the field if found, otherwise raises.
  const FIELD_TYPE& get_champ(const Motcle& nom) const;
  // Same thing, but without raising:
  bool has_champ(const Motcle& nom, OBS_PTR(FIELD_TYPE)& ref_champ) const;
  bool has_champ(const Motcle& nom) const;
  void ajoute_champ(const FIELD_TYPE& champ);
  const Noms liste_noms_compris() const;
  void clear_champs_compris() { liste_champs_.clear(); }

protected :
  std::unordered_map<std::string, OBS_PTR(FIELD_TYPE)> liste_champs_;
};

using Champs_compris = Champs_compris_T<Champ_base>;
using Champs_compris_IJK = Champs_compris_T<IJK_Field_double>;

#endif /* Champs_compris_included */

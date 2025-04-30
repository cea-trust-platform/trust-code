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

#ifndef Champs_compris_IJK_included
#define Champs_compris_IJK_included

#include <Champs_compris.h>
#include <IJK_Field_vector.h>

class Champ_base;

/*! @brief Same as Champs_compris, but specialised for IJK fields and also supports quering for vectorial fields
 * (aka IJK_Field_vector3_double).
 *
 * Note that this class has no member of type "Champs_compris_IJK". This member is kept separate and inserted
 * in Navier_Stokes_FTD_IJK, IJK_Interfaces, etc., because in some cases (typically at the probleme level Probleme_FTD_IJK_base)
 * we do not need the member, just the abstract interface below.
 */
class Champs_compris_IJK: public Champs_compris_IJK_base
{
public :
  void ajoute_champ_vectoriel(const IJK_Field_vector3_double& champ);
  bool has_champ_vectoriel(const Motcle& nom) const;
  const IJK_Field_vector3_double& get_champ_vectoriel(const Motcle& nom) const;
  const Noms liste_noms_compris_vectoriel() const;
  const Noms liste_noms_compris() const;

  void switch_ft_fields();

private:
  void switch_field(const Nom& field_name, const Nom& prefix);
  void switch_vector_field(const Nom& field_name, const Nom& prefix);
  static constexpr std::array<const char*,3> COMPOS = {"_X", "_Y", "_Z"};

  std::unordered_map<std::string, OBS_PTR(IJK_Field_vector3_double)> liste_champs_vecto_;
};


#endif /* Champs_compris_IJK_included */

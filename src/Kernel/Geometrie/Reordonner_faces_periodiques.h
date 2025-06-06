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

#ifndef Reordonner_faces_periodiques_included
#define Reordonner_faces_periodiques_included

#include <Interprete_geometrique_base.h>
#include <TRUSTTabs_forward.h>
#include <Domaine_forward.h>

/*! @brief Cet interprete permet de reordonner les faces d'un bord periodique selon la convention utilisee dans le decoupeur (d'abord les faces d'une extremite du domaine, puis dans le
 *
 *   meme ordre, les faces jumelles sur le bord oppose).
 *
 */
template <typename _SIZE_>
class Reordonner_faces_periodiques_32_64 : public Interprete_geometrique_base_32_64<_SIZE_>
{
public:
  using int_t = _SIZE_;

  static int reordonner_faces_periodiques(const Domaine_32_64<_SIZE_>& domaine, IntTab_T<_SIZE_>& faces, const ArrOfDouble& direction_perio, const double epsilon);

  static int check_faces_periodiques(const Frontiere_32_64<_SIZE_>& frontiere, ArrOfDouble& vecteur_delta, ArrOfDouble& erreur,
                                     bool verbose=false);

  static void renum_som_perio(const Domaine_32_64<_SIZE_>& dom, const Noms& liste_bords_periodiques,
                              ArrOfInt_T<_SIZE_>& renum_som_perio, bool calculer_espace_virtuel);

  static void chercher_direction_perio(ArrOfDouble& direction_perio, const Domaine_32_64<_SIZE_>& dom, const Nom& bord);
};

using Reordonner_faces_periodiques = Reordonner_faces_periodiques_32_64<int>;
using Reordonner_faces_periodiques_64 = Reordonner_faces_periodiques_32_64<trustIdType>;

#endif

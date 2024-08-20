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
#ifndef Partitionneur_Tranche_included
#define Partitionneur_Tranche_included

#include <Partitionneur_base.h>
#include <TRUSTTabs_forward.h>
#include <TRUST_Ref.h>

#include <Domaine_forward.h>

/*! @brief Partitionneur de domaine en tranches paralleles aux directions de l'espace.
 *
 * Voir construire_partition
 *
 */
template <typename _SIZE_>
class Partitionneur_Tranche_32_64 : public Partitionneur_base_32_64<_SIZE_>
{
  Declare_instanciable_32_64(Partitionneur_Tranche_32_64);

public:
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  using BigIntVect_ = TRUSTVect<int, _SIZE_>;  // always int as value type, will hold proc/partition number.

  void set_param(Param& param) override;
  void associer_domaine(const Domaine_t& domaine) override;
  void initialiser(const ArrOfInt& nb_tranches);
  void construire_partition(BigIntVect_& elem_part, int& nb_parts_tot) const override;
  static void chercher_direction_perio(const Domaine_t& domaine, const Noms& liste_bords_perio, ArrOfInt& directions_perio);

private:
  // Parametres du partitionneur
  OBS_PTR(Domaine_t) ref_domaine_;

  // Pour chaque dimension d'espace, 2 ou 3, nombre de tranches
  // a creer dans cette direction (>=1)
  ArrOfInt nb_tranches_;
};

using Partitionneur_Tranche = Partitionneur_Tranche_32_64<int>;
using Partitionneur_Tranche_64 = Partitionneur_Tranche_32_64<trustIdType>;

#endif

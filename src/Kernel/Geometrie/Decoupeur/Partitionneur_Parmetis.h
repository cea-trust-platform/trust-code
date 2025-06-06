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

#ifndef Partitionneur_Parmetis_included
#define Partitionneur_Parmetis_included

#include <Partitionneur_base.h>
#include <TRUST_Ref.h>

#include <Domaine_forward.h>

/*! @brief Partition d'un domaine en nb_parties parties equilibrees en utilisant la librairie PARMETIS.
 *
 * Voir construire_partition
 *
 */

class Partitionneur_Parmetis : public Partitionneur_base
{
  Declare_instanciable(Partitionneur_Parmetis);
public:

  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_domaine(const Domaine& domaine) override;
  void construire_partition(IntVect& elem_part, int& nb_parts_tot) const override;

private:

  // Parametres du partitionneur
  OBS_PTR(Domaine) ref_domaine_;
  int nb_parties_ = -1;

  // Drapeau: utiliser ou pas la ponderation des edges dans parmetis.
  bool use_weights_ = false;

};
#endif

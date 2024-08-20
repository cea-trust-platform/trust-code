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

#ifndef Partitionneur_Metis_included
#define Partitionneur_Metis_included

#include <Partitionneur_base.h>
#include <TRUST_Ref.h>

#include <Domaine_forward.h>


/*! @brief Partition d'un domaine en nb_parties parties equilibrees en utilisant la librairie METIS.
 *
 * Voir construire_partition
 *
 */
template <typename _SIZE_>
class Partitionneur_Metis_32_64 : public Partitionneur_base_32_64<_SIZE_>
{
  Declare_instanciable_32_64(Partitionneur_Metis_32_64);

public:
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  using BigIntVect_ = TRUSTVect<int, _SIZE_>;

  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void associer_domaine(const Domaine_t& domaine) override { ref_domaine_ = domaine; }
  void construire_partition(BigIntVect_& elem_part, int& nb_parts_tot) const override;


private:

  // Parametres du partitionneur
  OBS_PTR(Domaine_t) ref_domaine_;
  int nb_parties_ = -1;

  // Options de metis
  enum AlgoMetis { PMETIS, KMETIS };
  AlgoMetis algo_ = PMETIS;
  int nb_essais_ = -1;

  // Drapeau: utiliser ou pas la ponderation des edges dans metis.
  //  C'est mieux de l'utiliser s'il y a des bords periodiques, le
  //  graphe est mieux equilibre. En revanche, metis utilisera plus
  //  de memoire (peut poser probleme sur les gros maillages).
  //  Cette option n'est pas indispensable: le maillage genere est
  //  valide dans avec ou sans l'option car on verifie de toutes facons
  //  la partition generee par metis (voir (***))
  int use_weights_ = 0;
  int use_segment_to_build_connectivite_elem_elem_ = 0;

};
#ifndef NO_METIS
typedef struct
{
  const char * name;
  int option;
} Metis_String_Option;

#endif
#endif

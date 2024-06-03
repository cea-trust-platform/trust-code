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

#ifndef Champ_Fonc_MED_Table_Temps_included
#define Champ_Fonc_MED_Table_Temps_included

#include <Champ_Fonc_MED.h>
#include <Champ_lu_proto.h>
#include <Table.h>

class Param;

/*! @brief : class Champ_Fonc_MED_Table_Temps
 *
 *  <Description of class Champ_Fonc_MED_Table_Temps>
 */
class Champ_Fonc_MED_Table_Temps : public Champ_Fonc_MED, public Champ_lu_proto
{
  Declare_instanciable( Champ_Fonc_MED_Table_Temps ) ;
public:
  void lire_donnees_champ(const std::string& fileName, const std::string& meshName, const std::string& fieldName, ArrOfDouble& temps_sauv, int& size, int& nbcomp, Nom& type_champ) override;
  void lire(double tps, int given_iteration = -1) override;

protected :
  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  inline virtual const Champ_Fonc_base& le_champ0() const { return vrai_champ0_.valeur(); }
  inline virtual Champ_Fonc_base& le_champ0() { return vrai_champ0_.valeur(); }

  Table la_table;
  Champ_Fonc vrai_champ0_;
  bool table_lue_ = false;
};

#endif /* Champ_Fonc_MED_Table_Temps_included */

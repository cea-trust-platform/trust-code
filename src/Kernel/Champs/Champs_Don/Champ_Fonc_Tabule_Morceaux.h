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

#ifndef Champ_Fonc_Tabule_Morceaux_included
#define Champ_Fonc_Tabule_Morceaux_included

#include <TRUSTChamp_Morceaux_generique.h>
#include <Ref_Champ_base.h>
#include <TRUST_Vector.h>
#include <Table.h>

/*! @brief classe Champ_Fonc_Tabule_Morceaux Cette classe represente un champ prenant par morceaux des valuers fonctions
 *
 *      de l'espace et d'un autre champ scalaire passe en parametre .
 *
 * @sa : TRUSTChamp_Morceaux_generique
 */
class Champ_Fonc_Tabule_Morceaux : public TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC_TABULE>
{
  Declare_instanciable(Champ_Fonc_Tabule_Morceaux);
public :
  void mettre_a_jour(double temps) override;
  int initialiser(const double temps) override;

protected :
  using ArrStr = std::array<std::string, 2>;

  std::set<ArrStr> s_pb_ch; // couples { probleme, champ } utilises par au moins un probleme
  std::vector<std::vector<ArrStr>> m_pb_ch; //m_pb_ch[i][j] : { probleme, champ } du parametre j du morceau i
  std::vector<const Champ_base *> ch_param; /* liste de champs parametres */

  /* un morceau de champ */
  typedef struct
  {
    std::vector<int> i_ch; /* indices des champs parametres utilises pour ce morceau */
    Table la_table;
  } CHTAB;
  std::vector<CHTAB> morceaux; /* les morceaux */
  IntVect i_mor; //i_mor[e] : morceau de l'element e
};

#endif /* Champ_Fonc_Tabule_Morceaux_included */

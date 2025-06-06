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

#ifndef Champ_Front_Fonction_included
#define Champ_Front_Fonction_included

#include <Ch_front_var_instationnaire_dep.h>
#include <TRUST_Ref.h>
#include <Table.h>

class Probleme_base;

/*! @brief Classe Champ_Front_Fonction Classe derivee de Champ_front_var_instationnaire_dep qui represente les
 *
 *      champs fonctions d'un autre champ par une fonction
 *
 *
 *
 */
class Champ_Front_Fonction : public Ch_front_var_instationnaire_dep
{
  Declare_instanciable(Champ_Front_Fonction);
public:
  void mettre_a_jour(double temps) override;
  inline Nom& nom_champ_parametre()
  {
    return nom_champ_parametre_;
  };
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  Champ_front_base& affecter_(const Champ_front_base& ch) override;
protected:
  Nom nom_champ_parametre_;
  Table la_table;
  OBS_PTR(Probleme_base) ref_pb;
};


#endif


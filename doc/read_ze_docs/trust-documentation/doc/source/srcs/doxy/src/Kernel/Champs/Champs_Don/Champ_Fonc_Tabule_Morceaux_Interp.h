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

#ifndef Champ_Fonc_Tabule_Morceaux_Interp_included
#define Champ_Fonc_Tabule_Morceaux_Interp_included

#include <Champ_Fonc_Tabule_Morceaux.h>
#include <Champ_Fonc_Interp.h>
#include <TRUST_Vector.h>
#include <TRUST_Ref.h>
#include <Table.h>

class Champ_base;

/*! @brief classe Champ_Fonc_Tabule_Morceaux_Interp Cette classe represente un champ prenant par morceaux des valuers fonctions
 *
 *      de l'espace et d'un autre champ scalaire passe en parametre .
 *
 * @sa : TRUSTChamp_Morceaux_generique
 */
class Champ_Fonc_Tabule_Morceaux_Interp : public Champ_Fonc_Tabule_Morceaux
{
  Declare_instanciable(Champ_Fonc_Tabule_Morceaux_Interp);
public :

  void mettre_a_jour(double temps) override;
  int initialiser(const double temps) override;

protected :
  std::vector<Champ_Fonc_Interp> ch_param_interp; /* liste de champs parametres */
};

#endif /* Champ_Fonc_Tabule_Morceaux_Interp_included */

/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Champ_Uniforme_Morceaux_inst_included
#define Champ_Uniforme_Morceaux_inst_included


#include <Champ_Uniforme_Morceaux.h>


/*! @brief Classe Champ_Uniforme_Morceaux_inst Representent un champ constant par morceaux dans l'espace et dependant
 *
 *     du temps.
 *
 * @sa Champ_Uniforme_Morceaux Champ_Uniforme, Classe abstraite, Methode abstraite, void me_calculer(double t)
 */
class Champ_Uniforme_Morceaux_inst : public Champ_Uniforme_Morceaux
{

  Declare_base(Champ_Uniforme_Morceaux_inst);

public :

  inline void mettre_a_jour(double ) override;
  virtual void me_calculer(double t) =0;

private :

//  double temps_;
};


/*! @brief Effectue une mise a jour en temps du champ.
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Champ_Uniforme_Morceaux_inst::mettre_a_jour(double un_temps)
{
  me_calculer(un_temps);
  changer_temps(un_temps);
}

#endif


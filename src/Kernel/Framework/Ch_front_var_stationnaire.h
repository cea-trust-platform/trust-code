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

#ifndef Ch_front_var_stationnaire_included
#define Ch_front_var_stationnaire_included

#include <Champ_front_var.h>

/*! @brief classe Ch_front_var_stationnaire Cette classe abstraite represente un champ sur une frontiere,
 *
 *      variable en espace mais stationnaire en temps.
 *      De ce fait, la methode initialiser doit calculer le champ une fois
 *      pour toutes, et la methode mettre_a_jour ne doit rien faire.
 *
 */

class Ch_front_var_stationnaire : public Champ_front_var
{
  Declare_base(Ch_front_var_stationnaire);

public :
  bool has_valeurs_au_temps(double temps) const override ;
  DoubleTab& valeurs_au_temps(double temps) override;
  const DoubleTab& valeurs_au_temps(double temps) const override;
  int avancer(double temps) override;
  int reculer(double temps) override;
  Champ_front_base& affecter_(const Champ_front_base& ch) override;
  void changer_temps_futur(double temps,int i) override;
};
#endif

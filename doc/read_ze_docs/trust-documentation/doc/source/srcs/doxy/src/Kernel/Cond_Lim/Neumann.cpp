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

#include <Neumann.h>

Implemente_base(Neumann, "Neumann", Cond_lim_base);

Sortie& Neumann::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Neumann::readOn(Entree& s) { return Cond_lim_base::readOn(s); }

/*! @brief Renvoie la valeur du flux impose sur la i-eme composante du champ representant le flux a la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Neumann::flux_impose(int i) const
{
  if (le_champ_front.valeurs().size() == 1)
    return le_champ_front(0, 0);
  else if (le_champ_front.valeurs().dimension(1) == 1)
    return le_champ_front(i, 0);
  else
    Cerr << "Neumann::flux_impose error" << finl;
  Process::exit();
  return 0.;
}

/*! @brief Renvoie la valeur du flux impose sur la (i,j)-eme composante du champ representant le flux a la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 */
double Neumann::flux_impose(int i, int j) const
{
  if (le_champ_front.valeurs().dimension(0) == 1)
    return le_champ_front(0, j);
  else
    return le_champ_front(i, j);
}

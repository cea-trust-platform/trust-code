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

#include <Esp_Dist.h>

Implemente_instanciable_sans_constructeur(Esp_Dist,"Esp_Dist",ArrOfInt);

/*! @brief Constructeur par defaut
 *
 */
Esp_Dist::Esp_Dist() : PE_voisin_(-1) { }

/*! @brief Lecture d'un espace distant dans un flot d'entree.
 *
 * Un espace distant est represente par son tableau d'indices et le processus voisin concerne.
 *
 */
Entree& Esp_Dist::readOn(Entree& is)
{
  ArrOfInt::readOn(is);
  is >> PE_voisin_;
  is >> desc_ed_;
  return is;
}

/*! @brief Ecriture d'un espace distant sur un flot de sortie Un espace distant est represente par son tableau d'indices et le processus voisin concerne.
 *
 */
Sortie& Esp_Dist::printOn(Sortie& os) const
{
  ArrOfInt::printOn(os);
  os << PE_voisin_<<finl;
  os << desc_ed_;
  return os;
}

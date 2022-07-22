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

#include <Neumann_homogene.h>

Implemente_base(Neumann_homogene,"Neumann_homogene",Cond_lim_base);
// XD Neumann_homogene condlim_base Neumann_homogene -1 Homogeneous neumann boundary condition


/*! @brief Ecrit le type de l'objet sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Neumann_homogene::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

/*! @brief Type le champ a la frontiere en "Champ_front_uniforme" N'ecrit rien sur le flot passe en parametre
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree& s) le flot d'entree
 */
Entree& Neumann_homogene::readOn(Entree& s )
{
  le_champ_front.typer("Champ_front_uniforme");
  return s ;
}

/*! @brief Renvoie la valeur du flux impose sur la i-eme composante du champ representant le flux a la frontiere.
 *
 *     Toujours 0 pour Neumann homogene.
 *
 * @param (int) indice suivant la premiere dimension du champ
 * @return (double) la valeur du flux imposee sur la composante specifiee
 */
double Neumann_homogene::flux_impose(int ) const
{
  return 0;
}

/*! @brief Renvoie la valeur du flux impose sur la (i,j)-eme composante du champ representant le flux a la frontiere.
 *
 *     Toujours 0 pour Neumann homogene.
 *
 * @param (int) indice suivant la premiere dimension du champ
 * @param (int) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante specifiee
 */
double Neumann_homogene::flux_impose(int ,int ) const
{
  return 0;
}

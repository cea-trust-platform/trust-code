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

#ifndef Perte_Charge_included
#define Perte_Charge_included





/*! @brief Classe Perte_Charge Cette classe represente un terme de perte de charge quiest introduit
 *
 *     dans les equations de Navier-Stokes pour modeliser la presence d'un
 *     certain type d'obstacle dans l'ecoulement.
 *     On utilise la notion de perte de charge pour des obstacles sur lesquels
 *     on ne calcule pas le frottement (obstacles internes aux mailles, ou
 *     grilles par exemple)
 *     Un objet de type Perte_Charge s'applique sur
 *     une seule direction d'espace (direction_perte_charge() >= 0 )
 *     ou sur toutes (direction_perte_charge() == -1)
 *
 * @sa Ne derive pas d'Objet_U
 */
class Perte_Charge
{

public :

  inline int direction_perte_charge() const;

protected :

  int direction_perte_charge_;
};


/*! @brief Renvoie la direction de perte de charge.
 *
 * (X,Y ou Z), ou -1 si toutes les directions
 *
 * @return (int) la direction de perte de charge
 */
inline int Perte_Charge::direction_perte_charge() const
{
  return direction_perte_charge_;
}

#endif

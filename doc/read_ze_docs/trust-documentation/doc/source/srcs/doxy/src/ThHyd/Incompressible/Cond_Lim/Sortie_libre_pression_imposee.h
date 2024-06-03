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

#ifndef Sortie_libre_pression_imposee_included
#define Sortie_libre_pression_imposee_included

#include <Neumann_sortie_libre.h>

/*! @brief classe Sortie_libre_pression_imposee Cette classe derive de Neumann_sortie_libre
 *
 *     Elle represente une frontiere ouverte avec condition de pression imposee.
 *     L'objet de type Champ_bord le_champ_bord contient la pression et la fonction flux_impose() renvoie les valeurs de cette pression.
 *     champ_ext contient une valeur de la vitesse du fluide a l'exterieur accessible par la methode val_ext()
 *
 * @sa Neumann_sortie_libre
 */
class Sortie_libre_pression_imposee: public Neumann_sortie_libre
{
  Declare_instanciable(Sortie_libre_pression_imposee);
public:
  double flux_impose(int i) const override;
  double flux_impose(int i, int j) const override;
  void completer() override;

protected:
  double d_rho;
};

#endif

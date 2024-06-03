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

#include <EDO_Pression_th.h>

Implemente_instanciable(EDO_Pression_th,"EDO_Pression_th",DERIV(EDO_Pression_th_base));

Sortie& EDO_Pression_th::printOn(Sortie& os) const
{
  return DERIV(EDO_Pression_th_base)::printOn(os);
}

Entree& EDO_Pression_th::readOn(Entree& is)
{
  return is;
}

/*! @brief Type la loi d'etat
 *
 * @param (Nom& typ) le nom de type a donner a la loi d'etat
 */
void EDO_Pression_th::typer(const Nom& type)
{
  Nom typ = "EDO_Pression_th_";
  typ += type;
  Cerr<<"Typage de l'EDO sur la pression : ";
  DERIV(EDO_Pression_th_base)::typer(typ);

  Cerr<<" "<<valeur().que_suis_je()<<finl;
}

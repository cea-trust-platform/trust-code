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

#include <Cond_lim.h>

Implemente_instanciable(Cond_lim,"Cond_lim",DERIV(Cond_lim_base));

Sortie& Cond_lim::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Cond_lim::readOn(Entree& s )
{
  return DERIV(Cond_lim_base)::readOn(s) ;
}

/*! @brief Adopte la Cond_lim passee en parametre.
 *
 * Il n'y a pas de copie.
 *     La Cond_lim_base eventuellement detenue avant l'appel est detruite.
 *     ATTENTION cl est nul ensuite
 *
 * @param (Cond_lim& cl)
 */
void Cond_lim::adopt(Cond_lim& cl)
{
  detach();
  set_Objet_U_ptr(&(cl.valeur()));
  cl.set_Objet_U_ptr(NULL);
}

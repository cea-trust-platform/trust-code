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

#include <Domaine_dis.h>

Implemente_instanciable(Domaine_dis,"Domaine_dis",DERIV(Domaine_dis_base));

Sortie& Domaine_dis::printOn(Sortie& os) const
{
  return DERIV(Domaine_dis_base)::printOn(os);
}

Entree& Domaine_dis::readOn(Entree& is)
{
  return DERIV(Domaine_dis_base)::readOn(is);
}


const Sous_domaine_dis& Domaine_dis::sous_domaine_dis(int i) const
{
  return valeur().sous_domaine_dis(i);
}

Sous_domaine_dis& Domaine_dis::sous_domaine_dis(int i)
{
  return valeur().sous_domaine_dis(i);
}

/*! @brief Type le Domaine_dis.
 */
void Domaine_dis::typer(const Nom& typ)
{
  Nom ze_typ(typ);
  ze_typ.suffix("NO_FACE_");
  DERIV(Domaine_dis_base)::typer(ze_typ);
}


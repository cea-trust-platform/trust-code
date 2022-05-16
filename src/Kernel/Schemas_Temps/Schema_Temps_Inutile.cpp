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

#include <Schema_Temps_Inutile.h>
#include <Equation.h>

Implemente_instanciable(Schema_Temps_Inutile,"Schema_Temps_Inutile",Schema_Temps_base);


Sortie& Schema_Temps_Inutile::printOn(Sortie& s) const
{
  return  Schema_Temps_base::printOn(s);
}

Entree& Schema_Temps_Inutile::readOn(Entree& s)
{
  return Schema_Temps_base::readOn(s) ;
}

int Schema_Temps_Inutile::nb_valeurs_temporelles() const
{
  return 0 ;
}

int Schema_Temps_Inutile::nb_valeurs_futures() const
{
  return 0 ;
}

double Schema_Temps_Inutile::temps_futur(int i) const
{
  return 0.;
}

double Schema_Temps_Inutile::temps_defaut() const
{
  return 0.;
}

int Schema_Temps_Inutile::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  return 1;
}

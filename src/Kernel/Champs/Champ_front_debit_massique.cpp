/****************************************************************************
* Copyright (c) 2017, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_front_debit_massique.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_debit_massique.h>
#include <Champ_Don.h>
#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Zone_VF.h>
#include <DoubleTrav.h>

Implemente_instanciable(Champ_front_debit_massique,"Champ_front_debit_massique",Champ_front_debit);

Sortie& Champ_front_debit_massique::printOn(Sortie& os) const
{
  return Champ_front_normal::printOn(os);
}

Entree& Champ_front_debit_massique::readOn(Entree& is)
{
  is >> champ_debit_;
  fixer_nb_comp(dimension);
  return is;
}

void Champ_front_debit_massique::initialiser_coefficient(const Champ_Inc_base& inco)
{
  const Champ_Don& masse_volumique = inco.equation().milieu().masse_volumique();
  coeff_ = masse_volumique.valeur().valeurs().nb_dim() == 1 ? masse_volumique(0) : masse_volumique(0, 0);
}

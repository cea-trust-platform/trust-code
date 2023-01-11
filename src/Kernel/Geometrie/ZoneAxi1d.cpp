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

#include <ZoneAxi1d.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur( ZoneAxi1d, "DomaineAxi1d", Zone ) ;
// XD ZoneAxi1d domaine ZoneAxi1d -1 1D domain
ZoneAxi1d::ZoneAxi1d()
{
  axi1d_ = 1;
}

Sortie& ZoneAxi1d::printOn( Sortie& os ) const
{
  Zone::printOn( os );
  return os;
}

Entree& ZoneAxi1d::readOn( Entree& is )
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void ZoneAxi1d::set_param(Param& param)
{
  param.ajouter("ORIGINE",&champ_orig,Param::REQUIRED);
}

const Champ& ZoneAxi1d::champ_origine()
{
  return champ_orig;
}

const Champ& ZoneAxi1d::champ_origine() const
{
  return champ_orig;
}

const DoubleTab& ZoneAxi1d::origine_repere()
{
  return ref_origine_.valeur();
}

const DoubleTab& ZoneAxi1d::origine_repere() const
{
  return ref_origine_.valeur();
}

void ZoneAxi1d::associer_origine_repere(const DoubleTab& orig)
{
  ref_origine_ = orig;
}

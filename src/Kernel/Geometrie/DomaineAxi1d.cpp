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

#include <DomaineAxi1d.h>

Implemente_instanciable_sans_constructeur( DomaineAxi1d, "DomaineAxi1d", Domaine ) ;
// XD DomaineAxi1d domaine DomaineAxi1d -1 1D domain
DomaineAxi1d::DomaineAxi1d()
{
  axi1d = 1;
}

Sortie& DomaineAxi1d::printOn( Sortie& os ) const
{
  Domaine::printOn( os );
  return os;
}

Entree& DomaineAxi1d::readOn( Entree& is )
{
  const Motcle accolade_ouverte("{");
  const Motcle accolade_fermee("}");

  Motcle motlu;

  is >> motlu;

  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading 1D axi domain " <<  finl;
      Cerr << "We expected a " << accolade_ouverte << " instead of \n"
           << motlu << finl;
      Process::exit();
    }

  while (1)
    {
      is >> motlu;

      if (motlu == accolade_fermee)
        break;

      if (motlu=="ORIGINE")
        {
          is >> champ_orig;
        }
      else
        {
          Cerr << "Error while reading 1D axi domain " <<  finl;
          Cerr << "We expected a \"origine\" instead of \n"
               << motlu << finl;
          Process::exit();
        }
    }

  return is;
}

const Champ& DomaineAxi1d::champ_origine()
{
  return champ_orig;
}

const Champ& DomaineAxi1d::champ_origine() const
{
  return champ_orig;
}

const DoubleTab& DomaineAxi1d::origine_repere()
{
  return ref_origine_.valeur();
}

const DoubleTab& DomaineAxi1d::origine_repere() const
{
  return ref_origine_.valeur();
}

void DomaineAxi1d::associer_origine_repere(const DoubleTab& orig)
{
  ref_origine_ = orig;
}

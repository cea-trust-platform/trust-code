/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Liste_Champ_Generique.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Liste_Champ_Generique.h>

Implemente_liste(Champ_Generique);

Implemente_instanciable(Liste_Champ_Generique,"Liste_Champ_Generique",LIST(Champ_Generique));

Sortie& Liste_Champ_Generique::printOn(Sortie& os ) const
{
  return os;
}

Entree& Liste_Champ_Generique::readOn(Entree& is)
{
  return is;
}

int Liste_Champ_Generique::sauvegarder(Sortie& os) const
{
  int bytes=0;
  CONST_LIST_CURSEUR(Champ_Generique) curseur=*this;
  while(curseur)
    {
      bytes += curseur.valeur()->sauvegarder(os);
      ++curseur;
    }
  return bytes;
}

int Liste_Champ_Generique::reprendre(Entree& is)
{
  LIST_CURSEUR(Champ_Generique) curseur=*this;
  while(curseur)
    {
      curseur.valeur()->reprendre(is);
      ++curseur;
    }
  return 1;
}

void Liste_Champ_Generique::fixer_tstat_deb(double , double )
{

}

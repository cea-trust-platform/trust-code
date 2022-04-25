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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTSchema_RK.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/63
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTSchema_RK_included
#define TRUSTSchema_RK_included

#include<Schema_Temps_base.h>

enum class Ordre_RK { UN , RATIO_DEUX , DEUX , TROIS , QUATRE };

template <Ordre_RK _ORDRE_ >
class TRUSTSchema_RK : public Schema_Temps_base
{
  // Renvoie le nombre de valeurs temporelles a conserver. Ici : n et n+1, donc 2.
  int nb_valeurs_temporelles() const override { return 2 ; }

  // Renvoie le nombre de valeurs temporelles futures. Ici : n+1, donc 1.
  int nb_valeurs_futures() const override { return 1 ; }

  // Renvoie le le temps a la i-eme valeur future. Ici : t(n+1)
  double temps_futur(int i) const override
  {
    assert(i==1);
    return temps_courant()+pas_de_temps();
  }

  // Renvoie le le temps le temps que doivent rendre les champs a l'appel de valeurs(). Ici : t(n+1)
  double temps_defaut() const override { return temps_courant()+pas_de_temps(); }

  // a surcharger si utile
  void completer() override { /* Do nothing */ }
};

#endif /* TRUSTSchema_RK_included */

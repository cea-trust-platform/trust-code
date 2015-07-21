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
// File:        MuLambda_TBLE_Cte.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <MuLambda_TBLE_Cte.h>
#include <Eq_couch_lim.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(MuLambda_TBLE_Cte,"MuLambda_TBLE_Cte",MuLambda_TBLE_base);


Sortie& MuLambda_TBLE_Cte::printOn(Sortie& os) const
{
  return os;
}

Entree& MuLambda_TBLE_Cte::readOn(Entree& is)
{
  return is;
}




void MuLambda_TBLE_Cte::initialiser(const Milieu_base& milieu)
{
  if (!sub_type(Fluide_Incompressible, milieu))
    {
      Cerr << "Avec TBLE, vous devez utiliser un fluide incompressible " << finl;
      exit();
    }

  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,milieu);
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const Champ_Don& ch_alpha = le_fluide.diffusivite();
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      const DoubleTab& tab_visco = ch_visco_cin->valeurs();

      nu = max(tab_visco(0,0),DMINFLOAT);//visco cinematique supposee cste
    }
  else
    {
      Cerr << "Viscosite non constante : cas non traite encore dans TBLE" << finl;
      exit();
    }
  if (ch_alpha.non_nul())
    {
      if (sub_type(Champ_Uniforme,ch_alpha.valeur()))
        {
          const DoubleTab& tab_lambda = ch_alpha->valeurs();
          alpha = max(tab_lambda(0,0),DMINFLOAT);//lambda supposee cste
        }
      else
        {
          Cerr << "Diffusivite non constante : cas non traite encore dans TBLE" << finl;
          exit();
        }
    }

}


/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_front_fonc_gradient.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_fonc_gradient.h>
#include <Frontiere_dis_base.h>

Implemente_instanciable(Champ_front_fonc_gradient,"Champ_front_fonc_gradient",Champ_front_tangentiel);


Sortie& Champ_front_fonc_gradient::printOn(Sortie& os) const
{
  return Champ_front_tangentiel::printOn(os);
}


Entree& Champ_front_fonc_gradient::readOn(Entree& s)
{
  Cerr << "Champ_front_fonc_gradient::readOn" << finl;
  return s;
}

void Champ_front_fonc_gradient::associer_ch_inc_base(const Champ_Inc_base& inc)
{
  inconnue=inc;
}

const Cond_lim_base& Champ_front_fonc_gradient::condition_limite(const Nom& nom_bord)
{
  const Zone_Cl_dis_base& zcl=inconnue.valeur().equation().zone_Cl_dis().valeur();
  int n=zcl.nb_cond_lim();
  for(int i=0; i<n ; i++)
    {
      const Frontiere_dis_base& fr_dis=zcl.les_conditions_limites(i).frontiere_dis();
      if (fr_dis.le_nom() == nom_bord)
        return zcl.les_conditions_limites(i);
    }
  Cerr << nom_bord << " not found..." << finl;
  exit();
  return zcl.les_conditions_limites(0);
}

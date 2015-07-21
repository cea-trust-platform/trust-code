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
// File:        Terme_Puissance_Thermique_VDF_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Puissance_Thermique_VDF_base.h>
#include <Zone_VDF.h>
#include <Champ_val_tot_sur_vol_base.h>

Implemente_base(Terme_Puissance_Thermique_VDF_base,"Terme_Puissance_Thermique_VDF_base",Terme_Source_VDF_base);

////printOn
Sortie& Terme_Puissance_Thermique_VDF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

////readOn
Entree& Terme_Puissance_Thermique_VDF_base::readOn(Entree& s )
{
  const Equation_base& eqn = equation();
  Terme_Puissance_Thermique::lire_donnees(s,eqn);
  champs_compris_.ajoute_champ(la_puissance);
  return s ;
}

void Terme_Puissance_Thermique_VDF_base::associer_zones(const Zone_dis& zone_dis,
                                                        const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter->associer_zones(zvdf,zclvdf);
}

int Terme_Puissance_Thermique_VDF_base::initialiser(double temps)
{
  Terme_Source_VDF_base::initialiser(temps);

  if (sub_type(Champ_val_tot_sur_vol_base,la_puissance.valeur()))
    {
      const Zone_dis_base& zdis = equation().zone_dis().valeur();
      const Zone_Cl_dis_base& zcldis = equation().zone_Cl_dis().valeur();
      Champ_val_tot_sur_vol_base& champ_puis = ref_cast(Champ_val_tot_sur_vol_base,la_puissance.valeur());
      champ_puis.evaluer(zdis,zcldis);
    }
  return 1;
}



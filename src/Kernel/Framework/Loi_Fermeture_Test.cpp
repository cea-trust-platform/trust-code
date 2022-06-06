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

#include <Loi_Fermeture_Test.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Param.h>

Implemente_instanciable( Loi_Fermeture_Test, "Loi_Fermeture_Test", Loi_Fermeture_base ) ;
// XD loi_fermeture_test loi_fermeture_base loi_fermeture_test -1 Loi for test only

Sortie& Loi_Fermeture_Test::printOn( Sortie& os ) const
{
  Loi_Fermeture_base::printOn( os );
  return os;
}

Entree& Loi_Fermeture_Test::readOn( Entree& is )
{
  coef_=1;
  Loi_Fermeture_base::readOn( is );
  return is;
}

void Loi_Fermeture_Test::set_param(Param& param)
{
  param.ajouter("coef",&coef_); // XD_ADD_P double coefficient
}
void Loi_Fermeture_Test::mettre_a_jour(double temps)
{
  assert(status_ == COMPLET);
  champ_test_.valeurs()=temps*coef_;
  champ_test_.valeur().changer_temps(temps);
}
void Loi_Fermeture_Test::discretiser(const Discretisation_base& dis)
{
  Loi_Fermeture_base::discretiser(dis);
  const Probleme_base& pb = mon_probleme();
  const double temps = pb.schema_temps().temps_courant();
  const Zone_dis_base& la_zone_dis = pb.domaine_dis().zone_dis(0).valeur();


  dis.discretiser_champ("CHAMP_ELEM", la_zone_dis,"test_time", "s",1 /* nb composantes par defaut */,temps, champ_test_);

  champs_compris_.ajoute_champ(champ_test_);
}

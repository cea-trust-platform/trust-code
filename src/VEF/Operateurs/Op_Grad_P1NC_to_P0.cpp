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
// File:        Op_Grad_P1NC_to_P0.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_P1NC_to_P0.h>
#include <Les_Cl.h>
#include <Champ_P1NC.h>

Implemente_instanciable(Op_Grad_P1NC_to_P0,"Op_Grad_P1NC_to_P0",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_P1NC_to_P0::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_P1NC_to_P0::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Grad_P1NC_to_P0::associer(const Zone_dis& zone_dis,
                                  const Zone_Cl_dis& zone_Cl_dis,
                                  const Champ_Inc&)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF, zone_dis.valeur());
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
  la_zone_vef = zvef;
  la_zcl_vef = zclvef;

}

DoubleTab& Op_Grad_P1NC_to_P0::ajouter(const DoubleTab& valeurs_source, DoubleTab& grad) const
{
  Champ_P1NC::calcul_gradient(valeurs_source,grad,la_zcl_vef.valeur());
  grad.echange_espace_virtuel();
  return grad;
}

DoubleTab& Op_Grad_P1NC_to_P0::calculer(const DoubleTab& valeurs_source, DoubleTab& grad) const
{
  grad = 0;
  return ajouter(valeurs_source,grad);

}

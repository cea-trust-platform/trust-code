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



#include <grad_U_Champ_P1NC.h>
#include <Champ_P1NC.h>

Implemente_instanciable(grad_U_Champ_P1NC,"grad_U_Champ_P1NC",Champ_Fonc_P0_VEF);



Sortie& grad_U_Champ_P1NC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}



Entree& grad_U_Champ_P1NC::readOn(Entree& s)
{
  return s ;
}



void grad_U_Champ_P1NC::associer_champ(const Champ_P1NC& un_champ)
{
  mon_champ_= un_champ;
}



void grad_U_Champ_P1NC::me_calculer(double tps)
{
  mon_champ_->calcul_grad_U(la_zone_Cl_VEF.valeur(),valeurs());
}

const Zone_Cl_dis_base& grad_U_Champ_P1NC::zone_Cl_dis_base() const
{
  return la_zone_Cl_VEF.valeur();
}

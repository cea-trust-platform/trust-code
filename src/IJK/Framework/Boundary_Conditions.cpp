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

#include <Boundary_Conditions.h>
#include <IJK_Splitting.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Boundary_Conditions,"Boundary_Conditions",Objet_U);
double IJK_Splitting::shear_x_time_=0.;
int IJK_Splitting::defilement_=0;

Boundary_Conditions::Boundary_Conditions()
{
  bctype_kmin_ = Paroi;
  bctype_kmax_ = Paroi;
  vxkmin_ = 0.;
  vxkmax_ = 0.;
  dU_perio_ = 0.;
  t0_shear_=0.;
  defilement_=0;
}

// SYNTAXE:
//  {
//    BCTYPE_KMIN paroi|symetrie|perio|Mixte_shear
//    BCTYPE_KMAX paroi|symetrie|perio|Mixte_shear
//  }
Entree& Boundary_Conditions::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("vx_kmin", &vxkmin_);
  param.ajouter("dU_perio", &dU_perio_);
  param.ajouter("t0_shear", &t0_shear_);
  param.ajouter("defilement", &defilement_);
  param.ajouter("bctype_kmin", &bctype_kmin_, Param::REQUIRED);
  param.dictionnaire("Paroi", Paroi);
  param.dictionnaire("Symetrie", Symetrie);
  param.dictionnaire("Perio", Perio);
  param.dictionnaire("Mixte_shear", Mixte_shear);
  param.ajouter("vx_kmax", &vxkmax_);
  param.ajouter("bctype_kmax", &bctype_kmax_, Param::REQUIRED);
  param.dictionnaire("Paroi", Paroi);
  param.dictionnaire("Symetrie", Symetrie);
  param.dictionnaire("Perio", Perio);
  param.dictionnaire("Mixte_shear", Mixte_shear);

  param.lire_avec_accolades(is);
  IJK_Splitting::shear_x_time_=t0_shear_;
  IJK_Splitting::defilement_=defilement_;

  return is;
}

Sortie& Boundary_Conditions::printOn(Sortie& os) const { return os; }

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
// File:        Champ_front_debit_massique.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_debit_massique.h>
#include <Champ_Don.h>
#include <Champ_Inc_base.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Zone_VF.h>
#include <TRUSTTrav.h>

Implemente_instanciable(Champ_front_debit_massique,"Champ_front_debit_massique",Champ_front_debit);
// XD champ_front_debit_massique front_field_base champ_front_debit_massique 0 This field is used to define a flow rate field using the density
// XD attr ch front_field_base ch 0 uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_txyz that depends only on time.
Sortie& Champ_front_debit_massique::printOn(Sortie& os) const
{
  return Champ_front_debit::printOn(os);
}

Entree& Champ_front_debit_massique::readOn(Entree& is)
{
  return Champ_front_debit::readOn(is);
}

void Champ_front_debit_massique::initialiser_coefficient(const Champ_Inc_base& inco, double temps)
{
  Champ_front_debit::initialiser_coefficient(inco, temps);
  ch_rho = &inco.equation().milieu().masse_volumique().valeur();
  if (sub_type(Champ_Uniforme, *ch_rho)) //rho constant : on calcule le coeff maintenant, puis on ne le fait plus
    update_coeff(temps), update_coeff_ = false;
  else update_coeff_ = true;
}

void Champ_front_debit_massique::update_coeff(double temps)
{
  int i, fb, n, N = coeff_.line_size(), cR = sub_type(Champ_Uniforme, *ch_rho);
  DoubleTab rho_bord = cR ? ch_rho->valeurs() : ch_rho->valeur_aux_bords(); /* si rho uniforme, on ne peut pas appeler valeur_aux_bords() */
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis());
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  for(i = 0; i < le_bord.nb_faces_tot(); i++)
    for (fb = zone.fbord(le_bord.num_face(i)), n = 0; n < N; ++n)
      coeff_(i, n) = 1. / rho_bord(!cR * fb, n);
}

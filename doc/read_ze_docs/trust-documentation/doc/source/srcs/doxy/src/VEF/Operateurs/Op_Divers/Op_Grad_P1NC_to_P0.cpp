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

#include <Op_Grad_P1NC_to_P0.h>
#include <Champ_P1NC.h>

Implemente_instanciable(Op_Grad_P1NC_to_P0, "Op_Grad_P1NC_to_P0", Operateur_Grad_base);

Sortie& Op_Grad_P1NC_to_P0::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_P1NC_to_P0::readOn(Entree& s) { return s; }

void Op_Grad_P1NC_to_P0::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis, const Champ_Inc&)
{
  le_dom_vef = ref_cast(Domaine_VEF, domaine_dis.valeur());
  la_zcl_vef = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
}

DoubleTab& Op_Grad_P1NC_to_P0::ajouter(const DoubleTab& valeurs_source, DoubleTab& grad) const
{
  Champ_P1NC::calcul_gradient(valeurs_source, grad, la_zcl_vef.valeur());
  grad.echange_espace_virtuel();
  return grad;
}

DoubleTab& Op_Grad_P1NC_to_P0::calculer(const DoubleTab& valeurs_source, DoubleTab& grad) const
{
  grad = 0;
  return ajouter(valeurs_source, grad);
}

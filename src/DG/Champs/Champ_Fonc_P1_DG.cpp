/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Champ_Fonc_P1_DG.h>
#include <Domaine_DG.h>
#include <Quadrature_base.h>

Implemente_instanciable(Champ_Fonc_P1_DG, "Champ_Fonc_P1_DG", Champ_Fonc_P0_base);

Sortie& Champ_Fonc_P1_DG::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_P1_DG::readOn(Entree& s) { return s; }

Champ_base& Champ_Fonc_P1_DG::affecter_(const Champ_base& ch)
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG, le_dom_VF.valeur());

  const Quadrature_base& quad = domaine.get_quadrature(5);
  const DoubleTab& integ_points = quad.get_integ_points();
  int nb_pts_integ = quad.nb_pts_integ();
  int nb_elem_tot = domaine.nb_elem_tot();

  DoubleTab values;
  values.ref_tab(valeurs_,0,nb_elem_tot);
  values.reshape(nb_elem_tot*nb_pts_integ,1);

  ch.valeur_aux(integ_points, values);

  return *this;
}

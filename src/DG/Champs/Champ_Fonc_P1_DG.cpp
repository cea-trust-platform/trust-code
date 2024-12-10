/****************************************************************************
* Copyright (c) 2025, CEA
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

  const Quadrature_base& quad = domaine.get_quadrature(2);
  const DoubleTab& integ_points = quad.get_integ_points();
  int nb_elem = domaine.nb_elem();

  DoubleTab values(integ_points.dimension(0),1);

  ch.valeur_aux(integ_points, values);

  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      for (int k = 0; k < quad.nb_pts_integ(num_elem) ; k++)
        valeurs_(num_elem,k) = values(quad.ind_pts_integ(num_elem)+k);
    }

  return *this;
}

DoubleTab& Champ_Fonc_P1_DG::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{

  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  const DoubleVect& volume = domaine.volumes();

  const Quadrature_base& quad = domaine.get_quadrature(2);
  int nb_pts_integ_max = quad.nb_pts_integ_max();

  const DoubleTab& values = le_champ().valeurs();
  int nb_polys = polys.size();

  if (nb_polys == 0)
    return result;

  // TODO : FIXME
  // For FT the resize should be done in its good position and not here ...
  if (result.nb_dim() == 1) result.resize(nb_polys, 1);

  assert(result.line_size() == 1);
  ToDo_Kokkos("critical");

  DoubleTab value_pts(nb_pts_integ_max);

  for (int i = 0; i < nb_polys; i++)
    {
      int cell = polys(i);
      assert(cell < values.dimension_tot(0));

      if (cell != -1)
        {
          for (int k=0; k<quad.nb_pts_integ(cell); k++)
            value_pts(k) = values(cell,k);
//          value_pts.ref_tab(values,cell,1); //pb de const

          result(i,0) = quad.compute_integral_on_elem(cell, value_pts);
          result(i,0) /= volume(cell); //TODO DG est ce necessaire de multiplier dans integral pour diviser deriere ?
        }
    }

  return result;

}


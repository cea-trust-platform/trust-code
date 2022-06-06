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

#include <Champ_implementation_sommet_base.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Champ_base.h>

DoubleVect& Champ_implementation_sommet_base::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  const Champ_base& ch_base       = le_champ();
  int            nb_components = ch_base.nb_comp();
  const DoubleTab&  values        = ch_base.valeurs();
  assert(result.size() == nb_components);
  assert(position.size() == Objet_U::dimension);
  DoubleTab positions;
  positions = position;
  DoubleTab resu(1, nb_components);
  ArrOfInt polys(1);
  polys[0] = poly;
  value_interpolation(positions,polys,values,resu);
  for (int j=0; j<nb_components; j++)
    result(j) = resu(j, 0) ;
  return result;
}

double Champ_implementation_sommet_base::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  const Champ_base& ch_base       = le_champ();
  const DoubleTab&  values        = ch_base.valeurs();
  assert(ncomp>=0);
  assert(ncomp<ch_base.nb_comp());
  assert(position.size() == Objet_U::dimension);
  DoubleTab positions;
  positions = position;
  DoubleTab resu(1, 1);
  ArrOfInt polys(1);
  polys[0] = poly;
  value_interpolation(positions,polys,values,resu,ncomp);
  return resu(0, 0);
}

DoubleTab& Champ_implementation_sommet_base::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{
  const Champ_base& ch_base       = le_champ();
  const DoubleTab&  values        = ch_base.valeurs();
  assert((result.dimension(0) == polys.size())||(result.dimension_tot(0) == polys.size()));
  assert(positions.dimension(1) == Objet_U::dimension);
  value_interpolation(positions,polys,values,result);
  return result;
}

DoubleVect& Champ_implementation_sommet_base::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const
{
  const Champ_base& ch_base       = le_champ();
  const DoubleTab&  values        = ch_base.valeurs();
  int nb_polys = polys.size();

  assert(ncomp>=0);
  assert(ncomp<ch_base.nb_comp());
  assert(result.size() == nb_polys);
  assert(positions.dimension(1) == Objet_U::dimension);
  DoubleTab resu(nb_polys, 1);
  value_interpolation(positions, polys, values, resu);
  for(int i=0; i<nb_polys; i++)
    result(i)=resu(i,0);
  return result;
}

DoubleTab& Champ_implementation_sommet_base::remplir_coord_noeuds(DoubleTab& positions) const
{
  const Zone& zone = get_zone_geom();
  positions.resize(zone.nb_som(), Objet_U::dimension);
  positions = zone.domaine().les_sommets();
  return positions;
}

int Champ_implementation_sommet_base::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{
  const Zone& zone = get_zone_geom();
  positions.resize(zone.nb_som(), Objet_U::dimension);
  positions = zone.domaine().les_sommets();
  zone.chercher_elements(positions,polys);
  return 1;
}

DoubleTab& Champ_implementation_sommet_base::valeur_aux_sommets_impl(DoubleTab& result) const
{
  const Champ_base& ch_base = le_champ();
  const DoubleTab&  values = ch_base.valeurs();
  const int size = values.dimension(0), N = values.line_size();

  assert((result.dimension(0) == size) || (result.dimension_tot(0) == size));
  assert(result.line_size() == N);
  for (int i = 0; i < size; i++)
    for (int n = 0; n < N; n++)
      result(i, n) = values(i, n);

  return result;
}

DoubleVect& Champ_implementation_sommet_base::valeur_aux_sommets_compo_impl(DoubleVect& result, int ncomp) const
{
  const Champ_base& ch_base       = le_champ();
  const DoubleTab&  values        = ch_base.valeurs();
  int            size          = values.dimension(0);

  assert(ncomp>=0);
  assert(ncomp < ch_base.nb_comp());
  assert(result.size() == size);
  assert(values.nb_dim() == 2);
  assert(values.dimension(1) == ch_base.nb_comp());

  for (int i=0; i<size; i++)
    result(i) = values(i, ncomp);

  return result;
}

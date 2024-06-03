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

#include <Champ_implementation_P0.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Fonc_Morceaux.h>
#include <Champ_Don_Fonc_txyz.h>
#include <Champ_Uniforme.h>
#include <Champ_base.h>
#include <Domaine.h>
#include <Domaine_VF.h>

DoubleVect& Champ_implementation_P0::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  const Champ_base& ch_base = le_champ();
  int nb_components = ch_base.nb_comp();
  const DoubleTab& values = ch_base.valeurs();

  assert(result.size() == nb_components);
  assert(poly >= 0);
  assert(poly < values.dimension_tot(0));
  assert(values.nb_dim() == 2);
  assert(values.dimension(1) == nb_components);

  for (int i = 0; i < nb_components; i++) result(i) = values(poly, i);

  return result;
}

double Champ_implementation_P0::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  const Champ_base& ch_base = le_champ();
  const DoubleTab& values = ch_base.valeurs();

  assert(ncomp >= 0);
  assert(ncomp < ch_base.nb_comp());
  assert(poly >= 0);
  assert(poly < values.dimension_tot(0));

  double result = 0;

  assert(values.line_size() == ch_base.nb_comp());
  result = values(poly, ncomp);

  return result;
}

DoubleTab& Champ_implementation_P0::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{
  const Champ_base& ch_base = le_champ();
  int nb_components = ch_base.nb_comp();
  const DoubleTab& values = ch_base.valeurs();
  int nb_polys = polys.size();

  if (nb_polys == 0)
    return result;

  // TODO : FIXME
  // For FT the resize should be done in its good position and not here ...
  if (result.nb_dim() == 1) result.resize(nb_polys, 1);

  assert(values.line_size() == nb_components);
  assert(result.line_size() == nb_components || nb_components == 1);

  for (int i = 0; i < nb_polys; i++)
    {
      int cell = polys(i);
      assert(cell < values.dimension_tot(0));

      for (int j = 0; j < result.line_size(); j++)
        if (cell != -1)
          result(i, j) = values.nb_dim() == 1 ? values(cell) : values(cell, (result.line_size() == nb_components) * j); // Some post-processed fields can have nb_dim() == 1
    }

  return result;
}

DoubleVect& Champ_implementation_P0::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const
{
  const Champ_base& ch_base = le_champ();
  const DoubleTab& values = ch_base.valeurs();

  int nb_polys = polys.size();

  assert(ncomp >= 0);
  assert(ncomp < ch_base.nb_comp());
  assert(result.size() == nb_polys);

  assert(values.line_size() == ch_base.nb_comp());
  for (int i = 0; i < nb_polys; i++)
    {
      int cell = polys(i);
      assert(cell < values.dimension_tot(0));
      if (cell != -1) result(i) = values(cell, ncomp);
      // result(i) = (cell == -1) ? 0 : values(cell,ncomp);
    }

  return result;
}

DoubleTab& Champ_implementation_P0::remplir_coord_noeuds(DoubleTab& positions) const
{
  const Domaine& domaine = get_domaine_geom();
  positions.resize(domaine.nb_elem(), Objet_U::dimension);
  domaine.calculer_centres_gravite(positions);
  return positions;
}

int Champ_implementation_P0::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{
  const Domaine& domaine = get_domaine_geom();
  int nb_elem = domaine.nb_elem();
  polys.resize(nb_elem);
  remplir_coord_noeuds(positions);
  for (int i = 0; i < nb_elem; i++) polys(i) = i;
  return 1;
}

DoubleTab& Champ_implementation_P0::valeur_aux_sommets_impl(DoubleTab& result) const
{
  const Champ_base& ch_base = le_champ();
  int nb_components = ch_base.nb_comp();
  const DoubleTab& values = ch_base.valeurs();

  const Domaine& domaine = get_domaine_geom();
  int nb_cells = domaine.nb_elem_tot();
  int nb_nodes = domaine.nb_som();
  int nb_nodes_per_cell = domaine.nb_som_elem();

  ArrOfInt count(nb_nodes);

  assert((result.dimension_tot(0) == nb_nodes) || (result.dimension(0) == nb_nodes));

  result = 0;
  count = 0;

  assert(result.line_size() == nb_components);
  assert(values.line_size() == nb_components);

  for (int i = 0; i < nb_cells; i++)
    for (int j = 0; j < nb_nodes_per_cell; j++)
      {
        int node = domaine.sommet_elem(i, j);
        if (node < nb_nodes)
          {
            count[node]++;
            if (values.nb_dim() == 1) // TODO : FIXME : champ porosite doit etre un DoubleTab
              for (int k = 0; k < nb_components; k++)
                result(node, k) += values(i);
            else
              for (int k = 0; k < nb_components; k++)
                result(node, k) += values(i, k);
          }
      }
  for (int i = 0; i < nb_nodes; i++)
    for (int j = 0; j < nb_components; j++)
      result(i, j) /= count[i];

  return result;
}

DoubleVect& Champ_implementation_P0::valeur_aux_sommets_compo_impl(DoubleVect& result, int ncomp) const
{
  const Champ_base& ch_base = le_champ();
  const DoubleTab& values = ch_base.valeurs();

  const Domaine& domaine = get_domaine_geom();
  int nb_cells = domaine.nb_elem_tot();
  int nb_nodes = domaine.nb_som();
  int nb_nodes_per_cell = domaine.nb_som_elem();

  ArrOfInt count(nb_nodes);
  // dvq ajout result=0;
  result = 0;
  assert(ncomp >= 0);
  assert(ncomp < ch_base.nb_comp());
  assert(result.size() == nb_nodes);
  assert(values.line_size() == ch_base.nb_comp());

  for (int i = 0; i < nb_cells; i++)
    for (int j = 0; j < nb_nodes_per_cell; j++)
      {
        int node = domaine.sommet_elem(i, j);
        if (node < nb_nodes)
          {
            count[node]++;
            result(node) += values(i, ncomp);
          }
      }
  for (int i = 0; i < nb_nodes; i++) result(i) /= count[i];

  return result;
}

int Champ_implementation_P0::imprime_P0(Sortie& os, int ncomp) const
{
  const Champ_base& cha = le_champ();
  const Domaine& le_dom = get_domaine_geom();
  int nb_elem_tot = le_dom.nb_elem_tot();
  const DoubleTab& val = cha.valeurs();
  int elem;
  // On recalcule les centres de gravite :
  DoubleTab pos_centre;
  le_dom.calculer_centres_gravite(pos_centre);
  os << nb_elem_tot << finl;
  for (elem = 0; elem < nb_elem_tot; elem++)
    {
      if (Objet_U::dimension == 3)
        os << pos_centre(elem, 0) << " " << pos_centre(elem, 1) << " " << pos_centre(elem, 2) << " ";
      if (Objet_U::dimension == 2)
        os << pos_centre(elem, 0) << " " << pos_centre(elem, 1) << " ";
      os << val(elem, ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_P0_implementation::imprime_P0 END >>>>>>>>>> " << finl;
  return 1;
}

int Champ_implementation_P0::affecter_(const Champ_base& ch)
{
  // if (le_champ().a_un_domaine_dis_base() && ch.a_un_domaine_dis_base() && le_champ().domaine_dis_base()==ch.domaine_dis_base())
  // Plus general en comparant le domaine:
  // Ajout de Champ_Uniforme_Morceaux/Champ_Fonc_Morceaux/Champ_Don_Fonc_txyz qui sont aux elements
  if (sub_type(Champ_Uniforme_Morceaux, ch) || sub_type(Champ_Fonc_Morceaux, ch) || sub_type(Champ_Don_Fonc_txyz, ch)
      || (le_champ().a_un_domaine_dis_base() && ch.a_un_domaine_dis_base() && le_champ().domaine_dis_base().domaine() == ch.domaine_dis_base().domaine()))
    {
      // Meme support donc on utilise une methode plus rapide pour affecter_
      // que la methode generale dans Champ_Fonc_base::affecter_ ou Champ_Inc_base::affecter_
      DoubleTab noeuds;
      IntVect polys;
      remplir_coord_noeuds_et_polys(noeuds, polys);
      ch.valeur_aux_elems(noeuds, polys, le_champ().valeurs());
      return 1;
    }
  else
    {
      if ((le_champ().domaine_dis_base().domaine().nb_elem() > 10000) && (!sub_type(Champ_Uniforme, ch)))
        Cerr << "Warning (if called each time step): computing field " << le_champ().le_nom() << " on domain " << le_champ().domaine_dis_base().domaine().le_nom() << " is not optimized... " << finl;
      return 0;
    }
}

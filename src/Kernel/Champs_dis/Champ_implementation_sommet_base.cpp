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
// File:        Champ_implementation_sommet_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_implementation_sommet_base.h>
#include <Domaine.h>
#include <Zone_VF.h>
#include <Champ_base.h>

DoubleVect& Champ_implementation_sommet_base::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  const Champ_base& ch_base       = le_champ();
  int            nb_components = ch_base.nb_comp();
  const DoubleTab&  values        = ch_base.valeurs();

  //  const Zone&      zone              = get_zone_geom();
  //  const IntTab&    cells             = zone.les_elems();
  //const int     nb_nodes_per_cell = zone.nb_som_elem();

  //  assert(poly>=0);
  //assert(poly < cells.dimension_tot(0));
  assert(result.size() == nb_components);
  assert(position.size() == Objet_U::dimension);

  // result = 0.;
  ArrOfDouble resu(nb_components);

  value_interpolation(position,poly,values,resu);
  for (int j=0; j<nb_components; j++)
    {
      result(j) = resu(j) ;
    }
  /*
    if (nb_components == 1) {
    result(0)+=resu(0);

    if (values.nb_dim() == 1) {
    for (int i=0; i<nb_nodes_per_cell; i++) {
    int node = cells(poly,i);
    result(0) += values(node) * form_function(position, poly, i);
    }
    }
    else if (values.nb_dim() == 2) {
    assert(values.dimension(1) == 1);
    for (int i=0; i<nb_nodes_per_cell; i++) {
    int node = cells(poly,i);
    result(0) += values(node,0) * form_function(position, poly, i);
    }
    }
    else {
    Cerr << "Error TRUST in Champ_implementation_sommet_base::valeur_a_elem()" << finl;
    Cerr << "The dimensions of the field are inconsistent" << finl;
    Process::exit();
    }

    }
    else {
    assert(values.nb_dim() == 2);
    assert(values.dimension(1) == nb_components);
    for (int j=0; j<nb_components; j++) {
    result(j) += resu(j) ;
    }

    }
  */
  return result;
}

double Champ_implementation_sommet_base::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  const Champ_base& ch_base       = le_champ();
  //  int            nb_components = ch_base.nb_comp();
  const DoubleTab&  values        = ch_base.valeurs();

  //  const Zone&      zone              = get_zone_geom();
  //const IntTab&    cells             = zone.les_elems();
  //const int     nb_nodes_per_cell = zone.nb_som_elem();

  assert(ncomp>=0);
  assert(ncomp<ch_base.nb_comp());

  assert(position.size() == Objet_U::dimension);

  double result = 0.;

  ArrOfDouble resu(1);

  value_interpolation(position,poly,values,resu,ncomp);

  result = resu(0) ;
  /*
    if (nb_components == 1) {
    if (values.nb_dim() == 1) {
    for (int i=0; i<nb_nodes_per_cell; i++) {
    int node = cells(poly,i);
    result += values(node) * form_function(position, poly, i);
    }
    }
    else if (values.nb_dim() == 2) {
    assert(values.dimension(1) == 1);
    for (int i=0; i<nb_nodes_per_cell; i++) {
    int node = cells(poly,i);
    result += values(node,0) * form_function(position, poly, i);
    }
    }
    else {
    Cerr << "Error TRUST in Champ_implementation_sommet_base::valeur_a_elem_compo()" << finl;
    Cerr << "The dimensions of the field are inconsistent" << finl;
    Process::exit();
    }
    }
    else {
    assert(values.nb_dim() == 2);
    assert(values.dimension(1) == nb_components);
    for (int i=0; i<nb_nodes_per_cell; i++) {
    int node = cells(poly,i);
    result += values(node,ncomp) * form_function(position, poly, i);
    }
    }
  */
  return result;
}

DoubleTab& Champ_implementation_sommet_base::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{
  const Champ_base& ch_base       = le_champ();
  int            nb_components = ch_base.nb_comp();
  const DoubleTab&  values        = ch_base.valeurs();

  //const Zone&      zone              = get_zone_geom();
  //  const IntTab&    cells             = zone.les_elems();
  //const int     nb_nodes_per_cell = zone.nb_som_elem();

  int nb_polys = polys.size();

  assert((result.dimension(0) == nb_polys)||(result.dimension_tot(0) == nb_polys));
  assert(positions.dimension(1) == Objet_U::dimension);

  ArrOfDouble position(Objet_U::dimension);

// result = 0.;


  ArrOfDouble resu(nb_components);
  for (int i=0; i<nb_polys; i++)
    {
      resu=0;
      for (int j=0; j<Objet_U::dimension; j++)
        {
          position[j] = positions(i,j);
        }
      int cell = polys(i);
      if (cell!=-1)
        {
          value_interpolation(position,cell,values,resu);
          if (result.nb_dim()==1)
            {
              result(i)= resu(0);
            }
          else
            {
              for (int k=0; k<nb_components; k++)
                {
                  result(i,k)=resu(k);
                }
            }
        }
    }

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

  ArrOfDouble position(Objet_U::dimension);
  ArrOfDouble resu(1);
  //result = 0.;
  for (int i=0; i<nb_polys; i++)
    {
      resu(0)=0;
      for (int j=0; j<Objet_U::dimension; j++)
        {
          position[j] = positions(i,j);
        }
      int cell = polys(i);

      if (cell != -1)
        {
          value_interpolation(position,cell,values,resu,ncomp);
          double value=resu(0);
          result(i)=value;
        }
    }
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
  const Champ_base& ch_base       = le_champ();
  int            nb_components = ch_base.nb_comp();
  const DoubleTab&  values        = ch_base.valeurs();
  int            size          = values.dimension(0);

  assert((result.dimension(0) == size)||(result.dimension_tot(0) == size));

  if (nb_components == 1)
    {
      if (values.nb_dim() == 1)
        {
          if (result.nb_dim() == 1)
            {
              result = values;
            }
          else if (result.nb_dim() == 2)
            {
              assert(result.dimension(1) == 1);
              for (int i=0; i<size; i++)
                {
                  result(i,0) = values(i);
                }
            }
          else
            {
              Cerr << "Error TRUST in Champ_implementation_sommet_base::valeur_aux_sommets_impl()" << finl;
              Cerr << "The DoubleTab result has more than 2 entries" << finl;
              Process::exit();
            }
        }
      else if (values.nb_dim() == 2)
        {
          assert(values.dimension(1) == 1);
          if (result.nb_dim() == 1)
            {
              for (int i=0; i<size; i++)
                {
                  result(i) = values(i,0);
                }
            }
          else if (result.nb_dim() == 2)
            {
              assert(result.dimension(1) == 1);
              result = values;
            }
          else
            {
              Cerr << "Error TRUST in Champ_implementation_sommet_base::valeur_aux_sommets_impl()" << finl;
              Cerr << "The DoubleTab result has more than 2 entries" << finl;
              Process::exit();
            }
        }
      else
        {
          Cerr << "Error TRUST in Champ_implementation_sommet_base::valeur_aux_sommets_impl()" << finl;
          Cerr << "The dimensions of the field are inconsistent" << finl;
          Process::exit();
        }
    }
  else
    {
      assert(result.nb_dim() == 2);
      assert(values.nb_dim() == 2);
      assert(result.dimension(1) == values.dimension(1));
      result = values;
    }

  return result;
}

DoubleVect& Champ_implementation_sommet_base::valeur_aux_sommets_compo_impl(DoubleVect& result, int ncomp) const
{
  const Champ_base& ch_base       = le_champ();
  int            nb_components = ch_base.nb_comp();
  const DoubleTab&  values        = ch_base.valeurs();
  int            size          = values.dimension(0);

  assert(ncomp>=0);
  assert(ncomp<nb_components);
  assert(result.size() == size);

  if (nb_components == 1)
    {
      if (values.nb_dim() == 1)
        {
          for (int i=0; i<size; i++)
            {
              result(i) = values(i);
            }
        }
      else if (values.nb_dim() == 2)
        {
          assert(values.dimension(1) == 1);
          for (int i=0; i<size; i++)
            {
              result(i) = values(i,0);
            }
        }
      else
        {
          Cerr << "Error TRUST in Champ_implementation_sommet_base::valeur_aux_sommets_compo_impl()" << finl;
          Cerr << "The dimensions of the field are inconsistent" << finl;
          Process::exit();
        }
    }
  else
    {
      assert(values.nb_dim() == 2);
      assert(values.dimension(1) == nb_components);
      for (int i=0; i<size; i++)
        {
          result(i) = values(i,ncomp);
        }
    }

  return result;
}

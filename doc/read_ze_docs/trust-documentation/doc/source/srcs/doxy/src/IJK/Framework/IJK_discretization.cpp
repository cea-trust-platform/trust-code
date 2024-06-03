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

#include <IJK_discretization.h>
#include <Param.h>
#include <Domaine_VF.h>
#include <Probleme_base.h>
#include <Interprete_bloc.h>

Implemente_instanciable(IJK_discretization, "IJK_discretization", Objet_U);

Sortie& IJK_discretization::printOn(Sortie& s) const
{
  return s;
}

// The object must be named like this in the interpretor. Operators will implicitely search for this name:
const char * IJK_discretization::get_conventional_name()
{
  return "vdf_to_ijk";
}

Entree& IJK_discretization::readOn(Entree& is)
{
  if (le_nom() != get_conventional_name())
    {
      Cerr << "error in IJK_discretization: the object MUST be named " << get_conventional_name()
           << " (sorry, this is a hack)" << finl;
      exit();
    }
  Param param(que_suis_je());
  Nom vdf_problem;
  ArrOfInt splitting;
  ArrOfInt direction_mapping(3);
  ArrOfInt perio_flags(3);
  perio_flags = 0;
  direction_mapping[0] = 0;
  direction_mapping[1] = 1;
  direction_mapping[2] = 2;
  param.ajouter("vdf_problem", &vdf_problem, Param::REQUIRED);
  param.ajouter("splitting", &splitting);
  param.ajouter("x_maps_to", &direction_mapping[0]);
  param.dictionnaire("i",0);
  param.dictionnaire("j",1);
  param.dictionnaire("k",2);
  param.ajouter("y_maps_to", &direction_mapping[1]);
  param.dictionnaire("i",0);
  param.dictionnaire("j",1);
  param.dictionnaire("k",2);
  param.ajouter("z_maps_to", &direction_mapping[2]);
  param.dictionnaire("i",0);
  param.dictionnaire("j",1);
  param.dictionnaire("k",2);

  param.ajouter_flag("perio_i", &perio_flags[0]);
  param.ajouter_flag("perio_j", &perio_flags[1]);
  param.ajouter_flag("perio_k", &perio_flags[2]);

  param.lire_avec_accolades(is);

  // Check parameters
  if (splitting.size_array() != 3)
    {
      Cerr << "Error in IJK_discretization::readOn: splitting must be an array of 3 (ex splitting 3 8 8 8)" << finl;
      exit();
    }

  const Probleme_base& pb = ref_cast(Probleme_base, Interprete_bloc::objet_global(vdf_problem));
  const Domaine_VF& domaine_vdf = ref_cast(Domaine_VF, pb.domaine_dis().valeur());

  IJK_Grid_Geometry grid_geom;
  grid_geom.initialize_from_unstructured(domaine_vdf.domaine(),
                                         direction_mapping[0], direction_mapping[1], direction_mapping[2],
                                         perio_flags[0], perio_flags[1], perio_flags[2]);

  splitting_.initialize(grid_geom, splitting[0], splitting[1], splitting[2]);

  vdf_to_ijk_i_.initialize(domaine_vdf, splitting_, IJK_Splitting::FACES_I, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_j_.initialize(domaine_vdf, splitting_, IJK_Splitting::FACES_J, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_k_.initialize(domaine_vdf, splitting_, IJK_Splitting::FACES_K, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_elem_.initialize(domaine_vdf, splitting_, IJK_Splitting::ELEM, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_nodes_.initialize(domaine_vdf, splitting_, IJK_Splitting::NODES, direction_mapping[0], direction_mapping[1], direction_mapping[2]);

  return is;
}

const IJK_Splitting& IJK_discretization::get_IJK_splitting() const
{
  return splitting_;
}

const VDF_to_IJK& IJK_discretization::get_vdf_to_ijk(IJK_Splitting::Localisation loc) const
{
  switch(loc)
    {
    case IJK_Splitting::FACES_I:
      return vdf_to_ijk_i_;
    case IJK_Splitting::FACES_J:
      return vdf_to_ijk_j_;
    case IJK_Splitting::FACES_K:
      return vdf_to_ijk_k_;
    case IJK_Splitting::ELEM:
      return vdf_to_ijk_elem_;
    case IJK_Splitting::NODES:
      return vdf_to_ijk_nodes_;
    default:
      Cerr << "Error in IJK_discretization::get_vdf_to_ijk: unknown loc" << finl;
      exit();
    }
  return vdf_to_ijk_i_;
}

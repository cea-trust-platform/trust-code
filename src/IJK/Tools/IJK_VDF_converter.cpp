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

#include <IJK_VDF_converter.h>
#include <Param.h>
#include <Domaine_VF.h>
#include <Probleme_base.h>
#include <Interprete_bloc.h>

Implemente_instanciable(IJK_VDF_converter, "IJK_VDF_converter", Objet_U);

Sortie& IJK_VDF_converter::printOn(Sortie& s) const
{
  return s;
}

// The object must be named like this in the interpretor. Operators will implicitely search for this name:
const char * IJK_VDF_converter::get_conventional_name()
{
  return "vdf_to_ijk";
}

Entree& IJK_VDF_converter::readOn(Entree& is)
{
  if (le_nom() != get_conventional_name())
    {
      Cerr << "error in IJK_VDF_converter: the object MUST be named " << get_conventional_name()
           << " (sorry, this is a hack)" << finl;
      exit();
    }
  Param param(que_suis_je());
  Nom vdf_problem;
  ArrOfInt splitting;
  ArrOfInt direction_mapping(3);
  bool perio_flag_x = false;
  bool perio_flag_y = false;
  bool perio_flag_z = false;
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

  param.ajouter_flag("perio_i", &perio_flag_x);
  param.ajouter_flag("perio_j", &perio_flag_y);
  param.ajouter_flag("perio_k", &perio_flag_z);

  param.lire_avec_accolades(is);

  // Check parameters
  if (splitting.size_array() != 3)
    {
      Cerr << "Error in IJK_VDF_converter::readOn: splitting must be an array of 3 (ex splitting 3 8 8 8)" << finl;
      exit();
    }

  const Probleme_base& pb = ref_cast(Probleme_base, Interprete_bloc::objet_global(vdf_problem));
  const Domaine_VF& domaine_vdf = ref_cast(Domaine_VF, pb.domaine_dis());

  Domaine_IJK grid_geom;
  grid_geom.initialize_from_unstructured(domaine_vdf.domaine(),
                                         direction_mapping[0], direction_mapping[1], direction_mapping[2],
                                         perio_flag_x, perio_flag_y, perio_flag_z);

  domaine_ijk_.initialize_splitting(grid_geom, splitting[0], splitting[1], splitting[2]);

  vdf_to_ijk_i_.initialize(domaine_vdf, domaine_ijk_, Domaine_IJK::FACES_I, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_j_.initialize(domaine_vdf, domaine_ijk_, Domaine_IJK::FACES_J, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_k_.initialize(domaine_vdf, domaine_ijk_, Domaine_IJK::FACES_K, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_elem_.initialize(domaine_vdf, domaine_ijk_, Domaine_IJK::ELEM, direction_mapping[0], direction_mapping[1], direction_mapping[2]);
  vdf_to_ijk_nodes_.initialize(domaine_vdf, domaine_ijk_, Domaine_IJK::NODES, direction_mapping[0], direction_mapping[1], direction_mapping[2]);

  return is;
}


const VDF_to_IJK& IJK_VDF_converter::get_vdf_to_ijk(Domaine_IJK::Localisation loc) const
{
  switch(loc)
    {
    case Domaine_IJK::FACES_I:
      return vdf_to_ijk_i_;
    case Domaine_IJK::FACES_J:
      return vdf_to_ijk_j_;
    case Domaine_IJK::FACES_K:
      return vdf_to_ijk_k_;
    case Domaine_IJK::ELEM:
      return vdf_to_ijk_elem_;
    case Domaine_IJK::NODES:
      return vdf_to_ijk_nodes_;
    default:
      Cerr << "Error in IJK_VDF_converter::get_vdf_to_ijk: unknown loc" << finl;
      exit();
    }
  return vdf_to_ijk_i_;
}

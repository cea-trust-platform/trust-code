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

#include <IJK_Grid_Geometry.h>
#include <Domaine.h>
#include <Domaine.h>
#include <communications.h>
#include <IJK_Field.h>
#include <Param.h>
#include <EFichier.h>
#include <Hexaedre.h>

Implemente_instanciable_sans_constructeur(IJK_Grid_Geometry, "IJK_Grid_Geometry", Objet_U);
// XD IJK_Grid_Geometry domaine IJK_Grid_Geometry -1 Object to define the grid that will represent the domain of the simulation in IJK discretization

IJK_Grid_Geometry::IJK_Grid_Geometry()
{
  uniform_[0] = uniform_[1] = uniform_[2] = false;
  periodic_[0] = periodic_[1] = periodic_[2] = false;
  delta_xyz_.dimensionner(3);
  node_coordinates_xyz_.dimensionner(3);
}

Sortie& IJK_Grid_Geometry::printOn(Sortie& os) const
{
  return os;
}

Entree& IJK_Grid_Geometry::readOn(Entree& is)
{
  Cerr << "Reading IJK_Grid_Geometry parameters (" << le_nom() << ")" << finl;
  Param param(que_suis_je());
  ArrOfDouble origin(3); // defaults to [0,0,0]
  ArrOfInt perio_flags(3); // defaults to not periodic
  ArrOfInt ndir(3); // Number of elements per direction
  ndir = -1; // default means "unspecified"
  ArrOfDouble constant_deltadir(3); // Constant element size in each direction
  Noms file_coord(3);
  constant_deltadir = -1.; // default means "unspecified"
  param.ajouter_flag("perio_i", &perio_flags[0]); // XD_ADD_P rien flag to specify the border along the I direction is periodic
  param.ajouter_flag("perio_j", &perio_flags[1]); // XD_ADD_P rien flag to specify the border along the J direction is periodic
  param.ajouter_flag("perio_k", &perio_flags[2]); // XD_ADD_P rien flag to specify the border along the K direction is periodic
  param.ajouter("nbelem_i", &ndir[0]); // XD_ADD_P entier the number of elements of the grid in the I direction
  param.ajouter("nbelem_j", &ndir[1]); // XD_ADD_P entier the number of elements of the grid in the J direction
  param.ajouter("nbelem_k", &ndir[2]); // XD_ADD_P entier the number of elements of the grid in the K direction
  param.ajouter("uniform_domain_size_i", &constant_deltadir[0]); // XD_ADD_P floattant the size of the elements along the I direction
  param.ajouter("uniform_domain_size_j", &constant_deltadir[1]); // XD_ADD_P floattant the size of the elements along the J direction
  param.ajouter("uniform_domain_size_k", &constant_deltadir[2]); // XD_ADD_P floattant the size of the elements along the K direction
  param.ajouter("file_coord_i", &file_coord[0]);
  param.ajouter("file_coord_j", &file_coord[1]);
  param.ajouter("file_coord_k", &file_coord[2]);
  param.ajouter("origin_i", &origin[0]); // XD_ADD_P floattant I-coordinate of the origin of the grid
  param.ajouter("origin_j", &origin[1]); // XD_ADD_P floattant J-coordinate of the origin of the grid
  param.ajouter("origin_k", &origin[2]); // XD_ADD_P floattant K-coordinate of the origin of the grid
  param.lire_avec_accolades(is);

  VECT(ArrOfDouble) delta_dir(3);
  for (int i = 0; i < 3; i++)
    {
      if (file_coord[i] != "??")
        {
          if (Process::je_suis_maitre())
            {
              Cerr << "Reading coordinates for direction " << i << " in file " << file_coord[i] << finl;
              EFichier EFcoord(file_coord[i]);

              double previous_x = 0.;
              int n = 0;
              while(1)
                {
                  double next_x;
                  EFcoord >> next_x;
                  if (EFcoord.eof())
                    break;
                  if (n == 0)
                    origin[i] = next_x;
                  else
                    delta_dir[i].append_array(next_x - previous_x);
                  n++;
                  previous_x = next_x;
                }
              if (delta_dir[i].size_array() < 1)
                {
                  Cerr << "Error in IJK_Grid_Geometry::readOn: input file contains less than 2 values" << finl;
                }
            }
          envoyer_broadcast(delta_dir[i], 0);
        }
      else if (ndir[i] != -1)
        {
          if (ndir[i] < 1 || constant_deltadir[i] <= 0.)
            {
              Cerr << "Error in IJK_Grid_Geometry::readOn: wrong value of nbelem or uniform_domain_size for direction " << i
                   << "\n : nbelem=" << ndir[i] << " domain_size=" << constant_deltadir[i] << finl;
              exit();
            }
          delta_dir[i].resize_array(ndir[i]);
          delta_dir[i] = constant_deltadir[i] / ndir[i];
        }
      else
        {
          Cerr << "Error in IJK_Grid_Geometry::readOn: you must provide (nbelem_xxx and uniform_delta_xxx)\n"
               << "   or variable_delta_xxx_file for each direction i, j and k" << finl;
          exit();
        }
      double x = 0;
      for (int j = 0; j < delta_dir[i].size_array(); j++)
        {
          if (delta_dir[i][j] <= 0.)
            {
              Cerr << "Error in IJK_Grid_Geometry::readOn: size of element " << j << "  in direction " << i << " is not positive" << finl;
              exit();
            }
          x += delta_dir[i][j];
        }
      Cerr << " Direction " << i << " has " << delta_dir[i].size_array() << " elements. Total domain size = " << x << finl;
    }

  initialize_origin_deltas(origin[0], origin[1], origin[2],
                           delta_dir[0], delta_dir[1], delta_dir[2],
                           perio_flags[0], perio_flags[1], perio_flags[2]);
  return is;
}

static void retirer_doublons(ArrOfDouble& tab, double epsilon)
{
  const int n = tab.size_array();
  if (n == 0)
    return;
  double last_value = tab[0];
  int dest = 1;
  for (int i = 1; i < n; i++)
    {
      double x = tab[i];
      if (x > last_value + epsilon)
        {
          tab[dest++] = x;
          last_value = x;
        }
    }
  tab.resize_array(dest);
}

static void find_unique_coord(const DoubleTab& src, int column, ArrOfDouble& result)
{
  const int n = src.dimension(0);
  ArrOfDouble tmp(n);
  int i;
  for (i = 0; i < n; i++)
    tmp[i] = src(i, column);
  tmp.ordonne_array();
  retirer_doublons(tmp, Objet_U::precision_geom);


  if (Process::me() != 0)
    {
      envoyer(tmp, 0, 53);
    }
  else
    {
      result = tmp;
      int np = Process::nproc();
      for (i = 1; i < np; i++)
        {
          recevoir(tmp, i, 53 /* canal */);
          int m1Loc = tmp.size_array();
          int m2Loc = result.size_array();
          result.resize_array(m2Loc+m1Loc);
          result.inject_array(tmp, m1Loc /* nbelem */, m2Loc /* dest index */, 0 /* src index */);
        }
      result.ordonne_array();
      retirer_doublons(result, Objet_U::precision_geom);
    }

  envoyer_broadcast(result, 0);
}

// Extracts the mesh origin, dimensions and cell sizes from a distributed VDF mesh.
//
void IJK_Grid_Geometry::initialize_from_unstructured(const Domaine& domaine,
                                                     int direction_for_x,
                                                     int direction_for_y,
                                                     int direction_for_z,
                                                     bool perio_x, bool perio_y, bool perio_z)
{
  if (!sub_type(Hexaedre, domaine.type_elem().valeur()))
    {
      Cerr << "Error in IJK_Grid_Geometry::initialize_from_unstructured:\n"
           << " the provided domaine does not have Hexaedre element type" << finl;
      exit();
    }
  periodic_[0] = perio_x;
  periodic_[1] = perio_y;
  periodic_[2] = perio_z;
  // Find all coordinates in the unstructured mesh
  // swap directions
  const DoubleTab& coord_som = domaine.les_sommets();
  Cout << "IJK_Grid_Geometry::initialize_from_unstructured maps x->" <<  direction_for_x
       << " y->" << direction_for_y << " z->" << direction_for_z << finl;

  find_unique_coord(coord_som, 0 /* coordonnees y */, node_coordinates_xyz_[direction_for_x]);
  find_unique_coord(coord_som, 1 /* coordonnees y */, node_coordinates_xyz_[direction_for_y]);
  find_unique_coord(coord_som, 2 /* coordonnees z (swap y et z) */, node_coordinates_xyz_[direction_for_z]);
  const double eps = Objet_U::precision_geom;

  for (int dir = 0; dir < 3; dir++)
    {
      const ArrOfDouble& coord = node_coordinates_xyz_[dir];
      ArrOfDouble& delta = delta_xyz_[dir];
      const int nelem = coord.size_array() - 1;
      Cout << " Direction " << dir << " has " << nelem << " elements. coord[0]="
           << coord[0] << " coord[" << nelem << "]=" << coord[nelem];
      delta.resize_array(nelem);
      double mindelta = 1e30;
      double maxdelta = 0.;
      for (int i = 0; i < nelem; i++)
        {
          const double d = coord[i+1] - coord[i];
          delta[i] = d;
          mindelta = std::min(d, mindelta);
          maxdelta = std::max(d, maxdelta);
        }
      if (maxdelta - mindelta < eps)
        {
          // The mesh is uniform: recompute a constant delta with a better accuracy:
          double d = (coord[nelem] - coord[0]) / (double) nelem;
          delta = d; // affect the exact same value to all cells
          Cout << " Uniform, delta=" << d << finl;
          uniform_[dir] = true;
        }
      else
        {
          Cout << "IJK_Grid_Geometry::initialize_from_unstructured direction " << dir
               << " Not uniform, min delta=" << mindelta
               << " max delta=" << maxdelta << finl;
          uniform_[dir] = false;
        }
    }
}

void IJK_Grid_Geometry::initialize_origin_deltas(double x0, double y0, double z0,
                                                 const ArrOfDouble& delta_x, const ArrOfDouble& delta_y, const ArrOfDouble& delta_z,
                                                 bool perio_x, bool perio_y, bool perio_z)
{
  periodic_[0] = perio_x;
  periodic_[1] = perio_y;
  periodic_[2] = perio_z;

  const double eps = Objet_U::precision_geom;

  delta_xyz_[0] = delta_x;
  delta_xyz_[1] = delta_y;
  delta_xyz_[2] = delta_z;
  for (int dir = 0; dir < 3; dir++)
    {
      int n = delta_xyz_[dir].size_array();
      node_coordinates_xyz_[dir].resize_array(n+1);
      node_coordinates_xyz_[dir][0] = (dir==0) ? x0 : ((dir==1)?y0:z0);
      double mindelta = 1e30;
      double maxdelta = 0.;
      for (int i = 0; i < n; i++)
        {
          const double d = delta_xyz_[dir][i];
          node_coordinates_xyz_[dir][i+1] = node_coordinates_xyz_[dir][i] + d;
          mindelta = std::min(d, mindelta);
          maxdelta = std::max(d, maxdelta);
        }
      if (maxdelta - mindelta < eps)
        {
          uniform_[dir] = true;
        }
      else
        {
          uniform_[dir] = false;
        }
    }
}



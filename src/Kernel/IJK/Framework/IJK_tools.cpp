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

#include <IJK_tools.h>
#include <Parser.h>
#include <IJK_ptr.h>
#include <EChaine.h>
#include <SChaine.h>
#include <Interprete_bloc.h>
#include <stat_counters.h>

Probleme_base& creer_domaine_ijk(const Domaine_IJK& domain, const Nom& domain_name)
{
  // On va construire une partie de jdd a faire interpreter:
  const double x0 = domain.get_origin(DIRECTION_I);
  const double y0 = domain.get_origin(DIRECTION_J);
  const double z0 = domain.get_origin(DIRECTION_K);
  const double lx = domain.get_domain_length(DIRECTION_I);
  const double ly = domain.get_domain_length(DIRECTION_J);
  const double lz = domain.get_domain_length(DIRECTION_K);
  const int nslicek = domain.get_nprocessor_per_direction(DIRECTION_K);
  const int nslicej = domain.get_nprocessor_per_direction(DIRECTION_J);
  const int nslicei = domain.get_nprocessor_per_direction(DIRECTION_I);
  const int ni = domain.get_nb_elem_tot(DIRECTION_I) + 1; // number of nodes
  const int nj = domain.get_nb_elem_tot(DIRECTION_J) + 1;
  const int nk = domain.get_nb_elem_tot(DIRECTION_K) + 1;

  char fonction_coord_x[300];
  snprintf(fonction_coord_x, 300, "%.16g+x*%.16g", x0, lx);
  char fonction_coord_y[300];
  snprintf(fonction_coord_y, 300, "%.16g+y*%.16g", y0, ly);
  char fonction_coord_z[300];
  snprintf(fonction_coord_z, 300, "%.16g+z*%.16g", z0, lz);

  SChaine instructions;
  instructions << "Dimension 3 " << finl;
  instructions << "Domaine " << domain_name << finl;
  instructions << "MaillerParallel" << finl;
  instructions << "{" << finl;
  instructions << "  domain " << domain_name << finl;
  instructions << "  nb_nodes 3 " << ni << " " << nj << " " << nk << finl;
  instructions << "  splitting 3 " << nslicei << " " << nslicej << " " << nslicek << finl;
  instructions << "  ghost_thickness 1" << finl;
  instructions << "  function_coord_x " << fonction_coord_x << finl;
  instructions << "  function_coord_y " << fonction_coord_y << finl;
  instructions << "  function_coord_z " << fonction_coord_z << finl;
  instructions << "  boundary_xmin xmin" << finl;
  instructions << "  boundary_xmax xmax" << finl;
  instructions << "  boundary_ymin ymin" << finl;
  instructions << "  boundary_ymax ymax" << finl;
  instructions << "  boundary_zmin zmin" << finl;
  instructions << "  boundary_zmax zmax" << finl;
  const int np = Process::nproc();
  instructions << "  mapping " << finl;

  IntTab map(np, 3);
  map = -1;
  for (int k = 0; k < nslicek; k++)
    for (int j = 0; j < nslicej; j++)
      for (int i = 0; i < nslicei; i++)
        {
          int p = domain.get_processor_by_ijk(i, j, k);
          map(p, 0) = i;
          map(p, 1) = j;
          map(p, 2) = k;
        }

  if (min_array(map) < 0)
    {
      Cerr << "Error in creer_domaine_vdf: there are unused processors in the ijk splitting" << finl;
    }
  Nom pb_name = Nom("pb_") + domain_name;
  instructions << map << finl;
  instructions << "}" << finl;
  instructions << "Probleme_FT_Disc_gen " << pb_name << finl;
  instructions << "Associer " << pb_name << " " << domain_name << finl;
  instructions << "VDF dis" << domain_name << finl;
  instructions << "Schema_Euler_explicite sch" << domain_name << " Lire sch" << domain_name << " { nb_pas_dt_max 1 }" << finl;
  instructions << "Associer " << pb_name << " sch" << domain_name << finl;
  instructions << "Discretiser " << pb_name << " dis" << domain_name << finl;
  Cerr << "Interpretation de la chaine suivante:" << finl << instructions.get_str();

  EChaine is(instructions.get_str());
  Interprete_bloc::interprete_courant().interpreter_bloc(is, Interprete_bloc::BLOC_EOF, 0 /* flag verifie sans interpreter */);

  // Il faut construire une structure de donnees du Domaine_VF qui n'est pas construite par defaut:
  Probleme_base& pb = ref_cast(Probleme_base, Interprete_bloc::objet_global(pb_name));
  Domaine& domaine = pb.domaine_dis().domaine();
  domaine.construire_elem_virt_pe_num();

  return pb;
}

// Interpolate the "field" at the requested "coordinates" (array with 3 columns), and stores into "result"
static void ijk_interpolate_implementation(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result, int skip_unknown_points, double value_for_bad_points)
{
  const int ghost = field.ghost();
  const int ni = field.ni();
  const int nj = field.nj();
  const int nk = field.nk();

  const Domaine_IJK& domain = field.get_domain();
  const double dx = domain.get_constant_delta(DIRECTION_I);
  const double dy = domain.get_constant_delta(DIRECTION_J);
  const double dz = domain.get_constant_delta(DIRECTION_K);
  const Domaine_IJK::Localisation loc = field.get_localisation();
  // L'origine est sur un noeud. Donc que la premiere face en I est sur get_origin(DIRECTION_I)
  double origin_x = domain.get_origin(DIRECTION_I) + ((loc == Domaine_IJK::FACES_J || loc == Domaine_IJK::FACES_K || loc == Domaine_IJK::ELEM) ? (dx * 0.5) : 0.);
  double origin_y = domain.get_origin(DIRECTION_J) + ((loc == Domaine_IJK::FACES_K || loc == Domaine_IJK::FACES_I || loc == Domaine_IJK::ELEM) ? (dy * 0.5) : 0.);
  double origin_z = domain.get_origin(DIRECTION_K) + ((loc == Domaine_IJK::FACES_I || loc == Domaine_IJK::FACES_J || loc == Domaine_IJK::ELEM) ? (dz * 0.5) : 0.);
  const int nb_coords = coordinates.dimension(0);
  result.resize_array(nb_coords);
  for (int idx = 0; idx < nb_coords; idx++)
    {
      const double x = coordinates(idx, 0);
      const double y = coordinates(idx, 1);
      const double z = coordinates(idx, 2);
      const double x2 = (x - origin_x) / dx;
      const double y2 = (y - origin_y) / dy;
      const double z2 = (z - origin_z) / dz;

      // Coordonnes barycentriques du points dans la cellule :
      const double xfact = x2 - floor(x2);
      const double yfact = y2 - floor(y2);
      const double zfact = z2 - floor(z2);

      // Determining the local index of the element in the IJK domain.
      const int index_i = domain.get_i_along_dir_perio(DIRECTION_I, x, Domaine_IJK::ELEM);
      const int index_j = domain.get_i_along_dir_perio(DIRECTION_J, y, Domaine_IJK::ELEM);
      const int index_k = domain.get_i_along_dir_perio(DIRECTION_K, z, Domaine_IJK::ELEM);
      // is point in the domain ? (ghost cells ok...)
      const bool ok = (index_i >= - ghost && index_i < ni + ghost - 1) && (index_j >= - ghost && index_j < nj + ghost - 1) && (index_k >= - ghost && index_k < nk + ghost - 1);
      if (!ok)
        {
          if (skip_unknown_points) // Should not happen anymore
            {
              result[idx] = value_for_bad_points;
              continue; // go to next point
            }
          // else
          //   {
          // Error!
          Cerr << "Error in ijk_interpolate_implementation: request interpolation of point " << x << " " << y << " " << z << " which is outside of the domain on processor " << Process::me()
               << finl;
          Process::exit();
          // }
        }

      const double r = (((1. - xfact) * field(index_i, index_j, index_k) + xfact * field(index_i + 1, index_j, index_k)) * (1. - yfact)
                        + ((1. - xfact) * field(index_i, index_j + 1, index_k) + xfact * field(index_i + 1, index_j + 1, index_k)) * (yfact)) * (1. - zfact)
                       + (((1. - xfact) * field(index_i, index_j, index_k + 1) + xfact * field(index_i + 1, index_j, index_k + 1)) * (1. - yfact)
                          + ((1. - xfact) * field(index_i, index_j + 1, index_k + 1) + xfact * field(index_i + 1, index_j + 1, index_k + 1)) * (yfact)) * (zfact);
      result[idx] = r;
    }
}

void ijk_interpolate_skip_unknown_points(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result, const double value_for_bad_points)
{
  ijk_interpolate_implementation(field, coordinates, result, 1 /* yes:skip unknown points */, value_for_bad_points);
}

void ijk_interpolate(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result)
{
  ijk_interpolate_implementation(field, coordinates, result, 0 /* skip unknown points=no */, 0.);
}

// Interpolate the "field" at the requested coordinate and returns the result
static double ijk_interpolate_one_value(const IJK_Field_double& field, const Vecteur3& coordinates, int skip_unknown_points, double value_for_bad_points)
{
  const int ghost = field.ghost();
  const int ni = field.ni();
  const int nj = field.nj();
  const int nk = field.nk();

  const Domaine_IJK& domain = field.get_domain();
  static const double dx = domain.get_constant_delta(DIRECTION_I);
  static const double dy = domain.get_constant_delta(DIRECTION_J);
  static const double dz = domain.get_constant_delta(DIRECTION_K);
  const Domaine_IJK::Localisation loc = field.get_localisation();
  // L'origine est sur un noeud. Donc que la premiere face en I est sur get_origin(DIRECTION_I)
  double origin_x = domain.get_origin(DIRECTION_I) + ((loc == Domaine_IJK::FACES_J || loc == Domaine_IJK::FACES_K || loc == Domaine_IJK::ELEM) ? (dx * 0.5) : 0.);
  double origin_y = domain.get_origin(DIRECTION_J) + ((loc == Domaine_IJK::FACES_K || loc == Domaine_IJK::FACES_I || loc == Domaine_IJK::ELEM) ? (dy * 0.5) : 0.);
  double origin_z = domain.get_origin(DIRECTION_K) + ((loc == Domaine_IJK::FACES_I || loc == Domaine_IJK::FACES_J || loc == Domaine_IJK::ELEM) ? (dz * 0.5) : 0.);

  const double x = coordinates[0];
  const double y = coordinates[1];
  const double z = coordinates[2];
  const double x2 = (x - origin_x) / dx;
  const double y2 = (y - origin_y) / dy;
  const double z2 = (z - origin_z) / dz;

  // Coordonnes barycentriques du points dans la cellule :
  const double xfact = x2 - floor(x2);
  const double yfact = y2 - floor(y2);
  const double zfact = z2 - floor(z2);

  // Determining the local index of the element in the IJK domain.
  const int index_i = domain.get_i_along_dir_perio(DIRECTION_I, x, Domaine_IJK::ELEM);
  const int index_j = domain.get_i_along_dir_perio(DIRECTION_J, y, Domaine_IJK::ELEM);
  const int index_k = domain.get_i_along_dir_perio(DIRECTION_K, z, Domaine_IJK::ELEM);

  // is point in the domain ? (ghost cells ok...)
  bool ok = (index_i >= -ghost && index_i < ni + ghost - 1) && (index_j >= -ghost && index_j < nj + ghost - 1) && (index_k >= -ghost && index_k < nk + ghost - 1);
  if (!ok) // Should not happen
    {
      if (skip_unknown_points)
        {
          return value_for_bad_points;
        }
      else
        {
          // Error!
          Cerr << "Error in ijk_interpolate_implementation: request interpolation of point " << x << " " << y << " " << z << " which is outside of the domain on processor " << Process::me()
               << finl;
          Process::exit();
        }
    }

  double r = (((1. - xfact) * field(index_i, index_j, index_k) + xfact * field(index_i + 1, index_j, index_k)) * (1. - yfact)
              + ((1. - xfact) * field(index_i, index_j + 1, index_k) + xfact * field(index_i + 1, index_j + 1, index_k)) * (yfact)) * (1. - zfact)
             + (((1. - xfact) * field(index_i, index_j, index_k + 1) + xfact * field(index_i + 1, index_j, index_k + 1)) * (1. - yfact)
                + ((1. - xfact) * field(index_i, index_j + 1, index_k + 1) + xfact * field(index_i + 1, index_j + 1, index_k + 1)) * (yfact)) * (zfact);
  return r;
}

double ijk_interpolate_skip_unknown_points(const IJK_Field_double& field, const Vecteur3& coordinates, const double value_for_bad_points)
{
  return ijk_interpolate_one_value(field, coordinates, 1 /* yes:skip unknown points */, value_for_bad_points);
}

double ijk_interpolate(const IJK_Field_double& field, const Vecteur3& coordinates)
{
  return ijk_interpolate_one_value(field, coordinates, 0 /* skip unknown points=no */, 0.);
}

// build local coordinates of discretisation nodes for the given field
// (used in set_field_data() )
void build_local_coords(const IJK_Field_double& f, ArrOfDouble& coord_i, ArrOfDouble& coord_j, ArrOfDouble& coord_k)
{
  const Domaine_IJK& domain = f.get_domain();
  const int i_offset = domain.get_offset_local(DIRECTION_I);
  const int j_offset = domain.get_offset_local(DIRECTION_J);
  const int k_offset = domain.get_offset_local(DIRECTION_K);

  const ArrOfDouble& nodes_i = domain.get_node_coordinates(0);
  const ArrOfDouble& nodes_j = domain.get_node_coordinates(1);
  const ArrOfDouble& nodes_k = domain.get_node_coordinates(2);
  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();
  coord_i.resize_array(ni);
  coord_j.resize_array(nj);
  coord_k.resize_array(nk);

  if (f.get_localisation() == Domaine_IJK::NODES || f.get_localisation() == Domaine_IJK::FACES_I)
    {
      for (int i = 0; i < ni; i++)
        coord_i[i] = nodes_i[i + i_offset];
    }
  else
    {
      for (int i = 0; i < ni; i++)
        coord_i[i] = (nodes_i[i + i_offset] + nodes_i[i + i_offset + 1]) * 0.5;
    }
  if (f.get_localisation() == Domaine_IJK::NODES || f.get_localisation() == Domaine_IJK::FACES_J)
    {
      for (int i = 0; i < nj; i++)
        coord_j[i] = nodes_j[i + j_offset];
    }
  else
    {
      for (int i = 0; i < nj; i++)
        coord_j[i] = (nodes_j[i + j_offset] + nodes_j[i + j_offset + 1]) * 0.5;
    }
  if (f.get_localisation() == Domaine_IJK::NODES || f.get_localisation() == Domaine_IJK::FACES_K)
    {
      for (int i = 0; i < nk; i++)
        coord_k[i] = nodes_k[i + k_offset];
    }
  else
    {
      for (int i = 0; i < nk; i++)
        coord_k[i] = (nodes_k[i + k_offset] + nodes_k[i + k_offset + 1]) * 0.5;
    }
}

// Initialize field with specified function of x,y,z
void set_field_data(IJK_Field_double& f, double func(double, double, double))
{
  ArrOfDouble coord_i, coord_j, coord_k;
  build_local_coords(f, coord_i, coord_j, coord_k);

  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();

  for (int k = 0; k < nk; k++)
    {
      double z = coord_k[k];
      for (int j = 0; j < nj; j++)
        {
          double y = coord_j[j];
          for (int i = 0; i < ni; i++)
            {
              double x = coord_i[i];
              f(i, j, k) = func(x, y, z);
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

// GAB, Vy_initial non nul en reprise
//  field with specified string expression (must be understood by Parser class)
void compose_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z)
{
  ArrOfDouble coord_i, coord_j, coord_k;
  build_local_coords(f, coord_i, coord_j, coord_k);

  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();

  std::string expr(parser_expression_of_x_y_z);
  Parser parser;
  parser.setString(expr);
  parser.setNbVar(3);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  parser.parseString();
  for (int k = 0; k < nk; k++)
    {
      double z = coord_k[k];
      parser.setVar(2, z);
      for (int j = 0; j < nj; j++)
        {
          double y = coord_j[j];
          parser.setVar(1, y);
          for (int i = 0; i < ni; i++)
            {
              double x = coord_i[i];
              parser.setVar((int) 0, x);
              f(i, j, k) += parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}
//

// Initialize field with specified string expression (must be understood by Parser class)
void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z)
{
  ArrOfDouble coord_i, coord_j, coord_k;
  build_local_coords(f, coord_i, coord_j, coord_k);

  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();

  std::string expr(parser_expression_of_x_y_z);
  Parser parser;
  parser.setString(expr);
  parser.setNbVar(3);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  parser.parseString();
  for (int k = 0; k < nk; k++)
    {
      double z = coord_k[k];
      parser.setVar(2, z);
      for (int j = 0; j < nj; j++)
        {
          double y = coord_j[j];
          parser.setVar(1, y);
          for (int i = 0; i < ni; i++)
            {
              double x = coord_i[i];
              parser.setVar((int) 0, x);
              f(i, j, k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t, const double current_time)
{
  ArrOfDouble coord_i, coord_j, coord_k;
  build_local_coords(f, coord_i, coord_j, coord_k);

  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();

  std::string expr(parser_expression_of_x_y_z_and_t);
  Parser parser;
  parser.setString(expr);
  parser.setNbVar(4);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  parser.addVar("t");
  parser.parseString();
  parser.setVar(3, current_time);
  for (int k = 0; k < nk; k++)
    {
      double z = coord_k[k];
      parser.setVar(2, z);
      for (int j = 0; j < nj; j++)
        {
          double y = coord_j[j];
          parser.setVar(1, y);
          for (int i = 0; i < ni; i++)
            {
              double x = coord_i[i];
              parser.setVar((int) 0, x);
              f(i, j, k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t, const IJK_Field_double& input_f, const double current_time)
{
  ArrOfDouble coord_i, coord_j, coord_k;
  build_local_coords(f, coord_i, coord_j, coord_k);

  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();

  std::string expr(parser_expression_of_x_y_z_and_t);
  Parser parser;
  parser.setString(expr);
  parser.setNbVar(5);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  parser.addVar("t");
  parser.addVar("ff");
  parser.parseString();
  parser.setVar(3, current_time);
  for (int k = 0; k < nk; k++)
    {
      double z = coord_k[k];
      parser.setVar(2, z);
      for (int j = 0; j < nj; j++)
        {
          double y = coord_j[j];
          parser.setVar(1, y);
          for (int i = 0; i < ni; i++)
            {
              double x = coord_i[i];
              parser.setVar((int) 0, x);
              parser.setVar(4, input_f(i, j, k));
              f(i, j, k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t, const IJK_Field_double& input_f1, const IJK_Field_double& input_f2, const double current_time)
{
  ArrOfDouble coord_i, coord_j, coord_k;
  build_local_coords(f, coord_i, coord_j, coord_k);

  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();

  std::string expr(parser_expression_of_x_y_z_and_t);
  Parser parser;
  parser.setString(expr);
  parser.setNbVar(6);
  parser.addVar("x");
  parser.addVar("y");
  parser.addVar("z");
  parser.addVar("t");
  parser.addVar("f1");
  parser.addVar("ff");
  parser.parseString();
  parser.setVar(3, current_time);
  for (int k = 0; k < nk; k++)
    {
      double z = coord_k[k];
      parser.setVar(2, z);
      for (int j = 0; j < nj; j++)
        {
          double y = coord_j[j];
          parser.setVar(1, y);
          for (int i = 0; i < ni; i++)
            {
              double x = coord_i[i];
              parser.setVar((int) 0, x);
              parser.setVar(4, input_f1(i, j, k));
              parser.setVar(5, input_f2(i, j, k));
              f(i, j, k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

void complex_to_trig(const double re, const double im, double& modul, double& arg)
{
  modul = sqrt(re * re + im * im);
  if (re > 0)
    {
      arg = atan(im / re);
    }
  else if (re == 0)
    {
      if (im > 0)
        arg = M_PI / 2.;
      else
        arg = -M_PI / 2.;
      Cerr << "watchout!" << finl;
    }
  else
    {
      arg = M_PI + atan(im / re);
    }
  return;
}

void squared_3x3(double& a11, double& a12, double& a13, double& a21, double& a22, double& a23, double& a31, double& a32, double& a33)
{
  double aa11 = a11 * a11 + a12 * a21 + a13 * a31;
  double aa12 = a11 * a12 + a12 * a22 + a13 * a32;
  double aa13 = a11 * a13 + a12 * a23 + a13 * a33;
  double aa21 = a21 * a11 + a22 * a21 + a23 * a31;
  double aa22 = a21 * a12 + a22 * a22 + a23 * a32;
  double aa23 = a21 * a13 + a22 * a23 + a23 * a33;
  double aa31 = a31 * a11 + a32 * a21 + a33 * a31;
  double aa32 = a31 * a12 + a32 * a22 + a33 * a32;
  double aa33 = a31 * a13 + a32 * a23 + a33 * a33;

  a11 = aa11;
  a12 = aa12;
  a13 = aa13;
  a21 = aa21;
  a22 = aa22;
  a23 = aa23;
  a31 = aa31;
  a32 = aa32;
  a33 = aa33;

}


void interpolate_to_center(IJK_Field_vector3_double& cell_center_field, const IJK_Field_vector3_double& face_field)
{
  // We are not changing the const semantic of the field to update ghost cells:
  IJK_Field_vector3_double& face_fld_non_const = const_cast<IJK_Field_vector3_double&>(face_field);
  face_fld_non_const.echange_espace_virtuel();
  /* Interpole le champ face_field aux centres des elements et le stocke dans cell_center_field */
  const int kmax = cell_center_field[0].nk();
  const int jmax = cell_center_field[0].nj();
  const int imax = cell_center_field[0].ni();
  for (int k = 0; k < kmax; k++)
    for (int j = 0; j < jmax; j++)
      for (int i = 0; i < imax; i++)
        {
          cell_center_field[0](i,j,k) = (face_field[0](i,j,k) + face_field[0](i+1, j, k)) * 0.5;
          cell_center_field[1](i,j,k) = (face_field[1](i,j,k) + face_field[1](i, j+1, k)) * 0.5;
          cell_center_field[2](i,j,k) = (face_field[2](i,j,k) + face_field[2](i, j, k+1)) * 0.5;
        }
}

void interpolate_to_center_compo(IJK_Field_double& cell_center_field_compo, const IJK_Field_double& face_field_compo)
{
  Domaine_IJK::Localisation loc = face_field_compo.get_localisation();
  assert(loc != Domaine_IJK::ELEM && loc != Domaine_IJK::NODES);
  const int kmax = cell_center_field_compo.nk(),
            jmax = cell_center_field_compo.nj(),
            imax = cell_center_field_compo.ni();
  int ci = (int)loc == Domaine_IJK::FACES_I;
  int cj = (int)loc == Domaine_IJK::FACES_J;
  int ck = (int)loc == Domaine_IJK::FACES_K;

  for (int k = 0; k < kmax; k++)
    for (int j = 0; j < jmax; j++)
      for (int i = 0; i < imax; i++)
        cell_center_field_compo(i,j,k) = (face_field_compo(i,j,k) + face_field_compo(i+ci, j+cj, k+ck)) * 0.5;
}



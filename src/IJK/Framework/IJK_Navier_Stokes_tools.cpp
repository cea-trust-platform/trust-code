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

#include <IJK_Navier_Stokes_tools.h>
#include <Parser.h>
#include <IJK_Field_simd_tools.h>
#include <EChaine.h>
#include <SChaine.h>
#include <Interprete_bloc.h>
#include <stat_counters.h>

double compute_fractionnal_timestep_rk3(const double dt_tot, int step)
{
  assert(step >= 0 && step < 3);
  const double intermediate_tstep[3] = { 1./3., 5./12., 1./4. };
  return intermediate_tstep[step] * dt_tot;
}

static void extend_array(const IJK_Grid_Geometry& geom1,
                         const int direction,
                         const int ncells,
                         ArrOfDouble& delta,
                         double& origin)
{
  delta = geom1.get_delta(direction);
  origin= geom1.get_origin(direction);

  if (geom1.get_periodic_flag(direction))
    {
      // On cree des mailles supplementaires au debut et a la fin,
      // on decale l'origine vers la "gauche"
      const int n = delta.size_array(); // nombre de mailles initial
      delta.resize_array(n + 2 * ncells);
      int i;
      // On decale les mailles vers la droite dans delta, de ncells:
      for (i = n-1; i >= 0; i--)
        delta[i+ncells] = delta[i];
      // On duplique les mailles dont on a besoin:
      if (n < ncells)
        {
          Cerr << "Erreur dans build_extended_splitting, direction " << direction
               << " extension de " << ncells << " impossible car seulement "
               << n << " mailles dans le domaine" << finl;
          Process::exit();
        }
      for ( i = 0; i < ncells; i++)
        {
          // Copie des mailles de droite a gauche:
          delta[i] = delta[i + n];
          // Decalage de l'origine:
          origin -= delta[i];
          // Copie des mailles de gauche a droite:
          delta[ncells + i + n] = delta[ncells + i];
        }
    }
}

// split1 : Maillage d'origine sur lequel sont resolues les equations de NS.
// split2 : Resultat etendu contenant le domaine ou vivent les interfaces.
// n_cells : Nombre de cellules supplementaires crees de chaque cote.
//           (doit etre inferieur au nombre de mailles dans le domaine decoupee).
void build_extended_splitting(const IJK_Splitting& split1,
                              IJK_Splitting& split2,
                              int n_cells)
{
  const IJK_Grid_Geometry& geom1 = split1.get_grid_geometry();

  double origin_x, origin_y, origin_z;
  ArrOfDouble dx, dy, dz;
  extend_array(geom1, DIRECTION_I, n_cells, dx, origin_x);
  extend_array(geom1, DIRECTION_J, n_cells, dy, origin_y);
  extend_array(geom1, DIRECTION_K, n_cells, dz, origin_z);

  // Le domaine etendu n'est pas periodique: le champ n'est pas continu
  // entre les bords opposes du domaine etendu.
  IJK_Grid_Geometry geom2;
  Nom n(geom1.le_nom());
  geom2.nommer(n+"_EXT");
  geom2.initialize_origin_deltas(origin_x, origin_y, origin_z,
                                 dx, dy, dz,
                                 geom1.get_periodic_flag(0),
                                 geom1.get_periodic_flag(1),
                                 geom1.get_periodic_flag(2));
  // Construction du decoupage parallele: on utilise les memes parametres
  // de decoupage que pour le maillage d'origine:
  split2.initialize(geom2,
                    split1.get_nprocessor_per_direction(DIRECTION_I),
                    split1.get_nprocessor_per_direction(DIRECTION_J),
                    split1.get_nprocessor_per_direction(DIRECTION_K));
}

Probleme_base& creer_domaine_vdf(const IJK_Splitting& splitting, const Nom& nom_domaine)
{
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  // On va construire une partie de jdd a faire interpreter:
  const double x0 = geom.get_origin(DIRECTION_I);
  const double y0 = geom.get_origin(DIRECTION_J);
  const double z0 = geom.get_origin(DIRECTION_K);
  const double lx = geom.get_domain_length(DIRECTION_I);
  const double ly = geom.get_domain_length(DIRECTION_J);
  const double lz = geom.get_domain_length(DIRECTION_K);
  const int nslicek = splitting.get_nprocessor_per_direction(DIRECTION_K);
  const int nslicej = splitting.get_nprocessor_per_direction(DIRECTION_J);
  const int nslicei = splitting.get_nprocessor_per_direction(DIRECTION_I);
  const int ni = geom.get_nb_elem_tot(DIRECTION_I)+1; // number of nodes
  const int nj = geom.get_nb_elem_tot(DIRECTION_J)+1;
  const int nk = geom.get_nb_elem_tot(DIRECTION_K)+1;

  char fonction_coord_x[300];
  snprintf(fonction_coord_x,300,"%.16g+x*%.16g", x0, lx);
  char fonction_coord_y[300];
  snprintf(fonction_coord_y,300,"%.16g+y*%.16g", y0, ly);
  char fonction_coord_z[300];
  snprintf(fonction_coord_z,300,"%.16g+z*%.16g", z0, lz);

  SChaine instructions;
  instructions << "Dimension 3 " << finl;
  instructions << "Domaine " << nom_domaine << finl;
  instructions << "MaillerParallel" << finl;
  instructions << "{" << finl;
  instructions << "  domain " << nom_domaine << finl;
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
          int p = splitting.get_processor_by_ijk(i,j,k);
          map(p,0) = i;
          map(p,1) = j;
          map(p,2) = k;
        }

  if (min_array(map) < 0)
    {
      Cerr << "Error in creer_domaine_vdf: there are unused processors in the ijk splitting" << finl;
    }
  Nom pb_name = Nom("pb_")+nom_domaine;
  instructions << map << finl;
  instructions << "}" << finl;
  instructions << "Probleme_FT_Disc_gen "<< pb_name << finl;
  instructions << "Associer "<< pb_name<<" " << nom_domaine << finl;
  instructions << "VDF dis"<<nom_domaine << finl;
  instructions << "Schema_Euler_explicite sch"<<nom_domaine<<" Lire sch"<<nom_domaine<<" { nb_pas_dt_max 1 }" << finl;
  instructions << "Associer "<< pb_name<<" sch"<<nom_domaine << finl;
  instructions << "Discretiser "<< pb_name<<" dis"<< nom_domaine << finl;
  Cerr << "Interpretation de la chaine suivante:" << finl << instructions.get_str();

  EChaine is(instructions.get_str());
  Interprete_bloc::interprete_courant().interpreter_bloc(is, Interprete_bloc::BLOC_EOF,
                                                         0 /* flag verifie sans interpreter */);

  // Il faut construire une structure de donnees du Domaine_VF qui n'est pas construite par defaut:
  Probleme_base& pb =  ref_cast(Probleme_base, Interprete_bloc::objet_global(pb_name));
  Domaine& domaine = pb.domaine_dis().valeur().domaine();
  domaine.construire_elem_virt_pe_num();

  return pb;
}




// Imposer une condition limite de vitesse nulle aux parois (en zmin et zmax)
void force_zero_on_walls(IJK_Field_double& vz)
{
  const int nj = vz.nj();
  const int ni = vz.ni();
  const int kmin = vz.get_splitting().get_offset_local(DIRECTION_K);
  const int nktot = vz.get_splitting().get_nb_items_global(IJK_Splitting::FACES_K, DIRECTION_K);
  if (kmin == 0)
    {
      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni; i++)
          vz(i,j,0) = 0.;
    }
  if (kmin + vz.nk() == nktot)
    {
      const int k = vz.nk()-1;
      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni; i++)
          vz(i,j,k) = 0.;
    }
}


void allocate_velocity(FixedVector<IJK_Field_double, 3>& v, const IJK_Splitting& s, int ghost)
{
  v[0].allocate(s, IJK_Splitting::FACES_I, ghost);
  v[1].allocate(s, IJK_Splitting::FACES_J, ghost);
  v[2].allocate(s, IJK_Splitting::FACES_K, ghost);
}

// Interpolate the "field" at the requested "coordinates" (array with 3 columns), and stores into "result"
static void ijk_interpolate_implementation(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result,
                                           int skip_unknown_points, double value_for_bad_points)
{
  const int ghost = field.ghost();
  const int ni = field.ni();
  const int nj = field.nj();
  const int nk = field.nk();

  const IJK_Splitting& splitting = field.get_splitting();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  const double dx = geom.get_constant_delta(DIRECTION_I);
  const double dy = geom.get_constant_delta(DIRECTION_J);
  const double dz = geom.get_constant_delta(DIRECTION_K);
  const IJK_Splitting::Localisation loc = field.get_localisation();
  // L'origine est sur un noeud. Donc que la premiere face en I est sur get_origin(DIRECTION_I)
  double origin_x = geom.get_origin(DIRECTION_I)
                    + ((loc==IJK_Splitting::FACES_J || loc==IJK_Splitting::FACES_K || loc==IJK_Splitting::ELEM) ? (dx * 0.5) : 0. ) ;
  double origin_y = geom.get_origin(DIRECTION_J)
                    + ((loc==IJK_Splitting::FACES_K || loc==IJK_Splitting::FACES_I || loc==IJK_Splitting::ELEM) ? (dy * 0.5) : 0. ) ;
  double origin_z = geom.get_origin(DIRECTION_K)
                    + ((loc==IJK_Splitting::FACES_I || loc==IJK_Splitting::FACES_J || loc==IJK_Splitting::ELEM) ? (dz * 0.5) : 0. ) ;
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
      const int index_i = (int)(floor(x2)) - splitting.get_offset_local(DIRECTION_I);
      const int index_j = (int)(floor(y2)) - splitting.get_offset_local(DIRECTION_J);
      const int index_k = (int)(floor(z2)) - splitting.get_offset_local(DIRECTION_K);
      // Coordonnes barycentriques du points dans la cellule :
      const double xfact = x2 - floor(x2);
      const double yfact = y2 - floor(y2);
      const double zfact = z2 - floor(z2);

      // is point in the domain ? (ghost cells ok...)
      bool ok = (index_i >= -ghost && index_i < ni+ghost-1)
                && (index_j >= -ghost && index_j < nj+ghost-1)
                && (index_k >= -ghost && index_k < nk+ghost-1);
      if (!ok)
        {
          if (skip_unknown_points)
            {
              result[idx] = value_for_bad_points;
              continue; // go to next point
            }
          else
            {
              // Error!
              Cerr << "Error in ijk_interpolate_implementation: request interpolation of point "
                   << x << " " << y << " " << z << " which is outside of the domain on processor " << Process::me() << finl;
              Process::exit();
            }
        }

      double r = (((1.-xfact) * field(index_i, index_j, index_k) + xfact * field(index_i + 1, index_j, index_k)) * (1.-yfact)
                  + ((1.-xfact) * field(index_i, index_j+1, index_k) + xfact * field(index_i + 1, index_j+1, index_k)) * (yfact)) * (1.-zfact)
                 + (((1.-xfact) * field(index_i, index_j, index_k+1) + xfact * field(index_i + 1, index_j, index_k+1)) * (1.-yfact)
                    + ((1.-xfact) * field(index_i, index_j+1, index_k+1) + xfact * field(index_i + 1, index_j+1, index_k+1)) * (yfact)) * (zfact);
      result[idx] = r;
    }
}
void ijk_interpolate_skip_unknown_points(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result,
                                         const double value_for_bad_points)
{
  ijk_interpolate_implementation(field, coordinates, result, 1 /* yes:skip unknown points */, value_for_bad_points);
}

void ijk_interpolate(const IJK_Field_double& field, const DoubleTab& coordinates, ArrOfDouble& result)
{
  ijk_interpolate_implementation(field, coordinates, result, 0 /* skip unknown points=no */, 0.);
}


// Compute, for each cell, the integral on the cell of dt times the divergence of the velocity field.
// Computed as the sum on each face of ("velocity" scalar "normal vector" times "surface of the face")
void compute_divergence_times_constant(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                                       const double constant, IJK_Field_double& resu)
{
  const IJK_Grid_Geometry& geom = vx.get_splitting().get_grid_geometry();
  const double delta_x = geom.get_constant_delta(0);
  const double delta_y = geom.get_constant_delta(1);
  const int kmax = resu.nk();
  const int imax = resu.ni();
  const int jmax = resu.nj();
  const int offset = vx.get_splitting().get_offset_local(DIRECTION_K);
  const ArrOfDouble& delta_z_all = geom.get_delta(DIRECTION_K);
  for (int k = 0; k < kmax; k++)
    {
      const double delta_z = delta_z_all[k+offset];
      const double fx = delta_y * delta_z * constant;
      const double fy = delta_x * delta_z * constant;
      const double fz = delta_x * delta_y * constant;
      for (int j = 0; j < jmax; j++)
        {
          for (int i = 0; i < imax; i++)
            {
              double x = (vx(i+1,j,k) - vx(i,j,k)) * fx
                         + (vy(i,j+1,k) - vy(i,j,k)) * fy
                         + (vz(i,j,k+1) - vz(i,j,k)) * fz;
              resu(i,j,k) = x;
            }
        }
    }
}

// Compute, for each cell, the divergence of the velocity field
// without the product with volume or a constant.
void compute_divergence(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                        IJK_Field_double& resu)
{
  const IJK_Grid_Geometry& geom = vx.get_splitting().get_grid_geometry();
  const double delta_x = geom.get_constant_delta(0);
  const double delta_y = geom.get_constant_delta(1);
  const int kmax = resu.nk();
  const int imax = resu.ni();
  const int jmax = resu.nj();
  const int offset = vx.get_splitting().get_offset_local(DIRECTION_K);
  const ArrOfDouble& delta_z_all = geom.get_delta(DIRECTION_K);
  for (int k = 0; k < kmax; k++)
    {
      const double delta_z = delta_z_all[k+offset];
      const double fx = 1./delta_x;
      const double fy = 1./delta_y;
      const double fz = 1./delta_z;
      for (int j = 0; j < jmax; j++)
        {
          for (int i = 0; i < imax; i++)
            {
              double x = (vx(i+1,j,k) - vx(i,j,k)) * fx
                         + (vy(i,j+1,k) - vy(i,j,k)) * fy
                         + (vz(i,j,k+1) - vz(i,j,k)) * fz;
              resu(i,j,k) = x;
            }
        }
    }
}

// Add to vx, vy and vz the gradient of the pressure gradient multiplied by the constant.
// Input and output velocity is in m/s (not the integral of the momentum on the cell).
// On the walls, don't touch velocity
void add_gradient_times_constant(const IJK_Field_double& pressure, const double constant,
                                 IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz)
{
  const IJK_Grid_Geometry& geom = vx.get_splitting().get_grid_geometry();
  const int kmax = std::max(std::max(vx.nk(), vy.nk()), vz.nk());
  for (int k = 0; k < kmax; k++)
    {
      // i component of velocity
      if (k < vx.nk())
        {
          const int jmax = vx.nj();
          const int imax = vx.ni();
          const double f = constant / geom.get_constant_delta(0);
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vx(i,j,k) += (pressure(i,j,k) - pressure(i-1,j,k)) * f;
        }
      // j component:
      if (k < vy.nk())
        {
          const int jmax = vy.nj();
          const int imax = vy.ni();
          const double f = constant / geom.get_constant_delta(1);
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vy(i,j,k) += (pressure(i,j,k) - pressure(i,j-1,k)) * f;
        }
      // k component:
      bool on_the_wall = false;
      const int k_min = vz.get_splitting().get_offset_local(DIRECTION_K);
      const int nk_tot = vz.get_splitting().get_nb_items_global(IJK_Splitting::FACES_K, DIRECTION_K);
      const int offset = vz.get_splitting().get_offset_local(DIRECTION_K);
      const ArrOfDouble& delta_z_all = geom.get_delta(DIRECTION_K);
      bool perio_k=vz.get_splitting().get_grid_geometry().get_periodic_flag(DIRECTION_K);
      if ((k + k_min == 0 || k + k_min == nk_tot-1) && (!perio_k))
        on_the_wall = true;
      if (k < vz.nk() && (!on_the_wall))
        {
          const int jmax = vz.nj();
          const int imax = vz.ni();
          double f;
          if(!perio_k)
            {
              f = constant * 2. / (delta_z_all[k-1+k_min] + delta_z_all[k+k_min]);
            }
          else
            {
              f = (constant /  delta_z_all[k+offset]);
            }
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vz(i,j,k) += (pressure(i,j,k) - pressure(i,j,k-1)) * f;
        }
    }
}

// Add to vx, vy and vz the gradient of the pressure gradient multiplied by the constant.
// Input and output velocity is in m/s (not the integral of the momentum on the cell).
// On the walls, don't touch velocity
void add_gradient_times_constant_over_rho(const IJK_Field_double& pressure, const IJK_Field_double& rho, const double constant,
                                          IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz)
{
  const IJK_Grid_Geometry& geom = vx.get_splitting().get_grid_geometry();
  const int kmax = std::max(std::max(vx.nk(), vy.nk()), vz.nk());
  for (int k = 0; k < kmax; k++)
    {
      // i component of velocity
      if (k < vx.nk())
        {
          const int jmax = vx.nj();
          const int imax = vx.ni();
          const double f = constant / geom.get_constant_delta(0) * 2.;
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vx(i,j,k) += (pressure(i,j,k) - pressure(i-1,j,k)) / (rho(i,j,k) + rho(i-1,j,k)) * f;
        }
      // j component:
      if (k < vy.nk())
        {
          const int jmax = vy.nj();
          const int imax = vy.ni();
          const double f = constant / geom.get_constant_delta(1) * 2.;
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vy(i,j,k) += (pressure(i,j,k) - pressure(i,j-1,k)) / (rho(i,j,k) + rho(i,j-1,k))  * f;
        }
      // k component:
      bool on_the_wall = false;
      const int k_min = vz.get_splitting().get_offset_local(DIRECTION_K);
      const int nk_tot = vz.get_splitting().get_nb_items_global(IJK_Splitting::FACES_K, DIRECTION_K);
      const int offset = vz.get_splitting().get_offset_local(DIRECTION_K);
      const ArrOfDouble& delta_z_all = geom.get_delta(DIRECTION_K);
      bool perio_k=vz.get_splitting().get_grid_geometry().get_periodic_flag(DIRECTION_K);

      if ((k + k_min == 0 || k + k_min == nk_tot-1) && (!perio_k))
        on_the_wall = true;
      if (k < vz.nk() && (!on_the_wall))
        {
          const int jmax = vz.nj();
          const int imax = vz.ni();
          double f;
          if(!perio_k)
            {
              f = constant * 2. / (delta_z_all[k-1+k_min] + delta_z_all[k+k_min]) * 2.;
            }
          else
            {
              f = (constant /  delta_z_all[k+offset]) * 2.;
            }
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vz(i,j,k) += (pressure(i,j,k) - pressure(i,j,k-1)) / (rho(i,j,k) + rho(i,j,k-1)) * f;
        }
    }
}
void add_gradient_times_constant_times_inv_rho(const IJK_Field_double& pressure, const IJK_Field_double& inv_rho, const double constant,
                                               IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz)
{
  const IJK_Grid_Geometry& geom = vx.get_splitting().get_grid_geometry();
  const int kmax = std::max(std::max(vx.nk(), vy.nk()), vz.nk());
  for (int k = 0; k < kmax; k++)
    {
      // i component of velocity
      if (k < vx.nk())
        {
          const int jmax = vx.nj();
          const int imax = vx.ni();
          const double f = constant / geom.get_constant_delta(0) * 0.5;
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vx(i,j,k) += (pressure(i,j,k) - pressure(i-1,j,k)) * (inv_rho(i,j,k) + inv_rho(i-1,j,k)) * f;
        }
      // j component:
      if (k < vy.nk())
        {
          const int jmax = vy.nj();
          const int imax = vy.ni();
          const double f = constant / geom.get_constant_delta(1) * 0.5;
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vy(i,j,k) += (pressure(i,j,k) - pressure(i,j-1,k)) * (inv_rho(i,j,k) + inv_rho(i,j-1,k))  * f;
        }
      // k component:
      bool on_the_wall = false;
      const int k_min = vz.get_splitting().get_offset_local(DIRECTION_K);
      const int nk_tot = vz.get_splitting().get_nb_items_global(IJK_Splitting::FACES_K, DIRECTION_K);
      const int offset = vz.get_splitting().get_offset_local(DIRECTION_K);
      const ArrOfDouble& delta_z_all = geom.get_delta(DIRECTION_K);
      bool perio_k=vz.get_splitting().get_grid_geometry().get_periodic_flag(DIRECTION_K);

      if ((k + k_min == 0 || k + k_min == nk_tot-1) && (!perio_k))
        on_the_wall = true;
      if (k < vz.nk() && (!on_the_wall))
        {
          const int jmax = vz.nj();
          const int imax = vz.ni();
          double f;
          if(!perio_k)
            {
              f = constant * 2. / (delta_z_all[k-1+k_min] + delta_z_all[k+k_min]) * 0.5;
            }
          else
            {
              f = (constant /  delta_z_all[k+offset]) * 0.5;
            }
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vz(i,j,k) += (pressure(i,j,k) - pressure(i,j,k-1)) * (inv_rho(i,j,k) + inv_rho(i,j,k-1)) * f;
        }
    }
}

// Projects the input velocity field on the zero divergence subspace by solving the Poisson equation:
//  v_output = v_input - dt * gradient(pressure)
// pressure is such that div(v_output) = 0, that is, solve for pressure_ satisfying:
//  div(v_input) + div(-dt * gradient(pressure)),
// that is:
//  div(gradient(pressure_)) = 1/dt * div(v_input)
//
// input value of pressure_ is used as an initial guess by the solver
void pressure_projection(IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz,
                         IJK_Field_double& pressure, double dt,
                         IJK_Field_double& pressure_rhs,
                         int check_divergence,
                         Multigrille_Adrien& poisson_solver, double Shear_DU)
{
  static Stat_Counter_Id projection_counter_ = statistiques().new_counter(2, "maj vitesse : projection");
  statistiques().begin_count(projection_counter_);
  // We need the velocity on the face at right to compute the divergence:
  vx.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_I*/, Shear_DU);
  vy.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_J*/);
  vz.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_K*/);
  compute_divergence_times_constant(vx, vy, vz, -1./dt, pressure_rhs);

  // transformer pressure_rhs pour rendre le probleme triperiodique
  // on resoud ensuite p(x*,y*,z*)
  double divergence_before = 0.;
  if (check_divergence)
    {
      divergence_before = norme_ijk(pressure_rhs);
    }

//  IJK_Splitting::defilement_ = 0;
  poisson_solver.resoudre_systeme_IJK(pressure_rhs, pressure);
  // pressure gradient requires the "left" value in all directions:
  // transformation inverse pour que la pression satisfasse la condition de cisaillement.
  // p(x*,y*,z*)-->p(x,y,z)
//  IJK_Splitting::defilement_ = 1;
  pressure.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_LEFT_IJK*/);
  add_gradient_times_constant(pressure, -dt, vx, vy, vz);
  if (check_divergence)
    {
      IJK_Field rhs_after(pressure_rhs);

      vx.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_I*/, Shear_DU);
      vy.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_J*/);
      vz.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_K*/);

      compute_divergence_times_constant(vx, vy, vz, 1./dt, rhs_after);
      double divergence_after = norme_ijk(rhs_after);
      Cout << "Divergence of velocity field: before solver: " << divergence_before << " after solver: " << divergence_after << finl;
    }
  statistiques().end_count(projection_counter_);
}

// Projects the input velocity field on the zero divergence subspace by solving the Poisson equation:
//  v_output = v_input - dt * gradient(pressure)
// pressure is such that div(v_output) = 0, that is, solve for pressure_ satisfying:
//  div(v_input) + div(-dt * gradient(pressure)),
// that is:
//  div(gradient(pressure_)) = 1/dt * div(v_input)
//
// input value of pressure_ is used as an initial guess by the solver
void pressure_projection_with_rho(const IJK_Field_double& rho,
                                  IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz,
                                  IJK_Field_double& pressure, double dt,
                                  IJK_Field_double& pressure_rhs,
                                  int check_divergence,
                                  Multigrille_Adrien& poisson_solver, double Shear_DU)
{
  static Stat_Counter_Id projection_counter_ = statistiques().new_counter(2, "maj vitesse : projection");
  statistiques().begin_count(projection_counter_);
  // We need the velocity on the face at right to compute the divergence:
  vx.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_I*/, Shear_DU);
  vy.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_J*/);
  vz.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_K*/);
  compute_divergence_times_constant(vx, vy, vz, -1./dt, pressure_rhs);
  double divergence_before = 0.;
  if (check_divergence)
    {
      divergence_before = norme_ijk(pressure_rhs);
    }
  //IJK_Splitting::defilement_ = 0;

  poisson_solver.set_rho(rho);
  poisson_solver.resoudre_systeme_IJK(pressure_rhs, pressure);
  //IJK_Splitting::defilement_ = 1;

  // pressure gradient requires the "left" value in all directions:
  pressure.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_LEFT_IJK*/);
  add_gradient_times_constant_over_rho(pressure, rho, -dt, vx, vy, vz);
  if (check_divergence)
    {
      IJK_Field rhs_after(pressure_rhs);

      vx.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_I*/, Shear_DU);
      vy.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_J*/);
      vz.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_K*/);

      compute_divergence_times_constant(vx, vy, vz, 1./dt, rhs_after);
      double divergence_after = norme_ijk(rhs_after);
      Cout << "Divergence of velocity field: before solver: " << divergence_before << " after solver: " << divergence_after << finl;
    }
  statistiques().end_count(projection_counter_);
}

// Methode basee sur 1/rho au lieu de rho :
void pressure_projection_with_inv_rho(const IJK_Field_double& inv_rho,
                                      IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz,
                                      IJK_Field_double& pressure, double dt,
                                      IJK_Field_double& pressure_rhs,
                                      int check_divergence,
                                      Multigrille_Adrien& poisson_solver, double Shear_DU)
{
  static Stat_Counter_Id projection_counter_ = statistiques().new_counter(2, "maj vitesse : projection");
  statistiques().begin_count(projection_counter_);
  // We need the velocity on the face at right to compute the divergence:
  vx.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_I*/, Shear_DU);
  vy.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_J*/);
  vz.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_K*/);
  compute_divergence_times_constant(vx, vy, vz, -1./dt, pressure_rhs);
  double divergence_before = 0.;
  if (check_divergence)
    {
      divergence_before = norme_ijk(pressure_rhs);
    }
  poisson_solver.set_inv_rho(inv_rho); // Attention, on met l'inverse de rho.
//  IJK_Splitting::defilement_ = 0;
  //pressure_rhs.change_to_sheared_reference_frame(-1, 0);
  // Fait aussi : compute_faces_coefficients_from_inv_rho
  poisson_solver.resoudre_systeme_IJK(pressure_rhs, pressure);
  // pressure gradient requires the "left" value in all directions:
//  IJK_Splitting::defilement_ = 1;
//  pressure.change_to_sheared_reference_frame(1, 0);
//  pressure_rhs.change_to_sheared_reference_frame(1, 0);
  pressure.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_LEFT_IJK*/);
  add_gradient_times_constant_times_inv_rho(pressure, inv_rho, -dt, vx, vy, vz);
  if (check_divergence)
    {
      IJK_Field rhs_after(pressure_rhs);

      vx.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_I*/, Shear_DU);
      vy.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_J*/);
      vz.echange_espace_virtuel(1 /*, IJK_Field_double::EXCHANGE_GET_AT_RIGHT_K*/);

      compute_divergence_times_constant(vx, vy, vz, 1./dt, rhs_after);
      double divergence_after = norme_ijk(rhs_after);
      Cout << "Divergence of velocity field: before solver: " << divergence_before << " after solver: " << divergence_after << finl;
    }
  statistiques().end_count(projection_counter_);
}


// Take the provided derivative dv and update F and the unknown v for the Runge Kutta "rk_step"
//  (0<=rk_step<=2).
// F is an intermediate result, from the previous rk step (overwritten at step==0), used by
//  the RungeKutta algorithm
// dv is the derivative evaluated at the current step
// v is the unknown, the derivative for the next step must be computed with the output v of the
// this step, after the last step, v is the final value at the end of the timestep_
// dt_tot: total timestep
void runge_kutta3_update(const IJK_Field_double& dv, IJK_Field_double& F, IJK_Field_double& v,
                         const int step, const int k_layer, double dt_tot)
{
  const double coeff_a[3] = { 0., -5./9., -153./128. };
  // Fk[0] = 1; Fk[i+1] = Fk[i] * a[i+1] + 1
  const double coeff_Fk[3] = { 1., 4./9., 15./32. };

  const double facteurF = coeff_a[step];
  const double intermediate_dt = compute_fractionnal_timestep_rk3(dt_tot, step);
  const double delta_t_divided_by_Fk = intermediate_dt / coeff_Fk[step];
  const int imax = v.ni();
  const int jmax = v.nj();
  switch(step)
    {
    case 0:
      // don't read initial value of F (no performance benefit because write to F causes the
      // processor to fetch the cache line, but we don't wand to use a potentially uninitialized value
      for (int j = 0; j < jmax; j++)
        {
          for (int i = 0; i < imax; i++)
            {
              double x = dv(i,j,k_layer);
              F(i,j,k_layer) = x;
              v(i,j,k_layer) += x * delta_t_divided_by_Fk;
            }
        }
      break;
    case 1:
      // general case, read and write F
      for (int j = 0; j < jmax; j++)
        {
          for (int i = 0; i < imax; i++)
            {
              double x = F(i,j,k_layer) * facteurF + dv(i,j,k_layer);
              F(i,j,k_layer) = x;
              v(i,j,k_layer) += x * delta_t_divided_by_Fk;
            }
        }
      break;
    case 2:
      // do not write F
      for (int j = 0; j < jmax; j++)
        {
          for (int i = 0; i < imax; i++)
            {
              double x = F(i,j,k_layer) * facteurF + dv(i,j,k_layer);
              v(i,j,k_layer) += x * delta_t_divided_by_Fk;
            }
        }
      break;
    default:
      Cerr << "Error in runge_kutta_update: wrong step" << finl;
      Process::exit();
    };
}
#if 0
// Copie de la methode precedente mais pour les DoubleTab aux sommets :
void runge_kutta3_update(const DoubleTab& dvi, DoubleTab& G, DoubleTab& l,
                         const int step, const double dt_tot,
                         const Maillage_FT_IJK& maillage)
{
  const double coeff_a[3] = { 0., -5./9., -153./128. };
  // Gk[0] = 1; Gk[i+1] = Gk[i] * a[i+1] + 1
  const double coeff_Gk[3] = { 1., 4./9., 15./32. };

  const double facteurG = coeff_a[step];
  const double intermediate_dt = compute_fractionnal_timestep_rk3(dt_tot, step);
  const double delta_t_divided_by_Gk = intermediate_dt / coeff_Gk[step];
  const int nbsom = maillage.nb_sommets();

  // Resize du tableau
  //  G.set_smart_resize(1) ?
  G.resize(nbsom, 3);

  switch(step)
    {
    case 0:
      // don't read initial value of G (no performance benefit because write to G causes the
      // processor to fetch the cache line, but we don't wand to use a potentially uninitialized value
      for (int isom = 0; isom < nbsom; isom++)
        {
          for (int dir = 0; dir < 3; dir++)
            {
              if (maillage.sommet_virtuel(isom))
                {
                  G(isom,dir) = 111./intermediate_dt;
                  l(isom,dir) = 111./intermediate_dt;
                }
              double x = dvi(isom,dir);
              G(isom,dir) = x;
              l(isom,dir) += x * delta_t_divided_by_Gk;
            }
        }
      break;
    case 1:
      // general case, read and write G
      for (int isom = 0; isom < nbsom; isom++)
        {
          for (int dir = 0; dir < 3; dir++)
            {
              if (maillage.sommet_virtuel(isom))
                {
                  G(isom,dir) = 111./intermediate_dt;
                  l(isom,dir) = 111./intermediate_dt;
                }
              double x = G(isom,dir) * facteurG + dvi(isom,dir);
              G(isom,dir) = x;
              l(isom,dir) += x * delta_t_divided_by_Gk;
            }
        }
      break;
    case 2:
      // do not write G
      for (int isom = 0; isom < nbsom; isom++)
        {
          for (int dir = 0; dir < 3; dir++)
            {
              if (maillage.sommet_virtuel(isom))
                {
                  G(isom,dir) = 111./intermediate_dt;
                  l(isom,dir) = 111./intermediate_dt;
                }
              double x = G(isom,dir) * facteurG + dvi(isom,dir);
              l(isom,dir) += x * delta_t_divided_by_Gk;
            }
        }
      break;
    default:
      Cerr << "Error in runge_kutta_update: wrong step" << finl;
      Process::exit();
    };
}
#endif

// build local coordinates of discretisation nodes for the given field
// (used in set_field_data() )
void build_local_coords(const IJK_Field_double& f, ArrOfDouble& coord_i, ArrOfDouble& coord_j, ArrOfDouble& coord_k)
{
  const IJK_Grid_Geometry& geom = f.get_splitting().get_grid_geometry();
  const int i_offset = f.get_splitting().get_offset_local(DIRECTION_I);
  const int j_offset = f.get_splitting().get_offset_local(DIRECTION_J);
  const int k_offset = f.get_splitting().get_offset_local(DIRECTION_K);

  const ArrOfDouble& nodes_i = geom.get_node_coordinates(0);
  const ArrOfDouble& nodes_j = geom.get_node_coordinates(1);
  const ArrOfDouble& nodes_k = geom.get_node_coordinates(2);
  const int ni = f.ni();
  const int nj = f.nj();
  const int nk = f.nk();
  coord_i.resize_array(ni);
  coord_j.resize_array(nj);
  coord_k.resize_array(nk);

  if (f.get_localisation() == IJK_Splitting::NODES || f.get_localisation() == IJK_Splitting::FACES_I)
    {
      for (int i = 0; i < ni; i++)
        coord_i[i] = nodes_i[i+i_offset];
    }
  else
    {
      for (int i = 0; i < ni; i++)
        coord_i[i] = (nodes_i[i+i_offset] + nodes_i[i+i_offset+1]) * 0.5;
    }
  if (f.get_localisation() == IJK_Splitting::NODES || f.get_localisation() == IJK_Splitting::FACES_J)
    {
      for (int i = 0; i < nj; i++)
        coord_j[i] = nodes_j[i+j_offset];
    }
  else
    {
      for (int i = 0; i < nj; i++)
        coord_j[i] = (nodes_j[i+j_offset] + nodes_j[i+j_offset+1]) * 0.5;
    }
  if (f.get_localisation() == IJK_Splitting::NODES || f.get_localisation() == IJK_Splitting::FACES_K)
    {
      for (int i = 0; i < nk; i++)
        coord_k[i] = nodes_k[i+k_offset];
    }
  else
    {
      for (int i = 0; i < nk; i++)
        coord_k[i] = (nodes_k[i+k_offset] + nodes_k[i+k_offset+1]) * 0.5;
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
              f(i,j,k)=func(x, y, z);
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
              parser.setVar((int)0, x);
              f(i,j,k) += parser.eval();
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
              parser.setVar((int)0, x);
              f(i,j,k) = parser.eval();
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
              parser.setVar((int)0, x);
              f(i,j,k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}


void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t,
                    const IJK_Field_double& input_f, const double current_time)
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
              parser.setVar((int)0, x);
              parser.setVar(4, input_f(i,j,k));
              f(i,j,k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

void set_field_data(IJK_Field_double& f, const Nom& parser_expression_of_x_y_z_and_t,
                    const IJK_Field_double& input_f1, const IJK_Field_double& input_f2, const double current_time)
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
              parser.setVar((int)0, x);
              parser.setVar(4, input_f1(i,j,k));
              parser.setVar(5, input_f2(i,j,k));
              f(i,j,k) = parser.eval();
            }
        }
    }
  f.echange_espace_virtuel(f.ghost());
}

// Calculer rho*v sur la couche k de faces dans la direction DIR
// a partir de rho aux elements et v aux faces
static void calculer_rho_v_DIR(DIRECTION _DIR_, const IJK_Field_double& input_rho, const IJK_Field_double& input_v, IJK_Field_double& rho_v, const int k)
{
  const int ni = rho_v.ni();
  const int nj = rho_v.nj();
  ConstIJK_double_ptr rho(input_rho, 0, 0, k);
  ConstIJK_double_ptr v(input_v, 0, 0, k); // pointeur sur le plan k de la vitesse
  IJK_double_ptr resu(rho_v, 0, 0, k);

  for (int j = 0; j < nj; j++)
    {
      for (int i = 0; i < ni; i++)
        {
          double a, b, c;
          rho.get_left_center(_DIR_, i, a, b); // a et b sont les masses volumiques a gauche et a droite de la face
          v.get_center(i, c);
          resu.put_val(i, (a+b)*c*0.5);
        }
      if (j < nj-1)
        {
          rho.next_j();
          v.next_j();
          resu.next_j();
        }
    }
}

// Remplace la moyenne arithmetique par une moyenne des inverses :
// rho_face = 2 rho1 rho2 / (rho1+rho2)  => 1/rho_face = (1/rho1 + 1/rho2) /2
static void calculer_rho_harmonic_v_DIR(DIRECTION _DIR_, const IJK_Field_double& input_rho, const IJK_Field_double& input_v, IJK_Field_double& rho_v, const int k)
{
  const int ni = rho_v.ni();
  const int nj = rho_v.nj();
  ConstIJK_double_ptr rho(input_rho, 0, 0, k);
  ConstIJK_double_ptr v(input_v, 0, 0, k); // pointeur sur le plan k de la vitesse
  IJK_double_ptr resu(rho_v, 0, 0, k);

  for (int j = 0; j < nj; j++)
    {
      for (int i = 0; i < ni; i++)
        {
          double a, b, c;
          rho.get_left_center(_DIR_, i, a, b); // a et b sont les masses volumiques a gauche et a droite de la face
          v.get_center(i, c);
          resu.put_val(i, 2.*a*b/(a+b)*c);
        }
      if (j < nj-1)
        {
          rho.next_j();
          v.next_j();
          resu.next_j();
        }
    }
}


void calculer_rho_v(const IJK_Field_double& rho,
                    const FixedVector<IJK_Field_double, 3>& v,
                    FixedVector<IJK_Field_double, 3>& rho_v)
{
  // Conditions aux limites plans: on suppose que v = 0 sur le plan, alors rho_v sera = 0,
  // donc il n'y a rien a faire.
  const int nk = std::max(rho_v[0].nk(), std::max(rho_v[1].nk(), rho_v[2].nk()));
  // Boucle sur les plans de maillage pour reutiliser les valeurs de rho mises en cache
  for (int k = 0; k < nk; k++)
    {
      // Calcul des trois composantes de vitesse pour ce plan de maillage
      if (k < rho_v[0].nk())
        calculer_rho_v_DIR(DIRECTION::X, rho, v[0], rho_v[0], k);
      if (k < rho_v[1].nk())
        calculer_rho_v_DIR(DIRECTION::Y, rho, v[1], rho_v[1], k);
      if (k < rho_v[2].nk())
        calculer_rho_v_DIR(DIRECTION::Z, rho, v[2], rho_v[2], k);
    }
}

// On utilise la moyenne harmonique au lieu de la moyenne arithmetique.
void calculer_rho_harmonic_v(const IJK_Field_double& rho,
                             const FixedVector<IJK_Field_double, 3>& v,
                             FixedVector<IJK_Field_double, 3>& rho_v)
{
  // Conditions aux limites plans: on suppose que v = 0 sur le plan, alors rho_v sera = 0,
  // donc il n'y a rien a faire.
  const int nk = std::max(rho_v[0].nk(), std::max(rho_v[1].nk(), rho_v[2].nk()));
  // Boucle sur les plans de maillage pour reutiliser les valeurs de rho mises en cache
  for (int k = 0; k < nk; k++)
    {
      // Calcul des trois composantes de vitesse pour ce plan de maillage
      if (k < rho_v[0].nk())
        calculer_rho_harmonic_v_DIR(DIRECTION::X, rho, v[0], rho_v[0], k);
      if (k < rho_v[1].nk())
        calculer_rho_harmonic_v_DIR(DIRECTION::Y, rho, v[1], rho_v[1], k);
      if (k < rho_v[2].nk())
        calculer_rho_harmonic_v_DIR(DIRECTION::Z, rho, v[2], rho_v[2], k);
    }
}


// Calculer rho*v sur la couche k de faces dans la direction DIR
// a partir de rho aux elements et v aux faces
static void mass_solver_with_rho_DIR(DIRECTION _DIR_, const IJK_Field_double& input_rho, IJK_Field_double& velocity, const double volume, const int k)
{
  const int ni = velocity.ni();
  const int nj = velocity.nj();
  ConstIJK_double_ptr rho(input_rho, 0, 0, k);
  IJK_double_ptr v(velocity, 0, 0, k);

  const double facteur = volume * 0.5;

  for (int j = 0; j < nj; j++)
    {
      for (int i = 0; i < ni; i++)
        {
          double a = 0., b = 0., c, resu;
          rho.get_left_center(_DIR_, i, a, b); // a et b sont les masses volumiques a gauche et a droite de la face
          v.get_center(i, c); // v
          resu = c / ((a + b) * facteur); // division par le produit (volume * rho_face)
          v.put_val(i, resu);
        }
      if (j < nj-1)
        {
          rho.next_j();
          v.next_j();
        }
    }
}
// Remplace la moyenne arithmetique par une moyenne des inverses :
// rho_face = 2 rho1 rho2 / (rho1+rho2)  => 1/rho_face = (1/rho1 + 1/rho2) /2
static void mass_solver_with_inv_rho_DIR(DIRECTION _DIR_, const IJK_Field_double& input_inv_rho, IJK_Field_double& velocity, const double volume, const int k)
{
  const int ni = velocity.ni();
  const int nj = velocity.nj();
  ConstIJK_double_ptr inv_rho(input_inv_rho, 0, 0, k);
  IJK_double_ptr v(velocity, 0, 0, k);

  const double facteur = 0.5 / volume;

  for (int j = 0; j < nj; j++)
    {
      for (int i = 0; i < ni; i++)
        {
          double a=0., b=0., c, resu;
          inv_rho.get_left_center(_DIR_, i, a, b); // a et b sont les masses volumiques a gauche et a droite de la face
          v.get_center(i, c); // v
          resu = c * (a + b) * facteur; // v * inv_rho_face * volume
          v.put_val(i, resu);
        }
      if (j < nj-1)
        {
          inv_rho.next_j();
          v.next_j();
        }
    }
}

// Calcule le volume d'un volume de controle situe a l'indice local k
// en fonction de la localisation du champ
// (suppose que le maillage est uniforme en i et j)
// Si maillage non periodique, renvoie le demi-volume pour les faces paroi
double get_channel_control_volume(IJK_Field_double& field, int local_k_layer, const ArrOfDouble_with_ghost& delta_z_local)
{
  double delta_z;
  const double delta_x = field.get_splitting().get_grid_geometry().get_constant_delta(0);
  const double delta_y = field.get_splitting().get_grid_geometry().get_constant_delta(1);
  switch (field.get_localisation())
    {
    case IJK_Splitting::ELEM:
    case IJK_Splitting::FACES_I:
    case IJK_Splitting::FACES_J:
      delta_z = delta_z_local[local_k_layer];
      break;
    case IJK_Splitting::FACES_K:
      if (! field.get_splitting().get_grid_geometry().get_periodic_flag(DIRECTION_K))
        {
          const int global_k_index = local_k_layer + field.get_splitting().get_offset_local(DIRECTION_K);
          const int last_global_k = field.get_splitting().get_nb_items_global(IJK_Splitting::FACES_K, DIRECTION_K)-1;
          // We have walls, are we on a wall ?
          if (global_k_index == 0)
            {
              delta_z = delta_z_local[local_k_layer] * 0.5; // size of unique neighboug cell
            }
          else if (global_k_index == last_global_k)
            {
              delta_z = delta_z_local[local_k_layer-1] * 0.5; // size of unique neighboug cell
            }
          else
            {
              delta_z = (delta_z_local[local_k_layer-1] + delta_z_local[local_k_layer]) * 0.5;
            }
        }
      else
        {
          // Periodic mesh: always the same formula:
          delta_z = (delta_z_local[local_k_layer-1] + delta_z_local[local_k_layer]) * 0.5;
        }
      break;
    default:
      delta_z = 0.;
      Cerr << "Error in get_channel_control_volume: invalid field localisation" << finl;
      Process::exit();
    }
  return delta_x * delta_y * delta_z;
}

// Division de toutes les valeurs stockees dans velocity par le volume du "volume de controle"
//  et par la mase volumique moyenne au noeud de vitesse.
// (meme moyenne que pour calculer_rho_v)
void mass_solver_with_rho(IJK_Field_double& velocity, const IJK_Field_double& rho, const ArrOfDouble_with_ghost& delta_z_local, const int k)
{
  const double volume = get_channel_control_volume(velocity, k, delta_z_local);
  switch(velocity.get_localisation())
    {
    case IJK_Splitting::FACES_I:
      mass_solver_with_rho_DIR(DIRECTION::X, rho, velocity, volume, k);
      break;
    case IJK_Splitting::FACES_J:
      mass_solver_with_rho_DIR(DIRECTION::Y, rho, velocity, volume, k);
      break;
    case IJK_Splitting::FACES_K:
      mass_solver_with_rho_DIR(DIRECTION::Z, rho, velocity, volume, k);
      break;
    default:
      Process::exit();
    }
}

// Au lieu de diviser par rho, on multiplie par inv_rho
// C'est mieux car en discret : inv_rho = 1/rho_l * chi_l + 1/rho_v * (1-chi_l)
void mass_solver_with_inv_rho(IJK_Field_double& velocity, const IJK_Field_double& inv_rho, const ArrOfDouble_with_ghost& delta_z_local, const int k)
{
  const double volume = get_channel_control_volume(velocity, k, delta_z_local);
  switch(velocity.get_localisation())
    {
    case IJK_Splitting::FACES_I:
      mass_solver_with_inv_rho_DIR(DIRECTION::X, inv_rho, velocity, volume, k);
      break;
    case IJK_Splitting::FACES_J:
      mass_solver_with_inv_rho_DIR(DIRECTION::Y, inv_rho, velocity, volume, k);
      break;
    case IJK_Splitting::FACES_K:
      mass_solver_with_inv_rho_DIR(DIRECTION::Z, inv_rho, velocity, volume, k);
      break;
    default:
      Process::exit();
    }
}

void mass_solver_scalar(IJK_Field_double& dv, const ArrOfDouble_with_ghost& delta_z_local, int k_index)
{
  const double volume = get_channel_control_volume(dv, k_index, delta_z_local);
  const double constant = 1. / volume;
  const int imax = dv.ni();
  const int jmax = dv.nj();
  for (int j = 0; j < jmax; j++)
    {
      for (int i = 0; i < imax; i++)
        {
          dv(i, j, k_index) *= constant;
        }
    }
}

// Division de toutes les valeurs stockees dans velocity par
// la mase volumique moyenne au noeud de vitesse.
// (meme moyenne que pour calculer_rho_v)
void density_solver_with_rho(IJK_Field_double& velocity, const IJK_Field_double& rho, const ArrOfDouble_with_ghost& delta_z_local, const int k)
{
  switch(velocity.get_localisation())
    {
    case IJK_Splitting::FACES_I:
      mass_solver_with_rho_DIR(DIRECTION::X, rho, velocity, 1., k);
      break;
    case IJK_Splitting::FACES_J:
      mass_solver_with_rho_DIR(DIRECTION::Y, rho, velocity, 1., k);
      break;
    case IJK_Splitting::FACES_K:
      mass_solver_with_rho_DIR(DIRECTION::Z, rho, velocity, 1., k);
      break;
    default:
      Process::exit();
    }
}


// fonction moyenne en temps du champs de vitesse utilise dans le cas de bulles fixes
void update_integral_velocity(const FixedVector<IJK_Field_double, 3>& v_instant,  FixedVector<IJK_Field_double, 3>& v_tmp, const IJK_Field_double& indic, const IJK_Field_double& indic_tmp)
{
  const int ni = indic_tmp.ni();
  const int nj = indic_tmp.nj();
  const int nk = indic_tmp.nk();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              double u, v, w ;
              u = (v_instant[0](i,j,k) + v_instant[0](i+1, j, k)) * 0.5;
              v = (v_instant[1](i,j,k) + v_instant[1](i, j+1, k)) * 0.5;
              w = (v_instant[2](i,j,k) + v_instant[2](i, j, k+1)) * 0.5;
              if(indic_tmp(i,j,k)+indic(i,j,k)!=0.0 )
                {
                  v_tmp[0](i,j,k) = (v_tmp[0](i,j,k)*indic_tmp(i,j,k)+u*indic(i,j,k))/(indic_tmp(i,j,k)+indic(i,j,k));
                  v_tmp[1](i,j,k) = (v_tmp[1](i,j,k)*indic_tmp(i,j,k)+v*indic(i,j,k))/(indic_tmp(i,j,k)+indic(i,j,k));
                  v_tmp[2](i,j,k) = (v_tmp[2](i,j,k)*indic_tmp(i,j,k)+w*indic(i,j,k))/(indic_tmp(i,j,k)+indic(i,j,k));
                }
            }
        }
    }
  v_tmp[0].echange_espace_virtuel(v_tmp[0].ghost());
  v_tmp[1].echange_espace_virtuel(v_tmp[1].ghost());
  v_tmp[2].echange_espace_virtuel(v_tmp[2].ghost());
  return;
}

/*
* Calcul le gradient de U aux cellules a partir de la vitesse aux faces
* all components or not.
*/
void compute_and_store_gradU_cell(const IJK_Field_double& vitesse_i,
                                  const IJK_Field_double& vitesse_j,
                                  const IJK_Field_double& vitesse_k,
                                  /* Et les champs en sortie */
                                  IJK_Field_double& dudx, IJK_Field_double& dvdy, IJK_Field_double& dwdx,
                                  IJK_Field_double& dudz, IJK_Field_double& dvdz, IJK_Field_double& dwdz,
                                  const int compute_all,
                                  /* Following will be untouched if compute_all is 0 */
                                  IJK_Field_double& dudy, IJK_Field_double& dvdx, IJK_Field_double& dwdy,
                                  IJK_Field_double& lambda2)
{
  const IJK_Splitting& splitting = vitesse_i.get_splitting();

  // Pour detacher de toute classe :
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  const double dx=geom.get_constant_delta(0);
  const double dy=geom.get_constant_delta(1);
  const ArrOfDouble& tab_dz=geom.get_delta(2);

  // Nombre total de mailles en K
  const int nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, DIRECTION_K);
  // Nombre local de mailles :
  const int imax = splitting.get_nb_items_local(IJK_Splitting::ELEM, 0);
  const int jmax = splitting.get_nb_items_local(IJK_Splitting::ELEM, 1);
  const int kmax = splitting.get_nb_items_local(IJK_Splitting::ELEM, 2);
  const int offset = splitting.get_offset_local(DIRECTION_K);
  double residue = 0.;
  for (int k = 0; k < kmax; k++)
    {
      const double dz = tab_dz[k+offset];
      bool on_the_first_cell = false;
      bool on_the_last_cell = false;
      if (k + offset == 0)
        on_the_first_cell = true;
      if (k + offset == nktot - 1)
        on_the_last_cell = true;
      for (int j = 0; j < jmax; j++)
        {
          for (int i = 0; i < imax; i++)
            {
              // ******************************** //
              // derivation direction x
              // de Ux
              dudx(i,j,k) = (vitesse_i(i+1, j, k) - vitesse_i(i,j,k))/dx;

              // de Uz !!!!!! ATTENTION on veux calculer la moyenne entre (i,j,k) et (i,j,k+1) aux mailles i-1 et i+1
              double We_mi = ( vitesse_k(i-1,j,k) + vitesse_k(i-1,j,k+1) )*0.5;
              double We_pi = ( vitesse_k(i+1,j,k) + vitesse_k(i+1,j,k+1) )*0.5;
              dwdx(i,j,k) = (We_pi  - We_mi ) / (2*dx);

              // ******************************** //
              // derivation direction y
              // de Uy
              dvdy(i,j,k) = (vitesse_j(i, j+1, k) - vitesse_j(i,j,k))/dy;

              if (compute_all)
                {
                  double Ue_mj = ( vitesse_i(i,j-1,k) + vitesse_i(i+1,j-1,k) )*0.5;
                  double Ue_pj = ( vitesse_i(i,j+1,k) + vitesse_i(i+1,j+1,k) )*0.5;
                  dudy(i,j,k) = (Ue_pj - Ue_mj)/(2*dy);
                  double Ve_mi = ( vitesse_j(i-1,j,k) + vitesse_j(i-1,j+1,k) )*0.5;
                  double Ve_pi = ( vitesse_j(i+1,j,k) + vitesse_j(i+1,j+1,k) )*0.5;
                  dvdx(i,j,k) = (Ve_pi  - Ve_mi ) / (2*dx);
                  double We_mj = ( vitesse_k(i,j-1,k) + vitesse_k(i,j-1,k+1) )*0.5;
                  double We_pj = ( vitesse_k(i,j+1,k) + vitesse_k(i,j+1,k+1) )*0.5;
                  dwdy(i,j,k) = (We_pj - We_mj)/(2*dy);
                }

              // ******************************** //
              // derivation direction z
              // Si on est sur un bord, on utilise l'info que la vitesse y est nulle.
              // Cette info est a dz/2 donc on utilise la formule centree d'ordre 2 pour pas variable :
              // Formule centree (ordre 2) pour pas variable dans le domaine :
              // grad[1:-1] = (h1/h2*u_pl - h2/h1*u_m + (h2**2-h1**2)/(h1*h2)*u_c) / (h1+h2)
              //
              if (on_the_first_cell && !(geom.get_periodic_flag(DIRECTION_K)))
                {
                  // de Ux
                  double Ue_mk = 0.;
                  double Ue_ck = ( vitesse_i(i,j,k  ) + vitesse_i(i+1,j,k  ) )*0.5;
                  double Ue_pk = ( vitesse_i(i,j,k+1) + vitesse_i(i+1,j,k+1) )*0.5;
                  // Formule decentree (ordre 2) pour pas variable sur le bord gauche :
                  dudz(i,j,k) = (- 4*Ue_mk + 3*Ue_ck +Ue_pk  ) / (3*dz);

                  // de Uy
                  double Ve_mk = 0.;
                  double Ve_ck = ( vitesse_j(i,j,k  ) + vitesse_j(i,j+1,k  ) )*0.5;
                  double Ve_pk = ( vitesse_j(i,j,k+1) + vitesse_j(i,j+1,k+1) )*0.5;
                  dvdz(i,j,k) = (- 4*Ve_mk + 3*Ve_ck +Ve_pk  ) / (3*dz) ;
                }
              else if (on_the_last_cell && !(geom.get_periodic_flag(DIRECTION_K)))
                {
                  // de Ux
                  double Ue_mk = ( vitesse_i(i,j,k-1) + vitesse_i(i+1,j,k-1) )*0.5;
                  double Ue_ck = ( vitesse_i(i,j,k  ) + vitesse_i(i+1,j,k  ) )*0.5;
                  double Ue_pk = 0.;
                  // Formule decentree (ordre 2) pour pas variable sur le bord droit :
                  dudz(i,j,k) = (- Ue_mk - 3*Ue_ck +4*Ue_pk ) / (3*dz);

                  // de Uy
                  double Ve_mk = ( vitesse_j(i,j,k-1) + vitesse_j(i,j+1,k-1) )*0.5;
                  double Ve_ck = ( vitesse_j(i,j,k  ) + vitesse_j(i,j+1,k  ) )*0.5;
                  double Ve_pk = 0.;
                  dvdz(i,j,k) = (- Ve_mk - 3*Ve_ck +4*Ve_pk ) / (3*dz) ;
                }
              else
                {
                  // For any interior cell... Formule centree pour pas cste

                  // de Ux !!!!!! ATTENTION on veux calculer la moyenne entre (i,j,k) et (i+1,j,k) aux mailles k-1 et k+1
                  double Ue_mk = ( vitesse_i(i,j,k-1) + vitesse_i(i+1,j,k-1) )*0.5;
                  double Ue_pk = ( vitesse_i(i,j,k+1) + vitesse_i(i+1,j,k+1) )*0.5;
                  dudz(i,j,k) = (Ue_pk  - Ue_mk ) / (2*dz);

                  // de Uy !!!!!! ATTENTION on veux calculer la moyenne entre (i,j,k) et (i,j+1,k) aux mailles k-1 et k+1
                  double Ve_mk = ( vitesse_j(i,j,k-1) + vitesse_j(i,j+1,k-1) )*0.5;
                  double Ve_pk = ( vitesse_j(i,j,k+1) + vitesse_j(i,j+1,k+1) )*0.5;
                  dvdz(i,j,k) = (Ve_pk  - Ve_mk ) / (2*dz) ;
                }

              // de Uz
              dwdz(i,j,k) = (vitesse_k(i, j, k+1) - vitesse_k(i,j,k))/dz;

              // Calcul de lambda2 :
              if (compute_all)
                {
                  /*
                              const double a11=dudx(i,j,k);
                              const double a12=dudy(i,j,k);
                              const double a13=dudz(i,j,k);
                              const double a21=dvdx(i,j,k);
                              const double a22=dvdy(i,j,k);
                              const double a23=dvdz(i,j,k);
                              const double a31=dwdx(i,j,k);
                              const double a32=dwdy(i,j,k);
                              const double a33=dwdz(i,j,k);
                  */

                  // Sij = 1/2*(aij+aji)
                  double s11=dudx(i,j,k);
                  double s12=(dudy(i,j,k) + dvdx(i,j,k))*0.5;
                  double s13=(dudz(i,j,k) + dwdx(i,j,k))*0.5;
                  double s21=s12;
                  double s22=dvdy(i,j,k);
                  double s23=(dvdz(i,j,k) + dwdy(i,j,k))*0.5;
                  double s31=s13;
                  double s32=s23;
                  double s33=dwdz(i,j,k);
                  squared_3x3(s11, s12, s13, s21, s22, s23, s31, s32, s33);

                  // Omega_ij = 1/2*(aij-aji)
                  double o11=0.;
                  double o12=(dudy(i,j,k) - dvdx(i,j,k))*0.5;
                  double o13=(dudz(i,j,k) - dwdx(i,j,k))*0.5;
                  double o21=-o12;
                  double o22=0.;
                  double o23=(dvdz(i,j,k) - dwdy(i,j,k))*0.5;
                  double o31=-o13;
                  double o32=-o23;
                  double o33=0.;
                  squared_3x3(o11, o12, o13, o21, o22, o23, o31, o32, o33);

                  //
                  const double a11=s11+o11;
                  const double a12=s12+o12;
                  const double a13=s13+o13;
                  const double a21=s21+o21;
                  const double a22=s22+o22;
                  const double a23=s23+o23;
                  const double a31=s31+o31;
                  const double a32=s32+o32;
                  const double a33=s33+o33;


                  //double a11=1,a22=2,a33=3,a12=0,a21=0,a13=0,a31=0,a23=0,a32=0;
                  // Changement de tous les signes :
                  const double a = 1.;
                  const double b = -(a11+a22+a33);
                  const double c = -(a12*a21+a23*a32+a13*a31-a11*a22-a11*a33-a22*a33);
                  const double d = -(a11*a22*a33+a12*a23*a31+a13*a32*a21-a11*a23*a32-a22*a13*a31-a33*a12*a21);

                  const double delta = 18*a*b*c*d-4*b*b*b*d+b*b*c*c-4*a*c*c*c-27*a*a*d*d;
                  const double Q = (3.*c-b*b)/9.;
                  const double R = (9.*b*c - 27.*d - 2.*b*b*b)/54.;
                  const double D = Q*Q*Q + R*R;
                  if ((delta<0) || (Q>0))
                    {
                      double Re_sqrtD, Im_sqrtD;
                      double mod=0, arg=0;
                      if (D>0)
                        {
                          Cerr << "Singularity for lambda2 at " << i << " " << j << " " << k << " D=" << D << finl;
                          // only one real value :
                          Re_sqrtD = sqrt(D);
                          Im_sqrtD = 0.;
                          complex_to_trig(R+Re_sqrtD, Im_sqrtD, mod, arg);
                          // Puissance 1/3 :
                          const double modS = pow(mod, 1./3.);
                          const double argS = arg /3.;
                          const double Re_S = modS*cos(argS);
                          const double Im_S = modS*sin(argS);
                          //
                          complex_to_trig(R-Re_sqrtD, -Im_sqrtD, mod, arg);
                          // Puissance 1/3 :
                          const double modT = pow(mod, 1./3.);
                          const double argT = arg /3.;
                          const double Re_T = modT*cos(argT);
                          const double Im_T = modT*sin(argT);

                          const double Re_B = Re_S+Re_T;
                          const double Im_B = Im_S+Im_T;
                          const double Re_A = Re_S-Re_T;
                          const double Im_A = Im_S-Im_T;
                          Cerr << "Im_B : " << Im_B << " Re_A : " << Re_A  << finl;
                          const double z1 = -1./3.*b+Re_B;
                          const double z2 = -1./3.*b-0.5*Re_B+1./2.*sqrt(3.)*Im_A;
                          const double z3 = -1./3.*b-0.5*Re_B-1./2.*sqrt(3.)*Im_A;
                          Cerr << " z1,2,3 = " << z1 << " " << z2 << " " << z3 << finl;
                          Cerr << "Im_z1,2,3 = " << Im_B << " " << -0.5*Re_B+sqrt(3)/2.*Re_A << " " << -0.5*Re_B-sqrt(3)/2.*Re_A << finl;
                          double x = z1+z2+z3 - std::min(z1,std::min(z2,z3)) - std::max(z1,std::max(z2,z3));
                          lambda2(i,j,k) = x;
                          // Check that lambda2 is root :
                          x = z1;
                          Cerr << "x= " << x << " results in: " << a*x*x*x + b*x*x + c*x + d << finl;
                          x = z2;
                          Cerr << "x= " << x << " results in: " << a*x*x*x + b*x*x + c*x + d << finl;
                          x = z3;
                          Cerr << "x= " << x << " results in: " << a*x*x*x + b*x*x + c*x + d << finl;


                        }
                      else
                        {
                          lambda2(i,j,k) = -1000.;
                          Cerr << "Singularity for lambda2 at " << i << " " << j << " " << k << " value prescribed : -1000. " ;
                          Cerr << "delta = " << delta << " ; Q = " << Q << finl;
                          Cerr << "(R*sqrt(pow(fabs(Q),-3)))= " <<  (R*sqrt(pow(fabs(Q),-3))) << finl;
                        }
                    }
                  else
                    {
                      // Cerr << "D : " << D << finl;
                      // If computation from scratch, Q=0 at t=0
                      const double theta=acos(R*sqrt(-pow(Q-DMINFLOAT,-3)));
                      //Cerr << "theta : " << theta << finl;
                      const double z1 = 2*sqrt(-Q)*cos(theta/3.)-1./3.*b;
                      const double z2 = 2*sqrt(-Q)*cos((theta+2*M_PI)/3.)-1./3.*b;
                      const double z3 = 2*sqrt(-Q)*cos((theta+4*M_PI)/3.)-1./3.*b;
                      //Cerr << " z1,2,3 = " << z1 << " " << z2 << " " << z3 << finl;
                      /*
                                  double Re_sqrtD, Im_sqrtD;
                                  if (D<0)
                                    {
                                      Re_sqrtD = 0.;
                                      Im_sqrtD = sqrt(-D);
                                    }
                                  else
                                    {
                                      Re_sqrtD = sqrt(D);
                                      Im_sqrtD = 0.;
                                    }
                                  double mod=0, arg=0;
                                  complex_to_trig(R+Re_sqrtD, Im_sqrtD, mod, arg);
                                  // Puissance 1/3 :
                                  const double modS = pow(mod, 1./3.);
                                  const double argS = arg /3.;
                                  const double Re_S = modS*cos(argS);
                                  const double Im_S = modS*sin(argS);
                                  //
                                  complex_to_trig(R-Re_sqrtD, -Im_sqrtD, mod, arg);
                                  // Puissance 1/3 :
                                  const double modT = pow(mod, 1./3.);
                                  const double argT = arg /3.;
                                  const double Re_T = modT*cos(argT);
                                  const double Im_T = modT*sin(argT);

                                  const double Re_B = Re_S+Re_T;
                                  const double Im_B = Im_S+Im_T;
                                  const double Re_A = Re_S-Re_T;
                                  const double Im_A = Im_S-Im_T;
                                  Cerr << "Im_B : " << Im_B << " Re_A : " << Re_A  << finl;
                                  const double zz1 = -1./3.*b+Re_B;
                                  const double zz2 = -1./3.*b-0.5*Re_B+1./2.*sqrt(3.)*Im_A;
                                  const double zz3 = -1./3.*b-0.5*Re_B-1./2.*sqrt(3.)*Im_A;
                                  Cerr << " zz1,2,3 = " << zz1 << " " << zz2 << " " << zz3 << finl;

                                  //const double delta0 = b*b-3*a*c;
                                  //const double delta1 = 2*b*b*b-9*a*b*c+27*a*a*d;
                                  // Cerr << delta1*delta1-4*delta0*delta0*delta0 << " " << -27.*a*a*delta << finl;
                                  // Apres le premier delta1, WIKI dit +- choix libre ?!
                                  // WIKI : const double C = std::cbrt((delta1+sqrt(delta1*delta1-4*delta0*delta0*delta0))/2.);
                                  //                      const double C = std::cbrt((delta1+sqrt(-delta1*delta1+4*delta0*delta0*delta0))/2.);
                                  //const double C = cbrt((delta1+sqrt(-delta1*delta1+4*delta0*delta0*delta0))/2.);
                                  //const double r = -1./(3.*a)*(b+C+delta0/C);
                                  //
                                  // Other roots :
                                  //const double dprime = b*b-4*a*c-2*a*b*r-3*a*a*r*r;
                                  // WIKI : const double r1 = (-b-r*a+sqrt(dprime))/(2.*a);
                                  // WIKI : const double r2 = (-b-r*a-sqrt(dprime))/(2.*a);
                                  //const double r1 = (-b-r*a+sqrt(-dprime))/(2.*a);
                                  //const double r2 = (-b-r*a-sqrt(-dprime))/(2.*a);
                                  // Cerr << r << " " << r1 << " " << r2 << finl;
                                  // Sort it out :
                      */
                      //DoubleVect roots(3);
                      //roots[0] = z1;
                      //roots[1] = z2;
                      //roots[2] = z3;
                      //ArrOfInt cc(3);
                      //trier(roots,cc); // du + grand au plus petit...
                      //Cerr << roots;
                      //Cerr << "Sorted roots : " << roots[0] << " " << roots[1] << " " << roots[2] << finl;
                      const double x     = z1+z2+z3 - std::min(z1,std::min(z2,z3)) - std::max(z1,std::max(z2,z3));
                      lambda2(i,j,k)	 = x;
                      // Check that lambda2 is root :
                      const double rr	 = a*x*x*x + b*x*x + c*x + d ;
                      //const double tol = 0.01;
                      //if ((rr>tol)||(rr<-tol))
                      //  {
                      //    Cerr << "theta : " << theta << finl;
                      //    Cerr << " z1,2,3 = " << z1 << " " << z2 << " " << z3 << finl;
                      //    //Cerr << "Sorted roots : " << roots[0] << " " << roots[1] << " " << roots[2] << finl;
                      //    Cerr << "x= " << x << " results in: " << rr << finl;
                      //  }
                      residue = std::max(residue,fabs(rr));
                    }
                }
            }
        }
    }

  Cerr << "Maximal residue encountered with lambda2 : " << residue << finl;
  // Mise a jour des espaces virtuels :
  dudx.echange_espace_virtuel(dudx.ghost());
  dvdy.echange_espace_virtuel(dvdy.ghost());
  dwdx.echange_espace_virtuel(dwdx.ghost());
  dudz.echange_espace_virtuel(dudz.ghost());
  dvdz.echange_espace_virtuel(dvdz.ghost());
  dwdz.echange_espace_virtuel(dwdz.ghost());

  return;
}


void complex_to_trig(const double re, const double im,
                     double& modul, double& arg)
{
  modul = sqrt(re*re+im*im);
  if (re>0)
    {
      arg = atan(im/re);
    }
  else if (re==0)
    {
      if (im>0)
        arg = M_PI/2.;
      else
        arg = -M_PI/2.;
      Cerr << "watchout!" << finl;
    }
  else
    {
      arg = M_PI + atan(im/re);
    }
  return ;
}

void squared_3x3(double& a11, double& a12, double& a13,
                 double& a21, double& a22, double& a23,
                 double& a31, double& a32, double& a33)
{
  double aa11 = a11*a11 + a12*a21 + a13*a31;
  double aa12 = a11*a12 + a12*a22 + a13*a32;
  double aa13 = a11*a13 + a12*a23 + a13*a33;
  double aa21 = a21*a11 + a22*a21 + a23*a31;
  double aa22 = a21*a12 + a22*a22 + a23*a32;
  double aa23 = a21*a13 + a22*a23 + a23*a33;
  double aa31 = a31*a11 + a32*a21 + a33*a31;
  double aa32 = a31*a12 + a32*a22 + a33*a32;
  double aa33 = a31*a13 + a32*a23 + a33*a33;

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


void supprimer_chevauchement(IJK_Field_double& ind)
{

  const int ni = ind.ni();
  const int nj = ind.nj();
  const int nk = ind.nk();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              if(ind(i,j,k)!=0.0)
                {
                  ind(i,j,k) = 1.0;

                }
            }
        }
    }

  return;

}


void update_integral_pressure(const IJK_Field_double& p_instant,  IJK_Field_double& p_tmp, const IJK_Field_double& indic, const IJK_Field_double& indic_tmp)
{
  const int ni = p_instant.ni();
  const int nj = p_instant.nj();
  const int nk = p_instant.nk();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              if(indic_tmp(i,j,k)+indic(i,j,k)!=0.0 and indic(i,j,k)==1.0)
                {
                  p_tmp(i,j,k) = (p_tmp(i,j,k)*indic_tmp(i,j,k)+p_instant(i,j,k)*indic(i,j,k))/(indic_tmp(i,j,k)+indic(i,j,k));

                }
            }
        }
    }
  p_tmp.echange_espace_virtuel(p_tmp.ghost());
  return;
}

// out : the cumulative integral
void update_integral_indicatrice(const IJK_Field_double& indic, const double deltat, IJK_Field_double& out)
{
  const int ni = out.ni();
  const int nj = out.nj();
  const int nk = out.nk();
  for (int k = 0; k < nk; k++)
    for (int j = 0; j < nj; j++)
      for (int i = 0; i < ni; i++)
        out(i,j,k) += indic(i,j,k)*deltat;
  return;
}

double calculer_v_moyen(const IJK_Field_double& vx)
{
  const IJK_Splitting& splitting = vx.get_splitting();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  const int ni = vx.ni();
  const int nj = vx.nj();
  const int nk = vx.nk();
  double v_moy = 0.;
#ifndef VARIABLE_DZ
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              v_moy += vx(i,j,k);
            }
        }
    }
  // somme sur tous les processeurs.
  v_moy = Process::mp_sum(v_moy);
  // Maillage uniforme, il suffit donc de diviser par le nombre total de mailles:
  // cast en double au cas ou on voudrait faire un maillage >2 milliards
  const double n_mailles_tot = ((double) geom.get_nb_elem_tot(0)) * geom.get_nb_elem_tot(1) * geom.get_nb_elem_tot(2);
  v_moy /= n_mailles_tot;
#else
  const int offset = splitting.get_offset_local(DIRECTION_K);
  const ArrOfDouble& tab_dz=geom.get_delta(DIRECTION_K);
  for (int k = 0; k < nk; k++)
    {
      const double dz = tab_dz[k+offset];
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              v_moy += vx(i,j,k)*dz;
            }
        }
    }
  // somme sur tous les processeurs.
  v_moy = Process::mp_sum(v_moy);
  // Maillage uniforme, il suffit donc de diviser par le nombre total de mailles:
  // cast en double au cas ou on voudrait faire un maillage >2 milliards
  const double n_mailles_xy = ((double) geom.get_nb_elem_tot(0)) * geom.get_nb_elem_tot(1);
  v_moy /= (n_mailles_xy * geom.get_domain_length(DIRECTION_K) );
#endif
  return v_moy;
}

double calculer_rho_cp_u_moyen(const IJK_Field_double& vx,const IJK_Field_double& cp,const IJK_Field_double& rho_field )
{

  const int ni = vx.ni();
  const int nj = vx.nj();
  const int nk = vx.nk();
  double rho_cp_u_moy = 0.;

  for (int k = 0; k < nk; k++)
    for (int j = 0; j < nj; j++)
      for (int i = 0; i < ni; i++)
        {
          const double rho = rho_field(i,j,k);
          const double c = cp(i,j,k);
          const double u = (vx(i,j,k)+vx(i+1,j,k))/2;
          const double rho_cp_u = rho*c*u;
          rho_cp_u_moy += rho_cp_u;
        }
  // somme sur tous les processeurs.
  rho_cp_u_moy = Process::mp_sum(rho_cp_u_moy);
  // Maillage uniforme, il suffit donc de diviser par le nombre total de mailles:
  // cast en double au cas ou on voudrait faire un maillage >2 milliards
  const IJK_Splitting& splitting = vx.get_splitting();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  const double n_mailles_tot = ((double) geom.get_nb_elem_tot(0)) * geom.get_nb_elem_tot(1) * geom.get_nb_elem_tot(2);
  rho_cp_u_moy /= n_mailles_tot;
  return rho_cp_u_moy;
}
double calculer_temperature_adimensionnelle_theta_moy(const IJK_Field_double& vx, const IJK_Field_double& temperature_adimensionnelle_theta,
                                                      const IJK_Field_double& cp, const IJK_Field_double& rho_field)
{
  const IJK_Splitting& splitting = temperature_adimensionnelle_theta.get_splitting();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  double theta_adim_moy = 0;
  double rho_cp_u_moy =0;

  const int nk = temperature_adimensionnelle_theta.nk();
  const int ni = temperature_adimensionnelle_theta.ni();
  const int nj = temperature_adimensionnelle_theta.nj();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              const double theta_adim = temperature_adimensionnelle_theta(i,j,k);
              const double rho = rho_field(i,j,k);
              const double c = cp(i,j,k);
              const double u = (vx(i,j,k)+vx(i+1,j,k))/2.;
              rho_cp_u_moy += rho*c*u;
              theta_adim_moy += rho*c*u*theta_adim;
            }
        }
    }
  //somme sur les proc
  rho_cp_u_moy = Process::mp_sum(rho_cp_u_moy);
  theta_adim_moy = Process::mp_sum(theta_adim_moy);
  //Division par le nombre de mailles
  const double n_mailles_tot = ((double) geom.get_nb_elem_tot(0)) * geom.get_nb_elem_tot(1) * geom.get_nb_elem_tot(2);
  rho_cp_u_moy /= n_mailles_tot;
  theta_adim_moy /= n_mailles_tot;
  //valeur adimensionnelle moyenne
  theta_adim_moy /= rho_cp_u_moy;
  return theta_adim_moy;
}

double calculer_temperature_theta_moy(const IJK_Field_double& vx, const IJK_Field_double& temperature,
                                      const IJK_Field_double& cp, const IJK_Field_double& rho_field)
{
  const IJK_Splitting& splitting = temperature.get_splitting();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  double theta_moy = 0;
  double rho_cp_u_moy =0;

  const int nk = temperature.nk();
  const int ni = temperature.ni();
  const int nj = temperature.nj();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              const double theta = temperature(i,j,k);
              const double rho = rho_field(i,j,k);
              const double c = cp(i,j,k);
              const double u = (vx(i,j,k)+vx(i+1,j,k))/2.;
              rho_cp_u_moy += rho*c*u;
              theta_moy += rho*c*u*theta;
            }
        }
    }
  //somme sur les proc
  rho_cp_u_moy = Process::mp_sum(rho_cp_u_moy);
  theta_moy = Process::mp_sum(theta_moy);
  //Division par le nombre de mailles
  const double n_mailles_tot = ((double) geom.get_nb_elem_tot(0)) * geom.get_nb_elem_tot(1) * geom.get_nb_elem_tot(2);
  rho_cp_u_moy /= n_mailles_tot;
  theta_moy /= n_mailles_tot;
  //valeur adimensionnelle moyenne
  theta_moy /= rho_cp_u_moy;
  return theta_moy;
}
double calculer_variable_wall(const IJK_Field_double& variable, const IJK_Field_double& cp, const IJK_Field_double& rho_field, const int kmin, const int kmax)
{
  const IJK_Splitting& splitting = variable.get_splitting();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  double variable_moy = 0;
  double rho_cp_moy =0;

  const int ni = variable.ni();
  const int nj = variable.nj();
  const int nk = variable.nk();

  if (kmin == 0)
    {
      // int k=0;
      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni; i++)
          {
            const double var = variable(i,j,kmin);
            const double rho = rho_field(i,j,kmin);
            const double c = cp(i,j,kmin);
            rho_cp_moy += rho*c;
            variable_moy += rho*c*var;
          }
    }


  if(kmin + variable.nk() == kmax)
    {
      int k = nk-1;
      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni; i++)
          {
            const double var = variable(i,j,k);
            const double rho = rho_field(i,j,k);
            const double c = cp(i,j,k);
            //const double u = (vx(i,j,k)+vx(i+1,j,k))/2.;
            rho_cp_moy += rho*c;
            variable_moy += rho*c*var;
          }
    }


//somme sur les proc
  rho_cp_moy = Process::mp_sum(rho_cp_moy);
  variable_moy = Process::mp_sum(variable_moy);
  //Division par le nombre de mailles sur les 2 plans de bords
  const double n_mailles_plan_xy_tot = 2.*((double) geom.get_nb_elem_tot(0)) * geom.get_nb_elem_tot(1) ;
  rho_cp_moy /= n_mailles_plan_xy_tot;
  variable_moy /= n_mailles_plan_xy_tot;
  //valeur adimensionnelle moyenne
  variable_moy /= rho_cp_moy;
  return variable_moy;
}

// Add to vx, vy and vz the gradient of the pressure gradient multiplied by the constant.
// Input and output velocity is in m/s (not the integral of the momentum on the cell).
// On the walls, don't touch velocity
void add_gradient_temperature(const IJK_Field_double& temperature, const double constant,
                              IJK_Field_double& vx, IJK_Field_double& vy, IJK_Field_double& vz, const Boundary_Conditions_Thermique& boundary, const IJK_Field_double& lambda )
{
  const IJK_Grid_Geometry& geom = vx.get_splitting().get_grid_geometry();
  const int kmax = std::max(std::max(vx.nk(), vy.nk()), vz.nk());
  for (int k = 0; k < kmax; k++)
    {
      // i component of velocity
      if (k < vx.nk())
        {
          const int jmax = vx.nj();
          const int imax = vx.ni();
          const double f = constant / geom.get_constant_delta(0);
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vx(i,j,k) += ( temperature(i,j,k) -  temperature(i-1,j,k)) * f;
        }
      // j component:
      if (k < vy.nk())
        {
          const int jmax = vy.nj();
          const int imax = vy.ni();
          const double f = constant / geom.get_constant_delta(1);
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vy(i,j,k) += ( temperature(i,j,k) -  temperature(i,j-1,k)) * f;
        }
      // k component:
      bool on_the_wall = false;
      bool on_the_first_cell = false;
      bool on_the_last_cell = false;
      int bctype_kmin = boundary.get_bctype_k_min();
      int bctype_kmax = boundary.get_bctype_k_max();


      const int k_min = vz.get_splitting().get_offset_local(DIRECTION_K);
      const int nk_tot = vz.get_splitting().get_nb_items_global(IJK_Splitting::FACES_K, DIRECTION_K);
      const int offset = vz.get_splitting().get_offset_local(DIRECTION_K);
      const ArrOfDouble& delta_z_all = geom.get_delta(DIRECTION_K);
      bool perio_k=vz.get_splitting().get_grid_geometry().get_periodic_flag(DIRECTION_K);
      if ((k + k_min == 0 || k + k_min == nk_tot-1) && (!perio_k))
        on_the_wall = true;


      if (k < vz.nk() && (!on_the_wall))
        {
          const int jmax = vz.nj();
          const int imax = vz.ni();
          double f;
          if(!perio_k)
            {
              f = constant * 2. / (delta_z_all[k-1+k_min] + delta_z_all[k+k_min]);
            }
          else
            {
              f = (constant /  delta_z_all[k+offset]);
            }
          for (int j = 0; j < jmax; j++)
            for (int i = 0; i < imax; i++)
              vz(i,j,k) += ( temperature(i,j,k) -  temperature(i,j,k-1)) * f;
        }
      else if (k < vz.nk() && (on_the_wall))
        {
          if (k + k_min == 0)
            on_the_first_cell = true;
          if (k + k_min == nk_tot-1)
            on_the_last_cell = true;

          if(on_the_first_cell)
            {
              const int jmax = vz.nj();
              const int imax = vz.ni();
              double f;
              if(bctype_kmin == 0)
                {
                  f = constant *  2. / delta_z_all[k+offset];
                  // schema decentre prenant en compte le bord et les trois centres de cellules suivants
                  // pour calculer le gradient a la demi-longueur
                  //const double coef = 1./48.;

                  const double  temperature_kmin = boundary.get_temperature_kmin();
                  for (int j = 0; j < jmax; j++)
                    for (int i = 0; i < imax; i++)
                      vz(i,j,0) +=  (temperature(i,j,0) - temperature_kmin)*f;
                  //  vz(i,j,0) += (- (23.* temperature_kmin) + (21. * temperature(i,j,k)) + (3.* temperature(i,j,k+1)) - (1. * temperature(i,j,k+2)) ) * f * coef;
                  // vz(i,j,0) += ( 23.*temperature_kmin - 21.*temperature(i,j,k) - 3.*temperature(i,j,k+1) + 1.*temperature(i,j,k+2)) * f *coef;

                }

              if(bctype_kmin == 1)
                {
                  const double  flux_kmin = boundary.get_flux_kmin();
                  for (int j = 0; j < jmax; j++)
                    for (int i = 0; i < imax; i++)
                      {
                        double l = lambda(i,j,0);
                        vz(i,j,0) += -flux_kmin/l;
                      }


                }
            }

          if(on_the_last_cell)
            {
              const int jmax = vz.nj();
              const int imax = vz.ni();
              double f;

              if(bctype_kmax == 0)
                {
                  f = constant  *  2. / delta_z_all[k-1+offset];

                  Cerr << "IJK_Naviers_Stokes" << " " << "erreur_dans_le_calcul_du_gradient_de_T" << finl;

                  const double temperature_kmax = boundary.get_temperature_kmax();
                  //const double coef = 1./48.;
                  for (int j = 0; j < jmax; j++)
                    for (int i = 0; i < imax; i++)
                      vz(i,j,k) += (temperature_kmax -temperature(i,j,k-1))*f;
                  //vz(i,j,k) += ( 23.*temperature_kmax - 21.*temperature(i,j,k-1) - 3.*temperature(i,j,k-2) + 1.*temperature(i,j,k-3)) * f *coef;
                  // vz(i,j,k) += (-23.*temperature_kmax + 21.*temperature(i,j,k-1) + 3.*temperature(i,j,k-2)-1.*temperature(i,j,k+2) ) * f * coef;

                }

              if(bctype_kmax == 1)
                {
                  const double flux_kmax = boundary.get_flux_kmax();
                  for (int j = 0; j < jmax; j++)
                    for (int i = 0; i < imax; i++)
                      {
                        double l = lambda(i,j,k-1);
                        vz(i,j,k) += flux_kmax/l;
                      }


                }
            }
        }


    }
}



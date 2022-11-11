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

#include <Parallel_io_parameters.h>
#include <Param.h>
#include <IJK_Lata_writer.h>
#include <Interprete_bloc.h>
#include <Statistiques.h>
#include <IJK_Navier_Stokes_tools.h>

Implemente_instanciable(Parallel_io_parameters, "Parallel_io_parameters", Interprete);

// Reasonable default value for typical machines nowadays
long long Parallel_io_parameters::max_block_size_ = (long long) 0; // for the moment, deactivate parallel write by default 1024*1024*64;

// 0 means that the number of writing processes will be guessed depending on the
// number of compute processors
int Parallel_io_parameters::nb_writing_processes_ = 0;


Sortie& Parallel_io_parameters::printOn(Sortie& os) const
{
  Objet_U::printOn(os);
  return os;
}

Entree& Parallel_io_parameters::readOn(Entree& is)
{
  return is;
}

Entree& Parallel_io_parameters::interpreter(Entree& is)
{
  int bs_bytes = -1;
  int bs = (int) (max_block_size_ >> 20);
  int n  = nb_writing_processes_;
  Nom ijk_name_write, ijk_name_read;

  Param param(que_suis_je());
  param.ajouter("block_size_bytes", &bs_bytes);
  param.ajouter("block_size_megabytes", &bs);
  param.ajouter("writing_processes", &n);
  param.ajouter("bench_ijk_splitting_write", &ijk_name_write);
  param.ajouter("bench_ijk_splitting_read", &ijk_name_read);
  param.lire_avec_accolades(is);

  if (bs < 0)
    {
      Cerr << "Error in Parallel_io_parameters::interpreter: block_size is negative" << finl;
      Process::exit();
    }
  if (n < 0)
    n = 0;

  max_block_size_ = (long long) bs * 1024 * 1024;
  if (bs_bytes >= 0)
    max_block_size_ = (long long) bs_bytes; // A buffer smaller than 1MB is usefull only for debugging...

  nb_writing_processes_ = n;

  Cerr << "Parallel_io_parameters: blocksize= " << bs << " MB, nb writing processes= "
       << get_nb_writing_processes() << finl;

  if (ijk_name_write != "??")
    {
      run_bench_write(ijk_name_write);
    }
  if (ijk_name_read != "??")
    {
      run_bench_read(ijk_name_read);
    }
  return is;
}

long long Parallel_io_parameters::get_max_block_size()
{
  return max_block_size_;
}

int Parallel_io_parameters::get_nb_writing_processes()
{
  if (nb_writing_processes_ <= 0)
    {
      // Determine automatically:
      // Reasonable default value for the number of writing processes (one per node, but no more than 16)
      // Assume 24 cores per node...
      int n = Process::nproc() / 24;
      if (n > 16)
        n = 16;
      if (n == 0)
        n = 1;
      return n;
    }
  return nb_writing_processes_;
}

void Parallel_io_parameters::run_bench_write(const Nom& ijk_splitting_name)
{
  // Get the mesh:
  const IJK_Splitting& splitting = ref_cast(IJK_Splitting, Interprete_bloc::objet_global(ijk_splitting_name));
  // Build a velocity field and a scalar field:
  IJK_Field_double vx, vy, vz;
  vx.allocate(splitting, IJK_Splitting::FACES_I,0);
  vy.allocate(splitting, IJK_Splitting::FACES_J,0);
  vz.allocate(splitting, IJK_Splitting::FACES_K,0);

  set_field_data(vx,Nom("x*0.9+y*0.09*0.001+z*0.009"));
  set_field_data(vy,Nom("1.+x*0.9+y*0.09+z*0.009"));
  set_field_data(vz,Nom("-1.+x*0.9+y*0.09+z*0.009"));

  dumplata_header("test.lata", vx);
  dumplata_newtime("test.lata", 0.);

  statistiques().set_three_first_steps_elapsed(false);
  static Stat_Counter_Id cnt = statistiques().new_counter(1, "Parallel_io benchmark");
  statistiques().begin_count(cnt);
  dumplata_vector("test.lata", "VELOCITY", vx, vy, vz, 1);
  statistiques().end_count(cnt);
  double t = statistiques().last_time(cnt);
  double sz = (double) (splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_I)+1)
              * (splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_J)+1)
              * (splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_K)+1)
              * 3 * sizeof(float);
  Nom bw = (t==0)? Nom("infty") : Nom(sz/1024/1024/1024/t);
  Cerr << "Parallel_io_parameters benchmark write: data_size= " << sz/1024/1024/1024
       << " GB. Time= " << t << " s. Bandwidth= "
       << bw << " GB/s.(x3)" << finl;
}

double max_val_abs_ijk(const IJK_Field_double& residu,const IJK_Field_double& x)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  double m = 0.;
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              m = std::fmax(fabs(residu(i,j,k)-x(i,j,k)), m);
            }
        }
    }
  m = Process::mp_max(m);
  return m;
}


void Parallel_io_parameters::run_bench_read(const Nom& ijk_splitting_name)
{
  // Get the mesh:
  const IJK_Splitting& splitting = ref_cast(IJK_Splitting, Interprete_bloc::objet_global(ijk_splitting_name));
  // Build a velocity field and a scalar field:
  IJK_Field_double vx, vy, vz;
  vx.allocate(splitting, IJK_Splitting::FACES_I,0);
  vy.allocate(splitting, IJK_Splitting::FACES_J,0);
  vz.allocate(splitting, IJK_Splitting::FACES_K,0);

  vx.data() = 1e9;
  vy.data() = 1e9;
  vz.data() = 1e9;

  statistiques().set_three_first_steps_elapsed(false);
  static Stat_Counter_Id cnt = statistiques().new_counter(1, "Parallel_io benchmark_read");
  statistiques().begin_count(cnt);
  lire_dans_lata("test.lata", 1 /* timestep */,
                 splitting.get_grid_geometry().le_nom(),
                 "VELOCITY", vx, vy, vz);
  statistiques().end_count(cnt);

  double t = statistiques().last_time(cnt);
  double sz = (double) (splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_I)+1)
              * (splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_J)+1)
              * (splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_K)+1)
              * 3 * sizeof(float);
  Nom bw = (t==0)? Nom("infty") : Nom(sz/1024/1024/1024/t);
  Cerr << "Parallel_io_parameters benchmark read: data_size= " << sz/1024/1024/1024
       << " GB. Time= " << t << " s. Bandwidth= " << bw << " GB/s.(x3)" << finl;

  // Check values:
  IJK_Field_double vx2, vy2, vz2;
  vx2.allocate(splitting, IJK_Splitting::FACES_I,0);
  vy2.allocate(splitting, IJK_Splitting::FACES_J,0);
  vz2.allocate(splitting, IJK_Splitting::FACES_K,0);

  set_field_data(vx2,Nom("x*0.9+y*0.09*0.001+z*0.009"));
  set_field_data(vy2,Nom("1.+x*0.9+y*0.09+z*0.009"));
  set_field_data(vz2,Nom("-1.+x*0.9+y*0.09+z*0.009"));

  double delta;

  delta = max_val_abs_ijk(vx,vx2);
  Cerr << "L2 Norm of difference on vx: " << delta << finl;
  delta = max_val_abs_ijk(vy,vy2);
  Cerr << "L2 Norm of difference on vy: " << delta << finl;
  delta = max_val_abs_ijk(vz,vz2);
  Cerr << "L2 Norm of difference on vz: " << delta << finl;
}

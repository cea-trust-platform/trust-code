/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Multigrille_base.h>
#include <Param.h>
#include <TRUSTTab.h>
#include <Param.h>
#include <Statistiques.h>
#include <Schema_Comm_Vecteurs.h>
#include <communications.h>
#include <Matrice_Morse_Sym.h>
#include <IJK_discretization.h>
#include <stat_counters.h>
//#define DUMP_LATA_ALL_LEVELS
#ifdef DUMP_LATA_ALL_LEVELS
#include <IJK_lata_dump.h>
#include <IJK_Lata_writer.h>
#endif

// #define DUMP_X_B_AND_RESIDUE_IN_FILE
#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
#include <SFichier.h>
static int global_count_dump_in_file = 0;
#endif

Implemente_base_sans_constructeur(Multigrille_base, "Multigrille_base", SolveurSys_base);
// XD multigrid_solver interprete nul 1 Object defining a multigrid solver in IJK discretization
// XD   attr coarsen_operators coarsen_operators coarsen_operators 1 Definition of the number of grids that will be used, in addition to the finest (original) grid, followed by the list of the coarsen operators that will be applied to get those grids
// XD   attr ghost_size entier ghost_size 1 Number of ghost cells known by each processor in each of the three directions

Sortie& Multigrille_base::printOn(Sortie& os) const { return os; }

void Multigrille_base::ajouter_param(Param& param)
{
  param.ajouter("relax_jacobi", &relax_jacobi_); // XD_ADD_P list Parameter between 0 and 1 that will be used in the Jacobi method to solve equation on each grid. Should be around 0.7
  param.ajouter("pre_smooth_steps", &pre_smooth_steps_); // XD_ADD_P listentier First integer of the list indicates the numbers of integers that has to be read next. Following integers define the numbers of iterations done before solving the equation on each grid. For example, 2 7 8 means that we have a list of 2 integers, the first one tells us to perform 7 pre-smooth steps on the first grid, the second one tells us to perform 8 pre-smooth steps on the second grid. If there are more than 2 grids in the solver, then the remaining ones will have as many pre-smooth steps as the last mentionned number (here, 8)
  param.ajouter("smooth_steps", &smooth_steps_); // XD_ADD_P listentier First integer of the list indicates the numbers of integers that has to be read next. Following integers define the numbers of iterations done after solving the equation on each grid. Same behavior as pre_smooth_steps
  param.ajouter("nb_full_mg_steps", &nb_full_mg_steps_); // XD_ADD_P listentier Number of multigrid iterations at each level
  param.ajouter("solveur_grossier", &solveur_grossier_); // XD_ADD_P solveur_sys_base Name of the iterative solver that will be used to solve the system on the coarsest grid. This resolution must be more precise than the ones occurring on the fine grids. The threshold of this solver must therefore be lower than seuil defined above.
  param.ajouter("check_residu", &check_residu_);
  param.ajouter("seuil", &seuil_); // XD_ADD_P floattant Define an upper bound on the norm of the final residue (i.e. the one obtained after applying the multigrid solver). With hybrid precision, as long as we have not obtained a residue whose norm is lower than the imposed threshold, we keep applying the solver
  param.ajouter("iterations_gcp", &max_iter_gcp_);
  param.ajouter("iterations_gmres", &max_iter_gmres_);
  param.ajouter("solv_jacobi", &solv_jacobi_);
  param.ajouter("n_krilov", &n_krilov_);
  param.ajouter_flag("impr", &impr_); // XD_ADD_P rien Flag to display some info on the resolution on eahc grid
  param.ajouter("solver_precision", &solver_precision_); // XD_ADD_P chaine(into=["mixed","double"]) Precision with which the variables at stake during the resolution of the system will be stored. We can have a simple or double precision or both. In the case of a hybrid precision, the multigrid solver is launched in simple precision, but the residual is calculated in double precision.
  param.dictionnaire("double", precision_double_);
  param.dictionnaire("float", precision_float_);
  param.dictionnaire("mixed", precision_mix_);
  param.ajouter("iterations_mixed_solver", &max_iter_mixed_solver_); // XD_ADD_P entier Define the maximum number of iterations in mixed precision solver
}

Multigrille_base::Multigrille_base() : precision_double_(0), precision_float_(1),
  precision_mix_(2)
{
  check_residu_ = 0;
  seuil_ = 0.;
  max_iter_gcp_ = 0; // default, use multigrid solver, not gcp
  max_iter_gmres_ = 0; // default, use multigrid solver, not gmres
  n_krilov_ = 3;
  impr_ = 0;
  impr_gmres_ = 2;
  solv_jacobi_ = 0;
  solver_precision_ = precision_double_;
  relax_jacobi_.resize_array(1);
  relax_jacobi_[0] = 0.65;
  max_iter_mixed_solver_ = 4;
}

Entree& Multigrille_base::readOn(Entree& is)
{
  Process::exit();
  return is;
}



#ifdef DUMP_X_B_AND_RESIDUE_IN_FILE
void dump_x_b_residue_in_file(const IJK_Field_float& x, const IJK_Field_float& b,
                              IJK_Field_float& residu, int grid_level, int step_number,
                              Nom comment)
{

  if (Process::je_suis_maitre())
    {
      const int ni = x.ni();
      const int j_fix = 3;
      const int k_fix = 3;
      Nom n("plot_step");
      char ss[4];
      snprintf(ss, 4, "%03d", step_number);
      n += Nom(ss);
      n += Nom("_level");
      n += Nom(grid_level);
      n += Nom(".txt");
      SFichier f(n/* , ios::out --> default*/);
      f.setf(ios::scientific);
      for (int i = 0; i < ni; i++)
        {
          const float xx = x(i,j_fix, k_fix);
          const float bb = b(i,j_fix, k_fix);
          const float rr = residu(i,j_fix, k_fix);
          f << i << " " << xx << " " << bb << " " << rr << finl;
        }
      Cout << "STEP " << global_count_dump_in_file << " at grid level " << grid_level << " " << comment << finl;
      global_count_dump_in_file++;
    }
}

void dump_x_b_residue_in_file(const IJK_Field_double& x, const IJK_Field_double& b, IJK_Field_double& residu,
                              int grid_level, int step_number, Nom comment)
{

  if (Process::je_suis_maitre())
    {
      const int ni = x.ni();
      const int j_fix = 3;
      const int k_fix = 3;
      Nom n("plot_step");
      char ss[4];
      snprintf(ss, 4, "%03d", step_number);
      n += Nom(ss);
      n += Nom("_level");
      n += Nom(grid_level);
      n += Nom(".txt");
      SFichier f(n/* , ios::out --> default*/);
      f.setf(ios::scientific);
      for (int i = 0; i < ni; i++)
        {
          const double xx = x(i,j_fix, k_fix);
          const double bb = b(i,j_fix, k_fix);
          const double rr = residu(i,j_fix, k_fix);
          f << i << " " << xx << " " << bb << " " << rr << finl;
        }
      Cout << "STEP " << global_count_dump_in_file << " at grid level " << grid_level << " " << comment << finl;
      global_count_dump_in_file++;
    }
}
#endif



int Multigrille_base::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
  if (solver_precision_ == precision_double_)
    resoudre_systeme_template<double, ArrOfDouble>(a, b, x);
  else if (solver_precision_ == precision_float_)
    resoudre_systeme_template<float, ArrOfFloat>(a, b, x);
  else
    resoudre_systeme_template<double, ArrOfDouble>(a, b, x);
  return 1;
}

int Multigrille_base::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x, int n)
{
  resoudre_systeme(a, b, x);
  return 1;
}

// This solver does not need rhs to have updated virtual space
int Multigrille_base::get_flag_updated_input() const
{
  return 0;
}

int Multigrille_base::nsweeps_jacobi_residu(int level) const
{
  int i1 = level;
  int i2 = level;
  if (level > pre_smooth_steps_.size_array() - 1)
    i1 = pre_smooth_steps_.size_array() - 1;
  if (level > smooth_steps_.size_array() - 1)
    i2 = smooth_steps_.size_array() - 1;
  int nsweeps = std::max(pre_smooth_steps_[i1], smooth_steps_[i2]) + 1;
  return nsweeps;
}


template <>
void Multigrille_base::solve_ijk_in_storage_template<float>()
{
  IJK_Field_float& ijk_b = get_storage_float(STORAGE_RHS, 0);
  IJK_Field_float& ijk_x = get_storage_float(STORAGE_X, 0);
  IJK_Field_float& ijk_residu = get_storage_float(STORAGE_RESIDUE, 0);

  prepare_secmem(ijk_b);
  //pas sur de devoir echanger espace virtuel pour le second membre dans le cas du shear_perio...
  //ijk_b.echange_espace_virtuel(ijk_b.ghost());

  double norm_residue;
  norm_residue = multigrille(ijk_x, ijk_b, ijk_residu);
  if (norm_residue > seuil_)
    {
      Cerr << "Error in Multigrille_base: single precision pure multigrid solver did not converge to requested precision ("
           << seuil_ << ")\n Residue at end of solver= " << norm_residue << finl;
      Process::exit();
    }
}

template <>
void Multigrille_base::solve_ijk_in_storage_template<double>()
{
  IJK_Field_double& ijk_b = get_storage_double(STORAGE_RHS, 0);
  IJK_Field_double& ijk_x = get_storage_double(STORAGE_X, 0);
  IJK_Field_double& ijk_residu = get_storage_double(STORAGE_RESIDUE, 0);

  prepare_secmem(ijk_b);
  //pas sur de devoir echanger espace virtuel pour le second membre dans le cas du shear_perio...
  //ijk_b.echange_espace_virtuel(ijk_b.ghost());

  if (solver_precision_ == precision_double_)
    {
      if (max_iter_gcp_ > 0)
        {
          std::cout << "JE RENTRE DANDS resoudre_avec_gcp" << std::endl;
          resoudre_avec_gcp(ijk_x, ijk_b, ijk_residu);
        }
      else if (max_iter_gmres_ > 0)
        {
          std::cout << "JE RENTRE DANDS resoudre_avec_gmres" << std::endl;
          resoudre_avec_gmres(ijk_x, ijk_b, ijk_residu);
        }
      else if (solv_jacobi_ > 0)
        {
          std::cout << "JE RENTRE DANDS jacobi_residu" << std::endl;
          static int fcount = 0;
          ijk_x.data() = 0.;
          for (int i = 0; i < solv_jacobi_; i++)
            {
              jacobi_residu(ijk_x, &ijk_b, 0, 2 /* jacobi iterations */, &ijk_residu);
              dump_lata("x", ijk_x, fcount);
              dump_lata("residu", ijk_residu, fcount);
              fcount++;
            }
        }
      else
        {
          std::cout << "JE RENTRE DANDS multigrille" << std::endl;
          double norm_residue;
          norm_residue = multigrille(ijk_x, ijk_b, ijk_residu);
          if (norm_residue > seuil_)
            {
              Cerr << "Error in Multigrille_base: double precision pure multigrid solver did not converge to requested precision ("
                   << seuil_ << ")\n Residue at end of solver= " << norm_residue << finl;
              Process::exit();
            }
        }
    }
  else
    {
      // mixed precision solver
      ijk_x.data() = 0.;
      IJK_Field_float& float_b = get_storage_float(STORAGE_RHS, 0);
      IJK_Field_float& float_x = get_storage_float(STORAGE_X, 0);
      IJK_Field_float& float_residue = get_storage_float(STORAGE_RESIDUE, 0);
      float_b.data() = 0.;

      const int ni = ijk_x.ni();
      const int nj = ijk_x.nj();
      const int nk = ijk_x.nk();

      for (int k = 0; k < nk; k++)
        for (int j = 0; j < nj; j++)
          for (int i = 0; i < ni; i++)
            float_b(i, j, k) = (float)ijk_b(i, j, k);

      int iteration = 0;
      do
        {
          if (iteration > 0)
            for (int k = 0; k < nk; k++)
              for (int j = 0; j < nj; j++)
                for (int i = 0; i < ni; i++)
                  float_b(i, j, k) = (float)(-ijk_residu(i, j, k));


          // Launch multigrid solver in single precision:
          float_x.data() = 0.;
          prepare_secmem(float_b);
          //pas sur de devoir echanger espace virtuel pour le second membre dans le cas du shear_perio...
          //float_b.echange_espace_virtuel(float_b.ghost());
          float_x.shift_k_origin(needed_kshift_for_jacobi(0) - float_x.k_shift());
          double single_precision_residue = multigrille(float_x, float_b, float_residue);

          // Update x:
          for (int k = 0; k < nk; k++)
            for (int j = 0; j < nj; j++)
              for (int i = 0; i < ni; i++)
                ijk_x(i, j, k) += float_x(i, j, k);

          if (single_precision_residue < seuil_)
            break;

          // Compute residue in double precision:
          jacobi_residu(ijk_x, &ijk_b, 0, 0 /* jacobi iterations */, &ijk_residu);
          double nr = norme_ijk(ijk_residu);
          if (impr_)
            Cout << "Mixed precision solver iteration " << iteration
                 << " singleprecision residue= " << single_precision_residue
                 << " doubleprecision residue= " << nr << finl;

          iteration++;
          if (iteration > max_iter_mixed_solver_)
            {
              // Try to solve system on original grid with other solver
              nr = multigrille_failure();
              if (nr < seuil_)
                break;
              else
                {
                  Cerr << "Error in Multigrille_base: mixed precision solver did not converge in "
                       << max_iter_mixed_solver_ << " iterations." << finl;
                }
            }
        }
      while (1);
    }
}

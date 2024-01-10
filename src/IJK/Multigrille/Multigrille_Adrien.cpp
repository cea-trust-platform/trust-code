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

#include <Param.h>
#include <Parser.h>
#include <Statistiques.h>
#include <TRUSTArray.h>
#include <Multigrille_Adrien.h>
#include <communications.h>
#include <Domaine.h>
#include <Schema_Comm.h>
#include <IJK_discretization.h>
#include <Interprete_bloc.h>
#include <stat_counters.h>

#include <SSE_kernels.h>
using namespace SSE_Kernels;

Implemente_instanciable_sans_constructeur(Multigrille_Adrien, "Solv_Multigrille_Adrien", Multigrille_base);

Sortie& Multigrille_Adrien::printOn(Sortie& os) const
{
  return os;
}

Multigrille_Adrien::Multigrille_Adrien()
{
  ghost_size_ = 1;
}

void Multigrille_Adrien::ajouter_param(Param& param)
{
  param.ajouter("ghost_size", &ghost_size_);
  param.ajouter("coarsen_operators", &coarsen_operators_);

  Multigrille_base::ajouter_param(param);
}

Entree& Multigrille_Adrien::readOn(Entree& is)
{
  Nom ijkdis_name;
  Param param(que_suis_je());
  ajouter_param(param);
  param.ajouter("ijkdis_name", &ijkdis_name);
  param.lire_avec_accolades(is);

  if (ijkdis_name != Nom())
    {
      const IJK_discretization& ijkdis = ref_cast(IJK_discretization,
                                                  Interprete_bloc::objet_global(ijkdis_name));
      const IJK_Splitting& split = ijkdis.get_IJK_splitting();

      initialize(split);
    }
  return is;
}

int Multigrille_Adrien::completer(const Equation_base& eq)
{
  // fait dans readOn si on a lu ijkdis_name
  // fetch the vdf_to_ijk translator (assume there is one unique object, with conventional name)
  const Nom& ijkdis_name = IJK_discretization::get_conventional_name();
  const IJK_discretization& ijkdis = ref_cast(IJK_discretization,
                                              Interprete_bloc::objet_global(ijkdis_name));
  const IJK_Splitting& split = ijkdis.get_IJK_splitting();

  initialize(split);

  return 1;
}

void Multigrille_Adrien::initialize(const IJK_Splitting& split)
{
  if (solver_precision_ == precision_double_)
    completer_template<double, ArrOfDouble>(split);
  else if (solver_precision_ == precision_float_)
    completer_template<float, ArrOfFloat>(split);
  else if (solver_precision_ == precision_mix_)
    {
      completer_template<float, ArrOfFloat>(split);
      completer_double_for_residue(split);
    }

  IJK_Field_float rho;
  rho.allocate(split, IJK_Splitting::ELEM, 0);
  rho.data() = 1.;
  set_rho<float, ArrOfFloat>(rho);
}

// Says that the Poisson equation has variable coefficient rho, given at elements
// Must be called in parallel.
// (the function converts the given field to ijk ordering, computes the coarsened
//  rho fields, the matrix coefficients with boundary conditions for all levels
//  and the coarse matrix for the coarse grid solver).
void Multigrille_Adrien::set_rho(const DoubleVect& rho)
{
  static Stat_Counter_Id counter = statistiques().new_counter(0, "Multigrille_Adrien::set_rho");
  statistiques().begin_count(counter);
  if (solver_precision_ != precision_float_)
    {
      IJK_Field_double ijk_rho(grids_data_double_[0].get_update_rho());
      convert_to_ijk(rho, ijk_rho);
      set_rho(ijk_rho);
    }
  else
    {
      IJK_Field_float ijk_rho(grids_data_float_[0].get_update_rho());
      convert_to_ijk(rho, ijk_rho);
      set_rho(ijk_rho);
    }
  statistiques().end_count(counter);
}


int Multigrille_Adrien::needed_kshift_for_jacobi(int level) const
{
  return nsweeps_jacobi_residu(level);
}

void Multigrille_Adrien::completer_double_for_residue(const IJK_Splitting& splitting)
{
  Cerr << "Multigrille_Adrien::completer_double_for_residue" << finl;
  grids_data_double_.dimensionner(1);
  grids_data_double_[0].initialize(splitting, ghost_size_, nsweeps_jacobi_residu(0));
}

double Multigrille_Adrien::multigrille_failure()
{
  IJK_Field_double& b = get_storage_double(STORAGE_RHS, 0);
  IJK_Field_double& x = get_storage_double(STORAGE_X, 0);
  IJK_Field_double& residu = get_storage_double(STORAGE_RESIDUE, 0);

  set_coarse_matrix().build_matrix(set_grid_data<double>(0).get_faces_coefficients());
  coarse_solver(x, b);
  jacobi_residu(x, &b, 0, 0 /* not jacobi */, &residu);
  double norme_residu_final = norme_ijk(residu);
  return norme_residu_final;

}

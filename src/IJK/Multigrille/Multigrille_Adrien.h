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

#ifndef Multigrille_Adrien_included
#define Multigrille_Adrien_included

#include <Multigrille_base.h>
#include <Equation_base.h>
#include <TRUSTArray.h>
#include <Grid_Level_Data_template.h>
#include <Coarsen_Operator_base.h>

class IJK_Splitting;
class Multigrille_Adrien : public Multigrille_base
{
  Declare_instanciable_sans_constructeur(Multigrille_Adrien);
public:
  Multigrille_Adrien();
  int completer(const Equation_base& eq);
  void initialize(const IJK_Splitting&);
  int nb_grid_levels() const override { return coarsen_operators_.size() + 1; }

  void set_rho(const DoubleVect& rho);
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void set_rho(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rho);
  void reset_rho();
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void set_inv_rho(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& inv_rho);
  template <typename _TYPE_FUNC_, typename _TYPE_, typename _TYPE_ARRAY_>
  void set_inv_rho_template(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rho, bool set_coarse_matrix_flag, bool use_coeffs_from_double);

  inline void prepare_secmem(IJK_Field_float& x) const override
  {
    prepare_secmem_(x);
  }
  inline void prepare_secmem(IJK_Field_double& x) const override
  {
    prepare_secmem_(x);
  }

  inline void alloc_field(IJK_Field_float& x, int level, bool with_additional_k_layers = false) const override
  {
    alloc_field_(x,level, with_additional_k_layers);
  }
  inline void alloc_field(IJK_Field_double& x, int level, bool with_additional_k_layers = false) const override
  {
    alloc_field_(x,level, with_additional_k_layers);
  }
  inline void dump_lata(const Nom& field, const IJK_Field_float& data, int tstep) const override
  {
    dump_lata_(field, data, tstep);
  }
  inline void dump_lata(const Nom& field, const IJK_Field_double& data, int tstep) const override
  {
    dump_lata_(field, data, tstep);
  }

protected:
  void ajouter_param(Param& param) override;
  int needed_kshift_for_jacobi(int level) const override;

  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void completer_template(const IJK_Splitting&);

  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void setup_coarse_grid(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse) const;
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void compute_coefficients(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coeffs_faces,
                            const int grid_level) const;
  template <typename _TYPE_FUNC_, typename _TYPE_, typename _TYPE_ARRAY_>
  void set_rho_template(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& rho, bool set_coarse_matrix, bool use_coeffs_from_double);

  void completer_double_for_residue(const IJK_Splitting& splitting);

  inline void jacobi_residu(IJK_Field_float& x,
                            const IJK_Field_float *secmem, /* if null pointer, take secmem = 0 (to compute A*x) */
                            const int grid_level,
                            const int n_jacobi,
                            IJK_Field_float *residu) const override
  {
    jacobi_residu_(x,secmem, grid_level, n_jacobi, residu);
  }
  inline void jacobi_residu(IJK_Field_double& x,
                            const IJK_Field_double *secmem,
                            const int grid_level,
                            const int n_jacobi,
                            IJK_Field_double *residu) const override
  {
    jacobi_residu_(x,secmem, grid_level, n_jacobi, residu);
  }


  inline void coarsen(const IJK_Field_float& fine, IJK_Field_float& coarse, int fine_level) const override
  {
    coarsen_(fine, coarse, fine_level);
  }
  inline void coarsen(const IJK_Field_double& fine, IJK_Field_double& coarse, int fine_level) const override
  {
    coarsen_(fine, coarse, fine_level);
  }
  inline void interpolate_sub_shiftk(const IJK_Field_float& coarse, IJK_Field_float& fine, int fine_level) const override
  {
    interpolate_sub_shiftk_(coarse, fine, fine_level);
  }
  inline void interpolate_sub_shiftk(const IJK_Field_double& coarse, IJK_Field_double& fine, int fine_level) const override
  {
    interpolate_sub_shiftk_(coarse, fine, fine_level);
  }
  inline IJK_Field_float& get_storage_float(StorageId si, int level) override
  {
    return get_storage_template_<float>(si, level);
  }
  inline IJK_Field_double& get_storage_double(StorageId si, int level) override
  {
    return get_storage_template_<double>(si, level);
  }

private:

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,double>::value, int>::type
  get_grid_data_size() const { return grids_data_double_.size(); }

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,float>::value, int>::type
  get_grid_data_size() const { return grids_data_float_.size(); }

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,double>::value, const Grid_Level_Data_template<double>&>::type
  get_grid_data(int level) const { return grids_data_double_[level]; }

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,float>::value, const Grid_Level_Data_template<float>&>::type
  get_grid_data(int level) const { return grids_data_float_[level]; }

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,double>::value, Grid_Level_Data_template<double>&>::type
  set_grid_data(int level) { return grids_data_double_[level]; }

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,float>::value, Grid_Level_Data_template<float>&>::type
  set_grid_data(int level) { return grids_data_float_[level]; }

  //ToDo: temporary workaround as virtual templates are not possible...
  // when suppressing mix precision multigrid, move the template on the class
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void prepare_secmem_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x) const;
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void dump_lata_(const Nom& field, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& data, int tstep) const;
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void alloc_field_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, int level, bool with_additional_k_layers = false) const;
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void jacobi_residu_(IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x,
                      const IJK_Field_template<_TYPE_,_TYPE_ARRAY_> *secmem, /* if null pointer, take secmem = 0 (to compute A*x) */
                      const int grid_level,
                      const int n_jacobi,
                      IJK_Field_template<_TYPE_,_TYPE_ARRAY_> *residu) const;

  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void coarsen_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, int fine_level) const;
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void interpolate_sub_shiftk_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, int fine_level) const;
  template <typename _TYPE_>
  IJK_Field_template<_TYPE_,TRUSTArray<_TYPE_>>& get_storage_template_(StorageId, int level);

  // number of isotropic coarsening steps
  int nb_isotropic_coarsening_;
  // Thickness of the ghost zone: determines the maximum number of sweeps of the Jacobi
  // smoother that are performed in the same pass (1, 2, 4 or 6 can be efficient depending
  // on the architecture and the problem)
  int ghost_size_;

  // Coarsening operators (read in the .data file)
  VECT(DERIV(Coarsen_Operator_base)) coarsen_operators_;

  // Data for each grid (number of grids = number of coarsening operators + 1)
  TRUST_Vector<Grid_Level_Data_template, float> grids_data_float_;
  TRUST_Vector<Grid_Level_Data_template, double>  grids_data_double_;
};

#include<Multigrille_Adrien.tpp>

#endif

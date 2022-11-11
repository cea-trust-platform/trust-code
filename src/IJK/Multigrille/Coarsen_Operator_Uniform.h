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

#ifndef Coarsen_Operator_Uniform_included
#define Coarsen_Operator_Uniform_included

#include <Coarsen_Operator_base.h>

// Refine all cells by factor 2 in K, JK or IJK directions
class Coarsen_Operator_Uniform : public Coarsen_Operator_base
{
  Declare_instanciable(Coarsen_Operator_Uniform);
public:
  inline void initialize_grid_data(const Grid_Level_Data_double& fine, Grid_Level_Data_double& coarse, int additional_k_layers) override
  {
    initialize_grid_data_(fine, coarse, additional_k_layers);
  }
  inline void initialize_grid_data(const Grid_Level_Data_float& fine, Grid_Level_Data_float& coarse, int additional_k_layers) override
  {
    initialize_grid_data_(fine, coarse, additional_k_layers);
  }
  // Compute values on coarse mesh from values on fine mesh
  inline void coarsen(const IJK_Field_double& fine, IJK_Field_double& coarse, int compute_weighted_average = 0) const override
  {
    coarsen_(fine, coarse, compute_weighted_average);
  }
  inline void coarsen(const IJK_Field_float& fine, IJK_Field_float& coarse, int compute_weighted_average = 0) const override
  {
    coarsen_(fine, coarse, compute_weighted_average);
  }

  // Interpolate values on coarse mesh to fine mesh and substract them from existing values
  inline void interpolate_sub_shiftk(const IJK_Field_double& coarse, IJK_Field_double& fine, const int kshift) const override
  {
    interpolate_sub_shiftk_(coarse, fine, kshift);
  }
  inline void interpolate_sub_shiftk(const IJK_Field_float& coarse, IJK_Field_float& fine, const int kshift) const override
  {
    interpolate_sub_shiftk_(coarse, fine, kshift);
  }

protected:
  void ajouter_param(Param& param) override;
  // By how much shall we coarsen in each direction ?
  ArrOfInt coarsen_factors_;

private:

  //ToDo: temporary workaround as virtual templates are not possible...
  // when suppressing mix precision multigrid, move the template on the class
  template<typename _TYPE_>
  void initialize_grid_data_(const Grid_Level_Data_template<_TYPE_>& fine, Grid_Level_Data_template<_TYPE_>& coarse,
                             int additional_k_layers);

  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void coarsen_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, int compute_weighted_average = 0) const;

  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void interpolate_sub_shiftk_(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coarse, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& fine, const int kshift) const;

};

#include <Coarsen_Operator_Uniform.tpp>
#endif

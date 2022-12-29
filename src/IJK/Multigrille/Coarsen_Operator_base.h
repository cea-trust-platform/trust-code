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

#ifndef Coarsen_Operator_base_included
#define Coarsen_Operator_base_included

#include <Grid_Level_Data_template.h>
#include <TRUST_Vector.h>
#include <Param.h>

// Describes refine and coarsening operators
class Coarsen_Operator_base : public Objet_U
{
  Declare_base(Coarsen_Operator_base);
public:
  virtual void initialize_grid_data(const Grid_Level_Data_double& fine, Grid_Level_Data_double& coarse,
                                    int additional_k_layers) = 0;
  // Compute values on coarse mesh from values on fine mesh
  virtual void coarsen(const IJK_Field_double& fine, IJK_Field_double& coarse, int compute_weighted_average = 0) const = 0;
  // Interpolate values on coarse mesh to fine mesh and substract them from existing values
  virtual void interpolate_sub_shiftk(const IJK_Field_double& coarse, IJK_Field_double& fine, const int kshift) const = 0;
  virtual void initialize_grid_data(const Grid_Level_Data_float& fine, Grid_Level_Data_float& coarse,
                                    int additional_k_layers) = 0;
  // Compute values on coarse mesh from values on fine mesh
  virtual void coarsen(const IJK_Field_float& fine, IJK_Field_float& coarse, int compute_weighted_average = 0) const = 0;
  // Interpolate values on coarse mesh to fine mesh and substract them from existing values
  virtual void interpolate_sub_shiftk(const IJK_Field_float& coarse, IJK_Field_float& fine, const int kshift) const = 0;
protected:
  virtual void ajouter_param(Param& param) {};
};

#endif

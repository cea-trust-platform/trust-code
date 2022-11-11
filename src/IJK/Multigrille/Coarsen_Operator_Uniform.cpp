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

#include <Coarsen_Operator_Uniform.h>
#include <IJK_Grid_Geometry.h>
#include <stat_counters.h>

Implemente_instanciable_sans_constructeur(Coarsen_Operator_Uniform, "Coarsen_Operator_Uniform", Coarsen_Operator_base);

Coarsen_Operator_Uniform::Coarsen_Operator_Uniform()
{
  coarsen_factors_.resize_array(3);
  // default: uniform refinement by factor 2
  coarsen_factors_ = 2;
}

// Reads coarsening parameters in the .data file. See ajouter_param()
Entree& Coarsen_Operator_Uniform::readOn(Entree& is)
{
  Coarsen_Operator_base::readOn(is);
  // Check if valid parameters
  bool ok = false;
  if (coarsen_factors_[0] == 1 && coarsen_factors_[1] == 1 && coarsen_factors_[2] == 2) ok = true;
  if (coarsen_factors_[0] == 1 && coarsen_factors_[1] == 2 && coarsen_factors_[2] == 2) ok = true;
  if (coarsen_factors_[0] == 2 && coarsen_factors_[1] == 1 && coarsen_factors_[2] == 2) ok = true;
  if (coarsen_factors_[0] == 2 && coarsen_factors_[1] == 2 && coarsen_factors_[2] == 2) ok = true;
  if (coarsen_factors_[0] == 1 && coarsen_factors_[1] == 1 && coarsen_factors_[2] == 3) ok = true;
  if (coarsen_factors_[0] == 1 && coarsen_factors_[1] == 3 && coarsen_factors_[2] == 2) ok = true;
  if (!ok)
    {
      Cerr << "Error in Coarsen_Operator_Uniform::readOn: invalid combination of coarsening factors" << finl;
      Process::exit();
    }
  return is;
}

Sortie& Coarsen_Operator_Uniform::printOn(Sortie& os) const
{
  return Coarsen_Operator_base::printOn(os);
}

void Coarsen_Operator_Uniform::ajouter_param(Param& param)
{
  param.ajouter("coarsen_i", &coarsen_factors_[0]);
  param.ajouter("coarsen_j", &coarsen_factors_[1]);
  param.ajouter("coarsen_k", &coarsen_factors_[2]);
  Coarsen_Operator_base::ajouter_param(param);
}

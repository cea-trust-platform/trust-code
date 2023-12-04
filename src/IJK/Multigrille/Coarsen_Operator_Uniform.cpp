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

#include <Coarsen_Operator_Uniform.h>
#include <IJK_Grid_Geometry.h>
#include <stat_counters.h>

Implemente_instanciable_sans_constructeur(Coarsen_Operator_Uniform, "Coarsen_Operator_Uniform", Coarsen_Operator_base);
// XD Coarsen_Operator_Uniform objet_lecture nul 0 Object defining the uniform coarsening process of the given grid in IJK discretization
// XD   attr Coarsen_Operator_Uniform chaine Coarsen_Operator_Uniform 1 not_set
// XD   attr aco chaine(into=["{"]) aco 0 opening curly brace
// XD   attr coarsen_i chaine(into=["coarsen_i"]) coarsen_i 1 not_set
// XD   attr coarsen_i_val entier coarsen_i_val 1 Integer indicating the number by which we will divide the number of elements in the I direction (in order to obtain a coarser grid)
// XD   attr coarsen_j chaine(into=["coarsen_j"]) coarsen_j 1 not_set
// XD   attr coarsen_j_val entier coarsen_j_val 1 Integer indicating the number by which we will divide the number of elements in the J direction (in order to obtain a coarser grid)
// XD   attr coarsen_k chaine(into=["coarsen_k"]) coarsen_k 1 not_set
// XD   attr coarsen_k_val entier coarsen_k_val 1 Integer indicating the number by which we will divide the number of elements in the K direction (in order to obtain a coarser grid)
// XD   attr acof chaine(into=["}"]) acof 0 closing curly brace
// XD coarsen_operators listobj nul 0 Coarsen_Operator_Uniform 0 not_set
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

  const int coarsenCode = coarsen_factors_[0]*100 + coarsen_factors_[1]*10 + coarsen_factors_[2];
  switch (coarsenCode)
    {
    case 122:
    case 212:
    case 221:
    case 222:
    case 113:
    case 132:
      break;
    default:
      Cerr << "Error in Coarsen_Operator_Uniform::readOn: invalid combination of coarsening factors\n";
      Process::exit();
      break;
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

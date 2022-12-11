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

#include <Parser_Eval.h>

double Parser_Eval::eval_fct_single_position_single_compo(const DoubleVect& position, const double* t, const double* val_param, const int ncomp) const
{
  const int D = position.size();
  const int with_time = t ? 1 : 0;

  Parser_U& fct = parser(ncomp);
  if (with_time) fct.setVar(0, *t);
  for (int d = 0; d < D; d++)
    fct.setVar(with_time + d, position(d));
  if (val_param) fct.setVar(4, *val_param);
  return fct.eval();
}

void Parser_Eval::eval_fct_single_position(const DoubleVect& position, const double* t, const double* val_param, DoubleVect& val) const
{
  for (int k = 0; k < fonction_.size(); k++)
    val(k) = eval_fct_single_position_single_compo(position, t, val_param, k);
}

void Parser_Eval::eval_fct_single_compo(const DoubleTab& positions, const double* t, DoubleVect& val, const int ncomp) const
{
  const int pos_size = positions.dimension(0), D = positions.dimension(1);
  DoubleVect position(D);

  for (int i = 0; i < pos_size; i++)
    {
      for (int d = 0; d < D; d++) position(d) = positions(i, d);
      val(i) = eval_fct_single_position_single_compo(position, t, nullptr, ncomp);
    }
}

void Parser_Eval::eval_fct_(const DoubleTab& positions, const double* t, const DoubleTab* val_param, DoubleTab& val) const
{
  const int pos_size = positions.dimension(0), D = positions.dimension(1), dim = fonction_.size();
  DoubleVect position(D), val_elem(dim);

  for (int i = 0; i < pos_size; i++)
    {
      for (int d = 0; d < D; d++) position(d) = positions(i, d);
      eval_fct_single_position(position, t, val_param ? &(*val_param)(i) : nullptr, val_elem);
      for (int k = 0; k < dim; k++) val(i, k) = val_elem(k);
    }
}

void Parser_Eval::eval_fct(const DoubleTabs& variables, DoubleTab& val) const
{
  int dim;
  int pos_size = val.dimension(0);

  assert(fonction_.size() == 1);

  int nb_dim = val.nb_dim();
  int nbvars = variables.size();

  Parser_U& fct = parser(0);

  if (nb_dim==1)
    {
      for (int i=0; i<pos_size; i++)
        {
          for (int ivar=0; ivar<nbvars; ivar++)
            fct.setVar(ivar,variables[ivar](i));

          val(i) = fct.eval();
        }
    }
  else
    {
      dim = val.dimension(1);
      for (int i=0; i<pos_size; i++)
        {
          for(int k=0; k<dim; k++)
            {
              for (int ivar=0; ivar<nbvars; ivar++)
                fct.setVar(ivar,variables[ivar](i,k));
              val(i,k) = fct.eval();
            }
        }
    }
}


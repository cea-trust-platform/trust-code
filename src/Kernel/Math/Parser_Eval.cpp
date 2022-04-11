/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Parser_Eval.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Parser_Eval.h>

//Cas d une fonction a evaluer qui depend de l espace (x,y,z)
//On traite toute(s) le(s) composante(s)
void Parser_Eval::eval_fct(const DoubleVect& positions,DoubleVect& val) const
{
  int dim = fonction_.size();
  int dimension = positions.size();

  double x,y,z=0;
  x = positions(0);
  y = positions(1);
  if (dimension>2)
    z = positions(2);

  for(int k=0; k<dim; k++)
    {
      Parser_U& fct = parser(k);
      fct.setVar(0,x);
      fct.setVar(1,y);
      if (dimension>2)
        fct.setVar(2,z);
      val(k) = fct.eval();
    }


}

//Cas d une fonction a evaluer qui depend de l espace (x,y,z)
//On traite toute(s) le(s) composante(s)
void Parser_Eval::eval_fct(const DoubleTab& positions,DoubleTab& val) const
{
  int pos_size = positions.dimension(0);
  int dim = fonction_.size();
  int nb_dim = val.nb_dim();
  int dimension = positions.dimension(1);
  double x,y,z;
  x=y=z=0.;

  if (nb_dim==1)
    {
      Parser_U& fct = parser(0);
      for (int i=0; i<pos_size; i++)
        {
          x = positions(i,0);
          y = positions(i,1);
          if (dimension>2)
            z = positions(i,2);
          // Optimisation
          fct.setVar(0,x); //fct.setVar("x",x);
          fct.setVar(1,y); //fct.setVar("y",y);
          if (dimension>2)
            fct.setVar(2,z); //fct.setVar("z",z);
          val(i) = fct.eval();
        }
    }
  else
    {
      for (int i=0; i<pos_size; i++)
        {
          x = positions(i,0);
          y = positions(i,1);
          if (dimension>2)
            z = positions(i,2);

          for(int k=0; k<dim; k++)
            {
              Parser_U& fct = parser(k);
              fct.setVar(0,x);
              fct.setVar(1,y);
              if (dimension>2)
                fct.setVar(2,z);
              val(i,k) = fct.eval();
            }
        }
    }

}

//Cas d une fonction a evaluer qui depend de l espace (x,y,z)
//On ne traite que la composante ncomp
void Parser_Eval::eval_fct(const DoubleTab& positions,DoubleVect& val,const int ncomp) const
{

  int pos_size = positions.dimension(0);
  int dimension = positions.dimension(1);
  double x,y,z;
  x=y=z=0.;

  Parser_U& fct = parser(ncomp);
  for (int i=0; i<pos_size; i++)
    {
      x = positions(i,0);
      y = positions(i,1);
      if (dimension>2)
        z = positions(i,2);

      fct.setVar(0,x);
      fct.setVar(1,y);
      if (dimension>2)
        fct.setVar(2,z);

      val(i) = fct.eval();
    }


}

//Cas d une fonction a evaluer qui depend de l espace et du temps (x,y,z,t)
//On traite toute(s) le(s) composante(s)
void Parser_Eval::eval_fct(const DoubleVect& positions,const double tps,DoubleVect& val) const
{
  int dim = fonction_.size();
  int dimension = positions.size();

  double x,y,z=0;
  x = positions(0);
  y = positions(1);
  if (dimension>2)
    z = positions(2);

  for(int k=0; k<dim; k++)
    {
      Parser_U& fct = parser(k);
      fct.setVar(0,tps);
      fct.setVar(1,x);
      fct.setVar(2,y);
      if (dimension>2)
        fct.setVar(3,z);
      val(k) = fct.eval();
    }
}
//Cas d une fonction a evaluer qui depend de l espace et du temps (x,y,z,t)
//On traite toute(s) le(s) composante(s)
void Parser_Eval::eval_fct(const DoubleTab& positions,const double tps,DoubleTab& val) const
{
  int pos_size = positions.dimension(0);
  int dim = fonction_.size();
  int nb_dim = val.nb_dim();
  int dimension = positions.dimension(1);

  double x,y,z;
  x=y=z=0.;

  if (nb_dim==1)
    {
      Parser_U& fct = parser(0);
      fct.setVar(0,tps); //fct.setVar("t",tps);
      for (int i=0; i<pos_size; i++)
        {
          x = positions(i,0);
          y = positions(i,1);
          if (dimension>2)
            z = positions(i,2);

          fct.setVar(1,x);
          fct.setVar(2,y);
          if (dimension>2)
            fct.setVar(3,z);
          val(i) = fct.eval();
        }
    }
  else
    {
      for (int i=0; i<pos_size; i++)
        {
          x = positions(i,0);
          y = positions(i,1);
          if (dimension>2)
            z = positions(i,2);

          for(int k=0; k<dim; k++)
            {
              Parser_U& fct = parser(k);
              fct.setVar(0,tps);
              fct.setVar(1,x);
              fct.setVar(2,y);
              if (dimension>2)
                fct.setVar(3,z);
              val(i,k) = fct.eval();
            }
        }
    }


}

//Cas d une fonction a evaluer qui depend de l espace et du temps (x,y,z,t)
//On ne traite que la composante ncomp
void Parser_Eval::eval_fct(const DoubleTab& positions,const double tps,DoubleVect& val,const int ncomp) const
{

  int pos_size = positions.dimension(0);
  int dimension = positions.dimension(1);
  double x,y,z;
  x=y=z=0.;

  Parser_U& fct = parser(ncomp);
  fct.setVar(0,tps);
  for (int i=0; i<pos_size; i++)
    {
      x = positions(i,0);
      y = positions(i,1);
      if (dimension>2)
        z = positions(i,2);

      fct.setVar(1,x);
      fct.setVar(2,y);
      if (dimension>2)
        fct.setVar(3,z);

      val(i) = fct.eval();
    }

}

//Cas d une fonction a evaluer qui depend de l espace, du temps et des valeurs d un champ inconnu (x,y,z,t,val)
//On traite toute(s) le(s) composante(s)
void Parser_Eval::eval_fct(const DoubleTab& positions,const double tps,const DoubleTab& val_param,DoubleTab& val) const
{
  int pos_size = positions.dimension(0);
  int dim = fonction_.size();
  int nb_dim = val.nb_dim();
  int dimension = positions.dimension(1);

  double x,y,z,valp;
  x=y=z=valp=0.;

  if (nb_dim==1)
    {
      Parser_U& fct = parser(0);
      fct.setVar(0,tps);
      for (int i=0; i<pos_size; i++)
        {
          x = positions(i,0);
          y = positions(i,1);
          if (dimension>2)
            z = positions(i,2);
          valp = val_param(i);

          fct.setVar(1,x);
          fct.setVar(2,y);
          if (dimension>2)
            fct.setVar(3,z);
          fct.setVar(4,valp);
          val(i) = fct.eval();
        }
    }
  else
    {
      for (int i=0; i<pos_size; i++)
        {
          x = positions(i,0);
          y = positions(i,1);
          if (dimension>2)
            z = positions(i,2);

          for(int k=0; k<dim; k++)
            {
              Parser_U& fct = parser(k);
              valp = val_param(i,k);
              fct.setVar(0,tps);
              fct.setVar(1,x);
              fct.setVar(2,y);
              if (dimension>2)
                fct.setVar(3,z);
              fct.setVar(4,valp);
              val(i,k) = fct.eval();
            }
        }
    }

}

//Cas d une fonction a evaluer qui depend de l espace, du temps et des valeurs d un champ inconnu (x,y,z,t,val)
//On ne traite que la composante ncomp
void Parser_Eval::eval_fct(const DoubleTab& positions,const double tps,const DoubleTab& val_param,DoubleVect& val,const int ncomp) const
{
  int pos_size = positions.dimension(0);
  int dimension = positions.dimension(1);
  double x,y,z,valp;
  x=y=z=valp=0.;

  Parser_U& fct = parser(ncomp);
  fct.setVar(0,tps);
  for (int i=0; i<pos_size; i++)
    {
      x = positions(i,0);
      y = positions(i,1);
      if (dimension>2)
        z = positions(i,2);
      valp = val_param(i);

      fct.setVar(1,x);
      fct.setVar(2,y);
      if (dimension>2)
        fct.setVar(3,z);
      fct.setVar(4,valp);

      val(i) = fct.eval();
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


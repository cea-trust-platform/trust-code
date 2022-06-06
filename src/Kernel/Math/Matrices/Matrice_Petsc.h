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

#ifndef Matrice_Petsc_included
#define Matrice_Petsc_included

#include <petsc_for_kernel.h>
#include <Matrice_Base.h>
class Matrice_Petsc : public Matrice_Base
{
  Declare_instanciable(Matrice_Petsc);
public:
  int ordre() const override
  {
#ifdef PETSCKSP_H
    PetscInt m,n;
    MatGetSize(Mat_, &m, &n);
    return m == n ? m : 0;
#else
    Cerr << "Matrice_Petsc non disponible." << finl;
    Process::exit();
    return 0;
#endif
  };
  int nb_lignes() const override
  {
#ifdef PETSCKSP_H
    PetscInt m,n;
    MatGetSize(Mat_, &m, &n);
    return m;
#else
    Cerr << "Matrice_Petsc non disponible." << finl;
    Process::exit();
    return 0;
#endif
  }
  int nb_colonnes() const override
  {
#ifdef PETSCKSP_H
    PetscInt m,n;
    MatGetSize(Mat_, &m, &n);
    return n;
#else
    Cerr << "Matrice_Petsc non disponible." << finl;
    Process::exit();
    return 0;
#endif
  }
  void scale(const double x) override
  {
    Cerr << "ToDo" << finl;
    Process::exit(-1);
  };

  DoubleVect& ajouter_multvect_(const DoubleVect& x, DoubleVect& r) const override
  {
    Cerr << "ToDo" << finl;
    Process::exit(-1);
    return r;
  }
  DoubleVect& ajouter_multvectT_(const DoubleVect& x, DoubleVect& r) const override
  {
    Cerr << "ToDo" << finl;
    Process::exit(-1);
    return r;
  }
  DoubleTab& ajouter_multTab_(const DoubleTab& x, DoubleTab& r) const override
  {
    Cerr << "ToDo" << finl;
    Process::exit(-1);
    return r;
  }
#ifdef PETSCKSP_H
  inline const Mat& getMat() const
  {
    return Mat_;
  }

private :
  Mat Mat_;
#endif
};
#endif

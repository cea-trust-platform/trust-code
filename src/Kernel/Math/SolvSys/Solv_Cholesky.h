/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Solv_Cholesky.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_Cholesky_included
#define Solv_Cholesky_included

#include <SolveurSys_base.h>
#include <Matrice_Morse_Sym.h>

class IntVect;
class DoubleTab;
class Matrice_Morse_Sym;
class Matrice_Morse;

class Solv_Cholesky : public SolveurSys_base
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Solv_Cholesky);

public :

  inline Solv_Cholesky();
  inline ~Solv_Cholesky();

  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect& );

  inline int resoudre_systeme(const Matrice_Base& M, const DoubleVect& A,
                              DoubleVect& B,
                              int niter_max)
  {
    return resoudre_systeme(M, A,B);
  };
  inline int solveur_direct() const
  {
    return 1;
  };
protected :

  int Cholesky(const Matrice_Morse_Sym&, const DoubleVect&, DoubleVect& );
  int Fact_Cholesky(const Matrice_Morse_Sym&, const int ); // met a jour matrice_bande_factorisee_fortran_
  int largeur_de_bande_;
  ArrOfDouble matrice_bande_factorisee_fortran_;
  Matrice_Morse_Sym matrice_de_travail;
};



inline Solv_Cholesky::Solv_Cholesky()
{
  matrice_bande_factorisee_fortran_=0;
}

inline Solv_Cholesky::~Solv_Cholesky()
{
  //if ( matrice_bande_factorisee_fortran_ ) delete[] matrice_bande_factorisee_fortran_;
}

#endif


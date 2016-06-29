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
// File:        Precond_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Precond_base.h>
#include <Motcle.h>

Implemente_base_sans_constructeur(Precond_base,"Precond_base",Objet_U);

Precond_base::Precond_base()
{
  status_ = REINIT_ALL;
  echange_ev_solution_ = 1;
}

Sortie& Precond_base::printOn(Sortie& s ) const
{
  return s  << que_suis_je();
}

Entree& Precond_base::readOn(Entree& is )
{
  return is;
}

// Description: this method must be called before preconditionner() whenever
//  the matrix changes (coefficients only or coefficients and structure) to
//  tell the object to recompute some matrix related data.
void Precond_base::reinit(Init_Status x)
{
  switch(x)
    {
    case READY:
      break;
    case REINIT_COEFF:
      if (status_ != REINIT_ALL) status_ = REINIT_COEFF;
      break;
    case REINIT_ALL:
      status_ = REINIT_ALL;
      break;
    default:
      Cerr << "Error Precond_base::reinit" << finl;
      exit();
    }
}

// Description: returns 1 if the implementation accepts Matrice_Morse_Sym matrices,  otherwise 0.
//  (must be reimplemented if answer is 0)
int Precond_base::supporte_matrice_morse_sym()
{
  return 1;
}




// Description: Call this method to change the behaviour of the preconditionner() method
//  0-> result will not have an up-to-date virtual space
//  1-> yes it will (this must be the default value in the implementation)
void Precond_base::set_echange_espace_virtuel_resultat(int flag)
{
  echange_ev_solution_ = flag;
}

// Description: Call to the prepare() method if reinit() has been called previously, then
//  call to preconditionner_()...
// If echange_ev_solution is not zero, the solution vector will have an updated virtual space.
int Precond_base::preconditionner(const Matrice_Base& m, const DoubleVect& src, DoubleVect& solution)
{
  if (status_ != READY)
    prepare(m, src);
  // implementation of the prepare() method must call ancestor methods until Precond_base::prepare();
  assert(status_ == READY);
  int ok = preconditionner_(m, src, solution);
  return ok;
}

// Description: prepares the preconditionner if needed after a matrix change (reinit() call).
//  prepare() is public, in case we want to perform preparation separately from preconditionning (eg nested solvers)
//  but calling prepare() is not mandatory (done automatically in preconditionner())
//  m is the new matrix, and md is the descriptor of the solution vector.
void Precond_base::prepare(const Matrice_Base& m, const DoubleVect& v)
{
  if (status_ != READY)
    prepare_(m, v);
  assert(status_ == READY);
}

// Description: this method must be overloaded if some preparation is necessary.
//  It is called after a call to reinit() and before the next call to preconditionner_().
//  Do not forget to call the prepare_() method of the ancestor (at the end of the routine, otherwise
//   status will be put to READY) and use get_status() to know what should be prepared.
void Precond_base::prepare_(const Matrice_Base& m, const DoubleVect& v)
{
  status_ = READY;
}

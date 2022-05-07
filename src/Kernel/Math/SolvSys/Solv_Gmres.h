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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Solv_Gmres.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_Gmres_included
#define Solv_Gmres_included

#include <TRUSTTabs_forward.h>
#include <solv_iteratif.h>
#include <TRUSTVects.h>
class Matrice_Morse_Sym;
class Matrice_Morse;

class Solv_Gmres : public solv_iteratif
{
  Declare_instanciable_sans_constructeur(Solv_Gmres);

public :

  Solv_Gmres();
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect& ) override;
  inline int resoudre_systeme(const Matrice_Base& M, const DoubleVect& A, DoubleVect& B, int niter_max) override
  {
    return resoudre_systeme(M, A, B);
  }

protected :
  int Gmres(const Matrice_Morse&, const DoubleVect&, DoubleVect& );

  DoubleVects v; //espcace Krilov
  int is_local_gmres,precond_diag;
  int nb_it_max_, controle_residu_, dim_espace_Krilov_;
};

#endif /* Solv_Gmres_included */

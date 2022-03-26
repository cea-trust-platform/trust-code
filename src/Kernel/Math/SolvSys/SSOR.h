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
// File:        SSOR.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SSOR_included
#define SSOR_included

#include <Precond_base.h>
#include <TRUSTTab.h>

class Matrice_Morse_Sym;
class Matrice_Bloc_Sym;

class SSOR : public Precond_base
{
  Declare_instanciable_sans_constructeur(SSOR);
public:
  SSOR();

  double get_omega() { return omega_; }
  void   set_omega(double x)
  {
    omega_ = x;
    reinit(REINIT_COEFF);
  }

  // SSOR does not use the virtual space of the input vector:
  int get_flag_updated_input() const override { return 0; }

protected:
  int preconditionner_(const Matrice_Base&, const DoubleVect& secmem, DoubleVect& solution) override;
  void prepare_(const Matrice_Base&, const DoubleVect& secmem) override;

  void ssor(const Matrice_Morse_Sym&, DoubleVect&);
  void ssor(const Matrice_Bloc_Sym&, DoubleVect&);

  double omega_;
  int algo_fortran_, avec_assert_;
  // Membres initialises par la methode prepare()
  // Drapeaux des items a traiter (meme structure que le second membre) (traitement des items communs si algo_items_communs_)
  IntTab items_a_traiter_;
  // pour chaque partie du vecteur, y a-t-il des items communs ?
  int algo_items_communs_;
  // Le descripteur du second membre (utiliser pour verifier qu'on a initialise l'algo)
  MD_Vector md_secmem_;
  int line_size_;
  // Precalcul de omega divise par le coefficient diagonal de la matrice
  ArrOfDouble omega_diag_;
};

#endif /* SSOR_included */

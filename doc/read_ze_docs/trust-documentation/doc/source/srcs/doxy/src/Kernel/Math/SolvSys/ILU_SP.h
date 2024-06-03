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

#ifndef ILU_SP_included
#define ILU_SP_included

#include <Precond_base.h>
#include <Matrice_Morse.h>

#define _LFIL_ILU_ 1
#define _TYPE_ILU_ 2

class ILU_SP : public Precond_base
{
  Declare_instanciable(ILU_SP);
public:
  inline int a_un_interet() { return 1; }
  inline int type_precond() { return precond_; }
protected:
  void   prepare_(const Matrice_Base&, const DoubleVect&) override;
  int preconditionner_(const Matrice_Base&, const DoubleVect&, DoubleVect&) override;

  int factorisation(const Matrice_Morse&,const DoubleVect& secmem);
  virtual int factoriser(const Matrice_Morse&, const DoubleVect&);
  int ilu(int, const DoubleVect&, DoubleVect&);
  int ilut(int, const DoubleVect&, DoubleVect&);

  ArrOfInt ju;                        // tableau pour L et U
  ArrOfDouble alu;                // tableau pour L et U
  ArrOfInt jlu;                // tableau pour L et U
  enum Type_Precond { PRECOND_NUL = 0, PRECOND_GAUCHE = 1, PRECOND_DROITE = 2, PRECOND_GAUCHE_DROITE = 3};
  int precond_ = _TYPE_ILU_;         // Flag pour le preconditionnement ILU_SP
  int lfil_= _LFIL_ILU_;             // Parametre de remplissage pour ilut
  Matrice_Morse MLOC;
};

#endif

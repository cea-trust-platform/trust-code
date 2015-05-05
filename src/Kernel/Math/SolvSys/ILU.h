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
// File:        ILU.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ILU_included
#define ILU_included

#include <Precond_base.h>
#include <Matrice_Morse.h>

#define _LFIL_ILU_ 1
#define _TYPE_ILU_ 2

class ILU : public Precond_base
{
  Declare_instanciable(ILU);
public:
  inline int a_un_interet();
  inline int type_precond();
protected:
  void   prepare_(const Matrice_Base&, const DoubleVect&);
  int preconditionner_(const Matrice_Base&, const DoubleVect&, DoubleVect&);

  int factorisation(const Matrice_Morse&,const DoubleVect& secmem);
  virtual int factoriser(const Matrice_Morse&, const DoubleVect&);
  int ilu(int, const DoubleVect&, DoubleVect&);
  int ilut(int, const DoubleVect&, DoubleVect&);

  ArrOfInt ju;                        // tableau pour L et U
  ArrOfDouble alu;                // tableau pour L et U
  ArrOfInt jlu;                // tableau pour L et U
  enum Type_Precond { PRECOND_NUL = 0, PRECOND_GAUCHE = 1, PRECOND_DROITE = 2, PRECOND_GAUCHE_DROITE = 3};
  int precond_ ;         // Flag pour le preconditionnement ILU
  int lfil_;             // Parametre de remplissage pour ilut
  Matrice_Morse MLOC;
};



inline int ILU::a_un_interet()
{
  return 1;
}

inline int ILU::type_precond()
{
  return precond_;
}



#endif

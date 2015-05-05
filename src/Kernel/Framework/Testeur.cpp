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
// File:        Testeur.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Testeur.h>
#include <Matrice_Morse_Sym.h>
#include <SSOR.h>
#include <Param.h>

Implemente_instanciable(Testeur,"Testeur",Interprete);


// printOn et readOn

Sortie& Testeur::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Testeur::readOn(Entree& is )
{
  //
  // VERIFIER ICI QU'ON A BIEN TOUT LU;
  //
  return is;
}

Entree& Testeur::interpreter(Entree& is)
{
  Param param(que_suis_je());
  Matrice_Morse_Sym m;
  SSOR ssor;
  SSOR ssor2;
  DoubleVect v;
  param.ajouter("tab1", &m.tab1_);
  param.ajouter("tab2", &m.tab2_);
  param.ajouter("coef", &m.coeff_);
  param.ajouter("ssor", &ssor);
  param.ajouter("ssor2", &ssor2);
  param.ajouter("vect", &v);
  param.lire_avec_accolades(is);

  m.m_ = v.size();
  DoubleVect w(m.m_);

  ssor.preconditionner(m, v, w);
  Cout << "Result ssor: " << w << finl;

  ssor2.preconditionner(m, v, w);
  Cout << "Result ssor2: " << w << finl;
  return is;
}

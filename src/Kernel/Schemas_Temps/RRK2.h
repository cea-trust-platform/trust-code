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
// File:        RRK2.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RRK2_included
#define RRK2_included

#include <TRUSTSchema_RK.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe RRK2
//     Cette classe represente un  schema de Runge Kutta
//     rationel d'ordre 2 qui s'ecrit:
//     U(n+1) = U(n) + (2g1 (g1,b) - b(g1,g1))/(b,b)
//     ou : g1 = dt f(U(n))
//          g2 = dt f(U(n) + c2 g1)
//          b = b1 g1 + b2 g2
//     on choisit b1=2, b2=-1, c2=1/2
// .SECTION voir aussi
//     Schema_Temps_base RK3
//
//  03/07/2017 ABn. Quelques explications RRK2:
//  Le papier de reference pour l'implementation du Rational Runge Kutta 2 semble etre :
//  Wambeck - Rational Runge-Kutta methods for solving systems of ordinary differential equations:
//  https://link.springer.com/article/10.1007/BF02252381
//  Le "rational" vient de la fraction rationnelle utilisee dans l'expansion.
//  Le reste colle exactement a l'implementation, d'apres ce que je peux voir. On y retrouve notamment la definition interessante de
//    a.b / d = a(b,d)+b(d,a)-(a,b) / ||d||^2
//  pour a, b et d vecteurs.
//  En deux mots (tires de la conclusion) :
//  Although rational methods require more computational work than linear ones, they can have some other properties, such as a stable behaviour with explicitness, which make them preferable.
//  05/07/2017 ABn. Complement explications RRK2:
//  Application du RRK2 en CFD :
//  https://link.springer.com/content/pdf/10.1007%2F3-540-13917-6_112.pdf
//  rho*u reste au temps n pour une evaluation intermediaire de f.
//  Said differently, from the time scheme perspective, f is only a function of Y1.
//////////////////////////////////////////////////////////////////////////////

class RRK2: public TRUSTSchema_RK<Ordre_RK::RATIO_DEUX>
{
  Declare_instanciable(RRK2);
public :
  int faire_un_pas_de_temps_eqn_base(Equation_base&) override;
};

#endif /* RRK2_included */

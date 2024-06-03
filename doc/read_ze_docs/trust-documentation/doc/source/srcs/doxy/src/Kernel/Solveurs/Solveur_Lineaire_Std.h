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

#ifndef Solveur_Lineaire_Std_included
#define Solveur_Lineaire_Std_included

#include <TRUSTTabs_forward.h>
#include <Solveur_lineaire.h>
#include <SolveurSys.h>
class Param;

class Equation_base;

class Solveur_Lineaire_Std : public  Solveur_lineaire
{
  Declare_instanciable(Solveur_Lineaire_Std);

public :
  bool iterer_eqn(Equation_base& equation,const DoubleTab& inconnue, DoubleTab& result, double dt, int numero_iteration, int& ok) override;

public :
  inline SolveurSys& solveur_sys(void);
  inline const  SolveurSys& solveur_sys(void)const;

protected :
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  SolveurSys solveur;

};

/*! @brief
 *
 */
inline SolveurSys& Solveur_Lineaire_Std::solveur_sys()
{
  return solveur;
}

/*! @brief
 *
 */
inline const SolveurSys& Solveur_Lineaire_Std::solveur_sys() const
{
  return solveur;
}

#endif


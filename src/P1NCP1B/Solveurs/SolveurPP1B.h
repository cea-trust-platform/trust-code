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

#ifndef SolveurPP1B_included
#define SolveurPP1B_included

#include <Assembleur_P_VEFPreP1B.h>
#include <TRUSTTabs_forward.h>
#include <SolveurSys_base.h>
class Matrice_Base;

class SolveurPP1B : public SolveurSys_base
{
  Declare_instanciable(SolveurPP1B);
public :
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect&) override;
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect&,  int niter_max) override;

  inline void reinit() override
  {
    SolveurSys_base::reinit();
    solveur_pression_.valeur().reinit();
  }

  int associer(const Assembleur_P_VEFPreP1B&, const SolveurSys&);
  inline int solveur_direct() const override { return solveur_pression_.valeur().solveur_direct(); }
  inline void fixer_schema_temps_limpr(int l) override { solveur_pression_.valeur().fixer_schema_temps_limpr(l); }

protected :
  SolveurSys solveur_pression_;
  Assembleur_P_VEFPreP1B assembleur_pression_;
};

#endif

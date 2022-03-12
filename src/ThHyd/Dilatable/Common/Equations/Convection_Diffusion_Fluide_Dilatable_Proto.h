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
// File:        Convection_Diffusion_Fluide_Dilatable_Proto.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Convection_Diffusion_Fluide_Dilatable_Proto_included
#define Convection_Diffusion_Fluide_Dilatable_Proto_included

#include <TRUSTTrav.h>

class Convection_Diffusion_Fluide_Dilatable_base;
class Convection_Diffusion_std;
class Fluide_Dilatable_base;
class Probleme_base;
class Matrice_Morse;
class Champ_Inc;
class Sortie;
class Entree;

class Convection_Diffusion_Fluide_Dilatable_Proto
{
public:
  // E Saikali
  // Convection_Diffusion_std parce que on a un heritage V et on va appeler la classe mere de l'autre cote ...
  // Sinon il faut mettre les methodes dans Equation_base... a voir ...
  // [ Vive les classes templates ... dommage !]
  static int Sauvegarder_WC(Sortie& os, const Convection_Diffusion_std& eq, const Fluide_Dilatable_base& fld);
  static int Reprendre_WC(Entree& is, double temps,Convection_Diffusion_std& eq, Fluide_Dilatable_base& fld,
                          Champ_Inc& inco, Probleme_base& pb);

  virtual ~Convection_Diffusion_Fluide_Dilatable_Proto() {}

protected:
  void calculer_div_rho_u_impl(DoubleTab& res, const Convection_Diffusion_Fluide_Dilatable_base& eqn ) const;
  void assembler_impl(Convection_Diffusion_Fluide_Dilatable_base& eqn,
                      Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem);

  DoubleTab& derivee_en_temps_inco_sans_solveur_masse_impl(Convection_Diffusion_Fluide_Dilatable_base& eqn,
                                                           DoubleTab& derivee, const bool is_expl);

  virtual void calculer_div_u_ou_div_rhou(DoubleTab& res) const = 0;
  virtual bool is_thermal() const = 0;
  virtual bool is_generic() const = 0;
};

#endif /* Convection_Diffusion_Fluide_Dilatable_Proto_included */

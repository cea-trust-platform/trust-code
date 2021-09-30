/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <Ref_Fluide_Dilatable_base.h>
#include <Ref_Schema_Temps_base.h>
#include <Ref_Equation_base.h>
#include <Ref_Zone_Cl_dis.h>
#include <Ref_Champ_Inc.h>

class Convection_Diffusion_std;
class Probleme_base;
class Solveur_Masse;
class Equation_base;
class Matrice_Morse;
class DoubleTab;
class Operateur;
class Sources;

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

protected:
  void completer_common_impl(const Champ_Inc& inco, const Zone_Cl_dis& zcl_dis_modif,
                             const Zone_Cl_dis& zcl_dis, const Schema_Temps_base& sch,
                             const Fluide_Dilatable_base& fld, Equation_base& eq);

  void assembler_impl(Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem,
                      const Operateur& op_diff,const Operateur& op_conv, const Sources& src,
                      Solveur_Masse& solv);

  DoubleTab& derivee_en_temps_inco_sans_solveur_masse_imp(DoubleTab& derivee, const Operateur& op_diff,
                                                          const Operateur& op_conv, const Sources& src,
                                                          Solveur_Masse& solv, const bool is_expl);

private:
  void calculer_div_rho_u_impl(DoubleTab& res, const Operateur& op_conv) const;
  REF(Zone_Cl_dis) zcl_dis_modif_, zcl_dis_;
  REF(Fluide_Dilatable_base) fld_;
  REF(Schema_Temps_base) sch_;
  REF(Equation_base) eq_;
  REF(Champ_Inc) inco_;
};

#endif /* Convection_Diffusion_Fluide_Dilatable_Proto_included */

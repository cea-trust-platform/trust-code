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

#ifndef Terme_Source_Rappel_T_VEF_Face_included
#define Terme_Source_Rappel_T_VEF_Face_included

#include <Ref_Champ_Inc_base.h>
#include <TRUSTTabs_forward.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Ref_Zone_VEF.h>
#include <Source_base.h>
#include <Parser_U.h>

class Probleme_base;
class Navier_Stokes_std;

class Terme_Source_Rappel_T_VEF_Face : public Source_base
{
  Declare_instanciable(Terme_Source_Rappel_T_VEF_Face);

public :

  void associer_pb(const Probleme_base& ) override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& calculer(DoubleTab& ) const override;
  void completer() override;
  void mettre_a_jour(double temps) override {  } ;

protected :
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) override;
  REF(Zone_VEF) la_zone_VEF;
  REF(Zone_Cl_VEF) la_zone_Cl_VEF;

private :

  double alpha_tau;
  int fct_ok;
  mutable Parser_U force_rappel;

  REF(Champ_Inc_base) l_inconnue;
  REF(Zone_VEF) zone_VEF_autre_pb;
  Nom nom_pb, nom_autre_pb;
  Motcle nom_inco;

  ArrOfInt corresp_fin_gros; // correspondance elem de l'autre pb vers elem du pb courant
  ArrOfInt nb_fin; // nb d'elements de l'autre pb par element du pb courant
  ArrOfDouble vol_fin_tot; // volume totale des elements fins s'appuyant un element grossier

};
#endif

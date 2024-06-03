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

#ifndef Terme_Source_Rappel_T_VEF_Face_included
#define Terme_Source_Rappel_T_VEF_Face_included

#include <TRUSTTabs_forward.h>
#include <Source_base.h>
#include <TRUST_Ref.h>
#include <Parser_U.h>

class Navier_Stokes_std;
class Champ_Inc_base;
class Probleme_base;
class Domaine_Cl_VEF;
class Domaine_VEF;

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
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;

private :

  double alpha_tau=1.;
  int fct_ok=-1;
  mutable Parser_U force_rappel;

  REF(Champ_Inc_base) l_inconnue;
  REF(Domaine_VEF) domaine_VEF_autre_pb;
  Nom nom_pb, nom_autre_pb;
  Motcle nom_inco;

  ArrOfInt corresp_fin_gros; // correspondance elem de l'autre pb vers elem du pb courant
  ArrOfInt nb_fin; // nb d'elements de l'autre pb par element du pb courant
  ArrOfDouble vol_fin_tot; // volume totale des elements fins s'appuyant un element grossier

};
#endif

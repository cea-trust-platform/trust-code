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

#ifndef Terme_Source_Canal_RANS_LES_VEF_Face_included
#define Terme_Source_Canal_RANS_LES_VEF_Face_included

#include <TRUSTTabs_forward.h>
#include <Source_base.h>
#include <Champ_Don.h>
#include <TRUST_Ref.h>

class Navier_Stokes_std;
class Probleme_base;
class Domaine_Cl_VEF;
class Domaine_VEF;

/*! @brief class Terme_Source_Canal_RANS_LES_VEF_Face Cette classe concerne un terme source calcule en partie grace a un calcul RANS preliminaire et applique au calcul LES en cours
 *
 */
class Terme_Source_Canal_RANS_LES_VEF_Face : public Source_base
{
  Declare_instanciable_sans_destructeur(Terme_Source_Canal_RANS_LES_VEF_Face);

public :
  ~Terme_Source_Canal_RANS_LES_VEF_Face() override;

  void associer_pb(const Probleme_base& ) override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& calculer(DoubleTab& ) const override;
  void init();
  void mettre_a_jour(double ) override;

protected :
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;

private :
  int moyenne= 0; //type de moyenne
  int dir = -10; //direction
  Nom dir_nom; // axe
  double alpha_tau = -100., Ly= -100., u_tau= -100., nu= -100., rayon= -100.;
  double t_moy_start= -100.,f_start= 0.,f_tot= -100.;
  int u_target= 0;

  DoubleTab U_RANS;
  DoubleTab utemp;
  DoubleTab utemp_sum;

};
#endif

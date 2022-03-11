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
// File:        Traitement_particulier_NS_Profils_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Traitement_particulier
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_Profils_VDF_included
#define Traitement_particulier_NS_Profils_VDF_included

#include <Traitement_particulier_NS_Profils.h>
#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class Trait_part_NS_Profils_VDF
//     This classe enables a particular treatment
//     for computations which have a periodic BC (ie homogeneous direction)
//     and on which we want profiles in a specific direction and position :
//         + mean velocity profiles
//         + fluctuations
//
// .SECTION see also
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VDF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_Profils_VDF : public Traitement_particulier_NS_Profils
{
  Declare_instanciable(Traitement_particulier_NS_Profils_VDF);

public :

  void post_traitement_particulier(void) override ;
  Entree& lire(const Motcle& , Entree& ) override;
  Entree& lire(Entree& ) override;

protected :
  void init_calcul_moyenne(void) override;
  void calculer_moyenne_spatiale_vitesse(DoubleTab& , DoubleTab& ,DoubleTab& , DoubleTab& , const IntTab& , const IntTab& , const IntVect& , const int& , const DoubleTab& );
  void calculer_moyenne_spatiale_uv(DoubleTab& , const IntTab& , const IntTab& , const IntVect& , const DoubleTab& );
  void ecriture_fichiers_moy_spat(const DoubleTab& ,const DoubleTab& , const DoubleTab& , const DoubleTab& ,const DoubleTab& ,const DoubleTab& , const DoubleTab& , const DoubleTab& , const DoubleVect& , const DoubleVect& , const IntVect& , const DoubleTab& , const int& );
  void ecriture_fichiers_moy_spat_1col(const DoubleTab& , const DoubleTab& , const DoubleTab& , const DoubleVect& , const DoubleVect& , const IntVect& , const int& );
  void calculer_moyenne_spatiale_nut(DoubleTab& , const IntTab& , const IntTab& , const IntVect& , const DoubleTab& );
  void calculer_integrale_temporelle(DoubleTab& , DoubleTab& , DoubleTab& , const DoubleVect& , const DoubleVect&);
  void ecriture_fichiers_moy_temp(const DoubleTab& , const DoubleTab& , const DoubleTab& , const DoubleTab& ,const DoubleTab& , const IntVect& , const int&, const double& );
  void ecriture_fichiers_moy_temp_1col(const DoubleTab& , const DoubleTab& , const IntVect& , const int&, const double& );


protected:
  DoubleTab Yu_m,Yv_m,Yw_m,Yuv_m;                             // Coordonnees des points ou est definie la vitesse.
  DoubleTab Yu_p,Yv_p,Yw_p,Yuv_p;
  IntTab compt_x_m,compt_y_m,compt_z_m,compt_uv_m;            // Nombre de points a "meme Y" pour chaque grandeur.
  IntTab compt_x_p,compt_y_p,compt_z_p,compt_uv_p;
  IntTab corresp_u_m, corresp_v_m, corresp_w_m, corresp_uv_m; // Correspondance numerotation globale -> numerotation locale dans Yxx
  IntTab corresp_u_p, corresp_v_p, corresp_w_p, corresp_uv_p;

  DoubleTab xUm,xUp,xWm,xWp,xVm,xVp,xUVm,xUVp;               // Positions des points juste avant et apres.
  DoubleVect delta_Um,delta_Up,delta_Wm,delta_Wp,delta_Vm,delta_Vp,delta_UVm,delta_UVp;  // Distance (tjrs positive) entre le point avant ou apres et le point du profil, pour faire les interpolations lineaires et trouver les points en question.

};
#endif

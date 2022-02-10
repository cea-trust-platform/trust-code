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
// File:        Traitement_particulier_NS_Profils.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Traitement_particulier
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_Profils_included
#define Traitement_particulier_NS_Profils_included

#include <Traitement_particulier_NS_base.h>


//////////////////////////////////////////////////////////////////////////////
//
//     class Trait_part_NS_Profils_VDF
//     This classe enables a particular treatment
//     for computations which have a periodic BC (ie homogeneous direction)
//     and on which we want profiles in a specific direction and position :
//         + mean velocity profiles
//         + fluctuations
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VDF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_Profils : public Traitement_particulier_NS_base
{
  Declare_base(Traitement_particulier_NS_Profils);

public :
  Entree& lire(Entree& ) override;
  void preparer_calcul_particulier(void) override ;

  virtual Entree& lire(const Motcle& , Entree& ) =0;
  void post_traitement_particulier(void) override =0 ;
  void reprendre_stat() override;
  void sauver_stat() const override;

  inline void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) override ;

protected :
  void init_calcul_stats(void);

  virtual void init_calcul_moyenne(void) = 0;

  DoubleTab u_moy_temp_x,u_moy_temp_y,u_moy_temp_z;
  DoubleTab uv_moy_temp,u2_moy_temp,v2_moy_temp,w2_moy_temp,u3_moy_temp,v3_moy_temp,w3_moy_temp,u4_moy_temp,v4_moy_temp,w4_moy_temp,nu_t_temp;
  double temps_deb,temps_fin;
  double dt_post_stat,dt_post_inst;
  IntVect Nxy,Nyy,Nzy,Nuv;
  int Nap;
  Nom fich_repr_u,fich_repr_nu_t;
  int oui_repr,oui_u_inst, oui_stat,oui_profil_nu_t,oui_repr_nu_t;
  int n_probes;
  DoubleVect positions;
  int homo_dir,dir_profil;

};
#endif

inline void Traitement_particulier_NS_Profils::en_cours_de_resolution(int i, DoubleTab& a, DoubleTab& b,double c)
{
  ;
}

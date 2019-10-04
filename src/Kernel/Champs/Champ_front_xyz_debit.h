/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_front_xyz_debit.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_xyz_debit_included
#define Champ_front_xyz_debit_included

#include <Champ_front_normal.h>
#include <Parser_U.h>
#include <Param.h>
#include <Ch_front_var_instationnaire_dep.h>
#include <Champ_front.h>
#include <Zone_VF.h>
#include <Champ_front_uniforme.h>
#include <Ch_front_input_uniforme.h>
//.DESCRIPTION  class Champ_front_xyz_debit
//


class Champ_front_xyz_debit : public Champ_front_normal
{
  Declare_instanciable(Champ_front_xyz_debit);

protected:
  Champ_front velocity_profil_;
  Champ_front flow_rate_;
  // in TRUST the normal vector to a surface is stocked weighted by the area of the surface via "face_normales"
  // here we want normal vectors only, formely known as normales_divisees_par_aire_
  // We need the normal vector (only) because the velocity field will be perpendicular the surface
  DoubleTab normal_vectors_;
  double coeff_;
  double integrale_;
  bool flow_rate_alone_=0;

  virtual void initialiser_coefficient(const Champ_Inc_base& inco);
  void calculer_normales_et_integrale(const Front_VF& le_bord, DoubleTab& velocity_user);
  void calculer_champ_vitesse(const Front_VF& le_bord, DoubleTab& velocity_field, double flow_rate, DoubleTab& velocity_user);

public:
  virtual int initialiser(double tps, const Champ_Inc_base& inco);
  void mettre_a_jour(double temps);
  void associer_fr_dis_base(const Frontiere_dis_base& fr);
  void set_temps_defaut(double temps);
  void fixer_nb_valeurs_temporelles(int nb_cases);
  void changer_temps_futur(double temps,int i);
  int avancer(double temps);
  int reculer(double temps);

};

#endif

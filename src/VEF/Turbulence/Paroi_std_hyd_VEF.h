/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_std_hyd_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

// .NOM Paroi_std_hyd_VEF :
// .ENTETE TRUST VEF Turbulence
// .LIBRAIRIE libhydVEFturb
// .FILE Paroi_std_hyd_VEF.h
// .FILE Paroi_std_hyd_VEF.cpp

#ifndef Paroi_std_hyd_VEF_included
#define Paroi_std_hyd_VEF_included

#include <Paroi_hyd_base_VEF.h>
#include <distances_VEF.h>
#include <Paroi_log_QDM.h>

class Champ_Fonc_base;
class Zone_dis;
class Zone_Cl_dis;

//.DESCRIPTION
//
// CLASS: Paroi_std_hyd_VEF
//

//.SECTION  voir aussi
// Turbulence_paroi_base
void remplir_face_keps_imposee(int& flag_face_keps_imposee_,int methode_calcul_face_keps_impose_, IntVect& face_keps_imposee_, const Zone_VEF& zone_VEF,const REF(Zone_Cl_VEF) la_zone_Cl_VEF,int is_champ_P1NC);


class Paroi_std_hyd_VEF : public Paroi_hyd_base_VEF, public Paroi_log_QDM
{

  Declare_instanciable_sans_constructeur(Paroi_std_hyd_VEF);

public:

  Paroi_std_hyd_VEF();
  virtual void set_param(Param& param);
  virtual int init_lois_paroi();
  int calculer_hyd(DoubleTab& );
  int calculer_hyd(DoubleTab& , DoubleTab& );

  void imprimer_ustar(Sortie& ) const;
  double calculer_u_plus(const int& ,const double& ,const  double& erugu );

  virtual int calculer_k_eps(double& , double& , double , double , double , double);


protected:

  int methode_calcul_face_keps_impose_;  // 0 std avant: 1 toutes les faces accroches 2:: comme avant mais toutes les faces des elts accroches. 3: comme avant sans test si bord...  4: que les faces des elts acrroches

  DoubleVect uplus_;
  DoubleVect dplus_;

  DoubleVect seuil_LP;
  IntVect iterations_LP;

  double u_star_impose_;
  int is_u_star_impose_;
  virtual int init_lois_paroi_hydraulique();
};


// Description: cette classe permet de specifier des options a la loi de
// paroi standard. Elle est reservee aux experts.
class Loi_expert_hydr_VEF : public Paroi_std_hyd_VEF
{
  Declare_instanciable(Loi_expert_hydr_VEF);
};

#endif

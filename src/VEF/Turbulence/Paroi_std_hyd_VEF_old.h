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
// File:        Paroi_std_hyd_VEF_old.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Paroi_std_hyd_VEF_old_included
#define Paroi_std_hyd_VEF_old_included

#include <Paroi_hyd_base_VEF.h>
#include <distances_VEF.h>
#include <Paroi_log_QDM.h>

class Champ_Fonc_base;
class Zone_dis;
class Zone_Cl_dis;
class Param;

//.DESCRIPTION
//
// CLASS: Paroi_std_hyd_VEF_old
//

//.SECTION  voir aussi
// Turbulence_paroi_base

class Paroi_std_hyd_VEF_old : public Paroi_hyd_base_VEF, public Paroi_log_QDM
{
  Declare_instanciable(Paroi_std_hyd_VEF_old);

public:

  virtual void set_param(Param& param);
  virtual int init_lois_paroi();
  int calculer_hyd(DoubleTab& );
  int calculer_hyd(DoubleTab& , DoubleTab& );

  inline const DoubleVect& tab_u_plus() const;
  inline double tau_tang(int face,int k) const;
  void imprimer_ustar(Sortie& ) const;
  void calculer_uplus_dplus(DoubleVect& ,DoubleVect& ,DoubleVect& ,int ,double ,double ,double ) ;

protected:

  DoubleVect uplus_;
  DoubleVect dplus_;

  virtual int init_lois_paroi_hydraulique();
  int preparer_calcul_hyd(DoubleTab& );
  double calculer_u_star_sous_couche_visq(double ,double ,double ,int );
  int calculer_sous_couche_visq(DoubleTab& ,int );
  int calculer_sous_couche_visq(DoubleTab& ,DoubleTab& , int );
  double calculer_u_star_sous_couche_tampon(double& ,double ,double,double ,int );
  int calculer_sous_couche_tampon(double, DoubleTab& ,double ,double ,int ,int  );
  int calculer_sous_couche_tampon(double, DoubleTab& ,DoubleTab& , double ,double ,int ,int );
  double calculer_u_star_sous_couche_log(double ,double ,double ,int );
  int calculer_sous_couche_log(double, DoubleTab& ,double ,int ,int );
  int calculer_sous_couche_log(double, DoubleTab& ,DoubleTab& , double ,int ,int );
  double calculer_local(double ,double, DoubleTab&  ,double ,double ,int ,int );
  double calculer_local(double ,double, DoubleTab&  ,DoubleTab& , double ,double ,int ,int );
};

///////////////////////////////////////////////////////////
//
//  Fonctions inline de la classe Paroi_std_hyd_VEF_old
//
///////////////////////////////////////////////////////////

////inline Paroi_std_hyd_VEF_old::Paroi_std_hyd_VEF_old()
////  : table_hyd() , tab_u_star_() , Cisaillement_paroi_() {}

inline const DoubleVect& Paroi_std_hyd_VEF_old::tab_u_plus() const
{
  return uplus_;
}

inline double Paroi_std_hyd_VEF_old::tau_tang(int face, int k) const
{
  return Cisaillement_paroi_(face,k);
}


#endif

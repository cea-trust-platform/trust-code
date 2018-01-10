/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Paroi_std_hyd_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Paroi_std_hyd_VDF_included
#define Paroi_std_hyd_VDF_included

#include <Paroi_hyd_base_VDF.h>
#include <Paroi_log_QDM.h>

class Champ_Fonc_base;
class Zone_dis;
class Zone_Cl_dis;
class Param;

//.DESCRIPTION
//
// CLASS: Paroi_std_hyd_VDF
//

//.SECTION  voir aussi
// Turbulence_paroi_base
class Paroi_std_hyd_VDF : public Paroi_hyd_base_VDF, public Paroi_log_QDM
{
  Declare_instanciable(Paroi_std_hyd_VDF);

public:

  virtual void set_param(Param& param);
  virtual int init_lois_paroi();
  int calculer_hyd(DoubleTab& );
  int calculer_hyd(DoubleTab& , DoubleTab& );
  int calculer_hyd(DoubleTab& , int isKeps,DoubleTab& );
  inline const DoubleVect& tab_u_plus() const
  {
    return uplus_;
  };
  inline double tau_tang(int face,int k) const;
  void imprimer_ustar(Sortie& ) const;
  void calculer_uplus_dplus(DoubleVect& ,DoubleVect& ,DoubleVect& ,int ,double ,double ,double ) ;

protected:
  DoubleVect uplus_;

  virtual int init_lois_paroi_hydraulique();
  virtual int preparer_calcul_hyd(DoubleTab& );
  int calculer_u_star_sous_couche_visq(double ,double ,double ,int );
  int calculer_sous_couche_visq(DoubleTab& ,double,int,double,int );
  int calculer_sous_couche_visq(DoubleTab& ,DoubleTab& , int );
  double calculer_u_star_sous_couche_visq(double ,double ,double );
  int calculer_u_star_sous_couche_tampon(double& ,double ,double,double ,int );
  int calculer_sous_couche_tampon(DoubleTab& ,double ,double ,int ,int  );
  int calculer_sous_couche_tampon(DoubleTab& ,DoubleTab& , double ,double ,int ,int );
  double calculer_u_star_sous_couche_tampon(double ,double ,double, double& );
  int calculer_u_star_sous_couche_log(double ,double ,double ,int );
  virtual double calculer_u_star(double&, double&, double&);
  int calculer_sous_couche_log(DoubleTab& ,double ,int ,int );
  int calculer_sous_couche_log(DoubleTab& ,DoubleTab& , double ,int ,int );
  int calculer_local(double ,double, DoubleTab&  ,double ,double ,int ,int );
  int calculer_local(double ,double, DoubleTab&  ,DoubleTab& , double ,double ,int ,int );
  double calculer_local(double ,double ,double ,double,int&, double&  );
  void  calculer_moyennes_parois(double&,double&,double&,double&,double&,double&);
  void  modifs_valeurs_turb(int ,int , double , double , double , double, DoubleTab& ,DoubleTab& );
};

inline double Paroi_std_hyd_VDF::tau_tang(int face, int k) const
{
  if(face>=Cisaillement_paroi_.dimension(0))
    face-=la_zone_VDF.valeur().nb_faces_internes();
  if(face>=Cisaillement_paroi_.dimension_tot(0))
    {
      Cerr << "Erreur dans tau_tang " << finl;
      Cerr << "dimension : " << Cisaillement_paroi_.dimension(0) << finl;
      Cerr << "dimension_tot : " << Cisaillement_paroi_.dimension_tot(0) << finl;
      Cerr << "face : " << face << finl;
      exit();
    }
  return Cisaillement_paroi_(face,k);
}

// Description: cette classe permet de specifier des options a la loi de
// paroi standard. Elle est reservee aux experts.
class Loi_expert_hydr_VDF : public Paroi_std_hyd_VDF
{
  Declare_instanciable(Loi_expert_hydr_VDF);
};
#endif

/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Paroi_loi_WW_hyd_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Paroi_loi_WW_hyd_VEF_included
#define Paroi_loi_WW_hyd_VEF_included


#include <Paroi_std_hyd_VEF.h>
class Param;

class Paroi_loi_WW_hyd_VEF : public Paroi_std_hyd_VEF
{

  // appel a la macro pour une classe instanciable
  // dont le constructeur doit etre code
  Declare_instanciable_sans_constructeur(Paroi_loi_WW_hyd_VEF);

  /* PUBLIC */
public:
  inline Paroi_loi_WW_hyd_VEF();
  virtual void set_param(Param& param);

  /* calculer_hyd pour k-epsilon qui renvoie une erreur */
  int calculer_hyd(DoubleTab& );

  /* calculer_hyd pour L.E.S */
  int calculer_hyd(DoubleTab& , DoubleTab& );


  /* PROTECTED */
protected :

  /* constante pour la loi en WWance */
  double A ;
  double B  ;

  double Y0 ;

  int init_lois_paroi_hydraulique();
  int preparer_calcul_hyd(DoubleTab& );
  int calculer_u_star_sous_couche_visq(double ,double ,double ,int );
  int calculer_u_star_couche_puissance(double ,double ,double ,int );
  int calculer_couche_puissance(DoubleTab& ,DoubleTab&  ,double ,int ,int );
  int calculer_local(double, DoubleTab&  ,DoubleTab& , double , double,double ,int ,int );
  int impr;
};
inline Paroi_loi_WW_hyd_VEF::Paroi_loi_WW_hyd_VEF():impr(0) { }
double norm_2D_vit(const DoubleTab& vit,int num1,int num2,int fac,const Zone_VEF& zone,double& u,double& val1, double& val2);
double norm_2D_vit3(const DoubleTab& vit,int num1,int num2,int num3,int fac,const Zone_VEF& zone,double& u,double& val1, double& val2);
double norm_3D_vit(const DoubleTab& vit,int fac,int num1,int num2,int num3,const Zone_VEF& zone,double& val1,double& val2,double& val3);
double norm_3D_vit4(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,const Zone_VEF& zone,double& val1,double& val2,double& val3);
double distance_2D(int fac,int elem,const Zone_VEF& zone,double& signe);
double distance_2D_som(int fac,int elem,const Zone_VEF& zone);
double distance_3D(int fac,int elem,const Zone_VEF& zone,double& signe);
double distance_3D_som(int fac,int elem,const Zone_VEF& zone);


#endif


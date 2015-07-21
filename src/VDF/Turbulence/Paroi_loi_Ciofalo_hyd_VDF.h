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
// File:        Paroi_loi_Ciofalo_hyd_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Paroi_loi_Ciofalo_hyd_VDF_included
#define Paroi_loi_Ciofalo_hyd_VDF_included

#include <Paroi_std_hyd_VDF.h>

class Paroi_loi_Ciofalo_hyd_VDF : public Paroi_std_hyd_VDF
{

  // appel a la macro pour une classe instanciable
  // dont le constructeur doit etre code
  Declare_instanciable_sans_constructeur(Paroi_loi_Ciofalo_hyd_VDF);

  /* PUBLIC */
public:

  /* calculer_hyd pour k-epsilon qui renvoie une erreur */
  int calculer_hyd(DoubleTab& );

  /* calculer_hyd pour L.E.S */
  int calculer_hyd(DoubleTab& , DoubleTab& );

  /* PROTECTED */
protected :

  /* constante pour la loi de Ciofalo & Collins */

  double Y0 ;

  int init_lois_paroi_hydraulique();
  int preparer_calcul_hyd(DoubleTab& );
  int calculer_u_star_sous_couche_visq(double,double,double,int);
  int calculer_u_star_couche_log(double ,double ,double ,int, double );
  int calculer_local(double ,double, DoubleTab&  ,DoubleTab& , double ,double ,int ,int );
};

#endif


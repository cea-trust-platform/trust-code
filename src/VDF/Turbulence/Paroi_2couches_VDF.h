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
// File:        Paroi_2couches_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Paroi_2couches_VDF_included
#define Paroi_2couches_VDF_included

#include <Paroi_hyd_base_VDF.h>
#include <Paroi_log_QDM.h>

class Champ_Fonc_base;
class Zone_dis;
class Zone_Cl_dis;
class Param;

//.DESCRIPTION
//
// CLASS: Paroi_2couches_VDF
//

//.SECTION  voir aussi
// Turbulence_paroi_base
class Paroi_2couches_VDF : public Paroi_hyd_base_VDF, public Paroi_log_QDM
{

  Declare_instanciable_sans_constructeur(Paroi_2couches_VDF);

public:

  virtual void set_param(Param& param);
  virtual int init_lois_paroi();
  int calculer_hyd(DoubleTab& );
  int calculer_hyd(DoubleTab& , DoubleTab& );
  void calculer_uplus_dplus(DoubleVect& ,DoubleVect& ,DoubleVect& ,int ,double ,double ,double ) ;

  inline const DoubleVect& tab_u_plus() const;
  inline double tab_u_star(int face) const;
  void imprimer_ustar(Sortie& os) const;

  bool use_shear() const
  {
    return false;
  }
protected:


  double Cmu;
  double Kappa;                         // Constante de Karman
  double Erugu;                                // La valeur du parametre Erugu depend
  // de la rugosite de la paroi
  double A_plus;

  DoubleVect uplus_;
  DoubleVect dplus_;

  int init_lois_paroi_hydraulique();
  int calculer_u_star_sous_couche_tampon(double& ,double ,double,double ,int );
  int calculer_u_star_sous_couche_log(double ,double ,double ,int );

};

///////////////////////////////////////////////////////////
//
//  Fonctions inline de la classe Paroi_2couches_VDF
//
///////////////////////////////////////////////////////////

inline const DoubleVect& Paroi_2couches_VDF::tab_u_plus() const
{
  return uplus_;
}

inline double Paroi_2couches_VDF::tab_u_star(int face) const
{
  return tab_u_star_(face);
}

#endif

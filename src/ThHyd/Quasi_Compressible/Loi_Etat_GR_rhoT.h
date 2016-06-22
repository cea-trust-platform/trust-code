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
// File:        Loi_Etat_GR_rhoT.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_Etat_GR_rhoT_included
#define Loi_Etat_GR_rhoT_included

#ifndef RU_
//constante des gaz
#define RU_ 8.314*4.18448
#endif

#include <Loi_Etat_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Loi_Etat_GR_rhoT
//     Cette classe represente la loi d'etat pour les gaz reels.
//     Associe a un fluide incompressible, elle definit un fluide quasi compressible
//     dont la loi d'etat est :
//         rho=rho(Pth,H)
//         T  =  T(Pth,H)
// .SECTION voir aussi
//     Fluide_Quasi_Compressible Loi_Etat_base
//////////////////////////////////////////////////////////////////////////////

class Loi_Etat_GR_rhoT : public Loi_Etat_base
{
  Declare_instanciable_sans_constructeur(Loi_Etat_GR_rhoT);

public :

  Loi_Etat_GR_rhoT();
  inline double masse_molaire() const;
  double Cp_calc(double,double) const;
  const Nom type_fluide() const;

  void initialiser();
  void initialiser_inco_ch();
  void remplir_T();
  void calculer_Cp();
  void calculer_lambda();

  virtual void calculer_masse_volumique();
  double calculer_masse_volumique(double,double) const;
  double calculer_temperature(double,double);
  double calculer_H(double,double) const;
  double inverser_Pth(double,double);

  double Drho_DP(double,double) const ;
  double Drho_DT(double,double) const ;
  double De_DP(double,double) const ;
  double De_DT(double,double) const ;

  double DT_DH(double,double) const ;

protected :
  DoubleTab PolyRho_;
  DoubleTab PolyT_;
  double MMole_;
  DoubleTab tab_TempC;
  DoubleTab tab_Cp;

  double Cp_;
  double R;
};

inline double Loi_Etat_GR_rhoT::masse_molaire() const
{
  return MMole_;
}

#endif

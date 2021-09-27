/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Loi_Etat_Binaire_GP_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Loi_Etat
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_Etat_Binaire_GP_base_included
#define Loi_Etat_Binaire_GP_base_included

#include <Loi_Etat_Melange_GP_base.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Loi_Etat_Binaire_GP_base
//     Cette classe represente la loi d'etat pour les melanges binaires.
//     Elle definit un fluide binaire dilatable dont la loi d'eata est :
//        Pth = rho*R*T*(Y1/M1+Y2/M2)
// .SECTION voir aussi
//     Loi_Etat_Melange_GP_base
//////////////////////////////////////////////////////////////////////////////

class Loi_Etat_Binaire_GP_base : public Loi_Etat_Melange_GP_base
{
  Declare_base_sans_constructeur(Loi_Etat_Binaire_GP_base);
public:
  Loi_Etat_Binaire_GP_base();
  void calculer_lambda();
  void calculer_mu();
  void calculer_mu_wilke();
  void calculer_alpha();
  void calculer_mu_sur_Sc(); // returns rho * D
  void calculer_nu_sur_Sc(); // returns D
  void calculer_Cp();
  const Nom type_fluide() const;
  double inverser_Pth(double,double);
  virtual double calculer_masse_volumique(double P,double Y1) const;
  // Methode virtuelle pure
  virtual void calculer_masse_volumique()=0;

protected:
  double massmol1_,massmol2_,mu1_,mu2_,tempr_,diff_coeff_;
};

#endif /* Loi_Etat_Binaire_GP_base_included */

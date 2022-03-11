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
// File:        Loi_Etat_rhoT_GP_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Loi_Etat
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_Etat_rhoT_GP_QC_included
#define Loi_Etat_rhoT_GP_QC_included

#include <Loi_Etat_GP_base.h>
#include <Champ_Don.h>
#include <TRUSTTab.h>
#include <Parser_U.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Loi_Etat_rhoT_GP_QC
//
// <Description of class Loi_Etat_rhoT_GP_QC>
//
/////////////////////////////////////////////////////////////////////////////

class Loi_Etat_rhoT_GP_QC : public Loi_Etat_GP_base
{
  Declare_instanciable_sans_constructeur( Loi_Etat_rhoT_GP_QC ) ;

public :
  Loi_Etat_rhoT_GP_QC();
  void initialiser_inco_ch() override;
  void initialiser_rho();
  void calculer_masse_volumique() override;
  double calculer_masse_volumique(double, double) const override; // overrided
  double calculer_masse_volumique(double, double, int) const;// overloaded
  double inverser_Pth(double T, double rho) override; // forbidden

protected :
  bool is_exp_;
  Champ_Don rho_xyz_;
  DoubleTab rho_;
  mutable Parser_U  parser_;
};

#endif /* Loi_Etat_rhoT_GP_QC_included */

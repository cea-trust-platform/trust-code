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
// File:        Loi_Etat_Multi_GP_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Loi_Etat
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_Etat_Multi_GP_QC_included
#define Loi_Etat_Multi_GP_QC_included

#include <Convection_Diffusion_Espece_Multi_QC.h>
#include <Loi_Etat_Multi_GP_base.h>
#include <Ref_Espece.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Loi_Etat_Multi_GP_QC
//     Cette classe represente la loi d'etat pour un melange de gaz parfaits.
//
// .SECTION voir aussi
//     Fluide_Dilatable_base Loi_Etat_base Loi_Etat_Multi_GP_base
//////////////////////////////////////////////////////////////////////////////

Declare_liste(REF(Espece));

class Loi_Etat_Multi_GP_QC : public Loi_Etat_Multi_GP_base
{
  Declare_instanciable_sans_constructeur(Loi_Etat_Multi_GP_QC);

public :
  Loi_Etat_Multi_GP_QC();
  void associer_espece(const Convection_Diffusion_Espece_Multi_QC& eq);
  void calculer_masse_molaire(DoubleTab& M) const;
  void calculer_tab_Cp(DoubleTab& cp) const;
  void calculer_masse_volumique();
  void calculer_mu_sur_Sc();
  double calculer_masse_volumique(double,double) const;
  double calculer_masse_volumique(double P,double T,double r) const;

protected :
  void rabot(int futur = 0);
  LIST(REF(Espece)) liste_especes;
  int correction_fraction_,ignore_check_fraction_;
  double Sc_,dtol_fraction_;

private:
  void calculer_mu_wilke();
};

#endif /* Loi_Etat_Multi_GP_QC_included */

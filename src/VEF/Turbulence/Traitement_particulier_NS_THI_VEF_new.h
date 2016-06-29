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
// File:        Traitement_particulier_NS_THI_VEF_new.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_THI_VEF_new_included
#define Traitement_particulier_NS_THI_VEF_new_included

#include <Traitement_particulier_NS_THI_new.h>
#include <Op_Diff_VEF_Face.h>


//////////////////////////////////////////////////////////////////////////////
// 7
// .DESCRIPTION
//     classe Traitement_particulier_NS_THI_VEF_new
//     Cette classe permet de faire les traitements particuliers
//     pour les calculs de  THI, en particulier initialisation
//     particuliere et calculs des spectres!!
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VEF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_THI_VEF_new : public Traitement_particulier_NS_THI_new
{
  Declare_instanciable(Traitement_particulier_NS_THI_VEF_new);


protected :

  void renorm_Ec(void);
  void init_calc_spectre(void);
  void calcul_spectre(void);

private:

  void determine_new_tab_fft_VEF(void);
  void calculer_spectre_new(DoubleTab&, DoubleTab&, DoubleTab&, int, int, double, DoubleVect& , double& , DoubleVect& , double&);
  void calculer_spectre_new_s(DoubleTab&, DoubleTab&, DoubleTab&, int, int, double, DoubleVect& , double& , DoubleVect& , double&);

  void ch_vit_pour_fft_VEF(DoubleTab& , DoubleTab& , DoubleTab& ) const;
  void ch_vit_pour_fft_VEF_1(DoubleTab& , DoubleTab& , DoubleTab& ) const;
  void ch_vit_pour_fft_VEF_2(DoubleTab& , DoubleTab& , DoubleTab& ) const;
  void ch_vit_pour_fft_VEF_s(DoubleTab& , DoubleTab& , DoubleTab& ) const;

  IntTab tab_calc_fft;
  IntTab tab_calc_fft_1;
  IntTab tab_calc_fft_2;
  IntTab tab_calc_fft_s;

};

#endif

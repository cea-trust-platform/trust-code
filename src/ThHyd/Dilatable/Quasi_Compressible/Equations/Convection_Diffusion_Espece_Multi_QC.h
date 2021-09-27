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
// File:        Convection_Diffusion_Espece_Multi_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Equations
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Convection_Diffusion_Espece_Multi_QC_included
#define Convection_Diffusion_Espece_Multi_QC_included

#include <Convection_Diffusion_Espece_Multi_base.h>
#include <Espece.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Espece_Multi_QC
//     Cas particulier de Convection_Diffusion_Espece_Multi_base
//     pour un fluide quasi conpressible quand le scalaire subissant le transport est
//     la fraction massique
// .SECTION voir aussi
//     Convection_Diffusion_Espece_Multi_base
//////////////////////////////////////////////////////////////////////////////

class Convection_Diffusion_Espece_Multi_QC : public Convection_Diffusion_Espece_Multi_base
{
  Declare_instanciable(Convection_Diffusion_Espece_Multi_QC);

public :
  void set_param(Param& titi);
  void completer();
  void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  const Champ_base& diffusivite_pour_pas_de_temps();
  DoubleTab& derivee_en_temps_inco(DoubleTab& );

  // Methodes inlines
  inline const Espece& espece() const { return mon_espece_; }
  inline Espece& espece() { return mon_espece_; }

protected :
  Espece mon_espece_;
};

#endif /* Convection_Diffusion_Espece_Multi_QC_included */

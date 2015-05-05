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
// File:        Traitement_particulier_NS_THI_thermo_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_THI_thermo_VDF_included
#define Traitement_particulier_NS_THI_thermo_VDF_included

#include <Traitement_particulier_NS_THI_VDF.h>
#include <Ref_Convection_Diffusion_std.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_NS_THI_thermo_VDF
//     Cette classe permet de faire les traitements particuliers
//     pour les calculs de Turbulence Homogene Isotrope en Thermique
//     avec l'equation d'energie, ou l'on cherche a faire des spectres
//     3D pour le champ de temperature. On ne fait qu'une sortie dans
//     un fichier a chaque pas de temps.
//     Cette classe herite de Trait_part_NS_THI_VDF
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VDF, Traitement_particulier_NS_THI_VDF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_THI_thermo_VDF : public Traitement_particulier_NS_THI_VDF
{
  Declare_instanciable(Traitement_particulier_NS_THI_thermo_VDF);

public :

protected :

  REF(Convection_Diffusion_std) mon_equation_NRJ;

private:
  void init_calc_spectre(void);
  void calcul_spectre(void);
  void sorties_fichiers(void);
  double calcul_enstrophie(void);

};
#endif


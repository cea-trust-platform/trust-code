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
// File:        Turbulence_hyd_sous_maille_axi_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Axi/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Turbulence_hyd_sous_maille_axi_VDF_included
#define Turbulence_hyd_sous_maille_axi_VDF_included



#include <Turbulence_hyd_sous_maille_VDF.h>

// .DESCRIPTION classe Turbulence_hyd_sous_maille_axi_VDF
//  Cette classe correspond a la mise en oeuvre du modele sous
//  maille en axi en VDF
//

// .SECTION  voir aussi
// Turbulence_hyd_sous_maille_VDF

class Turbulence_hyd_sous_maille_axi_VDF : public Turbulence_hyd_sous_maille_VDF
{

  Declare_instanciable(Turbulence_hyd_sous_maille_axi_VDF);

public:

  Champ_Fonc& calculer_viscosite_turbulente();
  void calculer_fonction_structure() ;
  //Surcharge vide de calculer_longueurs_caracteristiques()
  void  calculer_longueurs_caracteristiques();

protected :


};

#endif

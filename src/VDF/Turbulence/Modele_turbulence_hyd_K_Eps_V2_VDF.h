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
// File:        Modele_turbulence_hyd_K_Eps_V2_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_hyd_K_Eps_V2_VDF_included
#define Modele_turbulence_hyd_K_Eps_V2_VDF_included


#include <Modele_turbulence_hyd_K_Eps_V2.h>

#include <Ref_Zone_Cl_VDF.h>
#include <Champ_Face.h>

class Zone_dis;
class Zone_Cl_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Modele_turbulence_hyd_K_Eps_V2_VDF
//    Cette classe represente le modele de turbulence (k,eps,v2) pour les
//    equations de Navier-Stokes.en VDF
// .SECTION voir aussi
//    Mod_turb_hyd_base Mod_turb_hyd_ss_maille
//////////////////////////////////////////////////////////////////////////////
class Modele_turbulence_hyd_K_Eps_V2_VDF : public Modele_turbulence_hyd_K_Eps_V2
{
  Declare_instanciable(Modele_turbulence_hyd_K_Eps_V2_VDF);

public:

  void mettre_a_jour(double );
  void associer(const Zone_dis& , const Zone_Cl_dis& );
  DoubleTab& Calculer_Echelle_T(const DoubleTab&, const DoubleTab&, DoubleTab&, DoubleTab&, double, double, DoubleTab&);
  DoubleTab& Calculer_Echelle_L(const DoubleTab&, const DoubleTab&, DoubleTab&, DoubleTab&, double, double,DoubleTab& );
  DoubleTab& calculer_Sij(const Zone_VDF& ,const Zone_Cl_VDF& , DoubleTab& ,const DoubleTab&,Champ_Face& );

protected :

  virtual Champ_Fonc& calculer_viscosite_turbulente(double temps);
  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
};

#endif

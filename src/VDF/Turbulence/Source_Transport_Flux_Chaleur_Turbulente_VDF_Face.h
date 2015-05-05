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
// File:        Source_Transport_Flux_Chaleur_Turbulente_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Source_Transport_Flux_Chaleur_Turbulente_VDF_Face_included
#define Source_Transport_Flux_Chaleur_Turbulente_VDF_Face_included



//
// .DESCRIPTION class Source_Transport_Flux_Chaleur_Turbulente_VDF_Face
//
// .SECTION voir aussi

#define c1_DEFAULT 5     // Valeurs par defaut des constantes qui interviennent
#define c2_DEFAULT 0.5   // dans le calcul des termes sources des equations
#define c3_DEFAULT 0.33  //  de transport du flux de chaleur turbulent.

#include <Ref_Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Ref_Convection_Diffusion_Temperature.h>
#include <Ref_Equation_base.h>
#include <Ref_Transport_K_Eps_Bas_Reynolds.h>
#include <Ref_Transport_Fluctuation_Temperature.h>
#include <Ref_Transport_Flux_Chaleur_Turbulente.h>
#include <Transport_Fluctuation_Temperature.h>
#include <Transport_Flux_Chaleur_Turbulente.h>
#include <Ref_Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Ref_Champ_Don.h>
#include <Ref_Champ_Don_base.h>

class Probleme_base;
class Champ_Don_base;
class DoubleVect;
class DoubleTab;
class Zone_dis;
class Zone_Cl_dis;
class Zone_Cl_VDF;
class Champ_Face;

// La classe derive de Source_base et peut etre d'un terme source
class Source_Transport_Flux_Chaleur_Turbulente_VDF_Face : public Source_base
{
  Declare_instanciable_sans_constructeur(Source_Transport_Flux_Chaleur_Turbulente_VDF_Face);

public :
  void associer_pb(const Probleme_base& );
  inline Source_Transport_Flux_Chaleur_Turbulente_VDF_Face(double cte1 = c1_DEFAULT, double cte2 = c2_DEFAULT,  double cte3 = c3_DEFAULT);

  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  DoubleTab& calculer_gteta2(const Zone_VDF&,DoubleTab&,const DoubleTab&,double,const DoubleVect&) const;
  DoubleTab& calculer_gteta2(const Zone_VDF&,DoubleTab&,const DoubleTab&, const DoubleTab&,const DoubleVect&) const;

  DoubleTab& calculer_uiuj(const Zone_VDF&, const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& calculer_Grad_U(const Zone_VDF&, const DoubleTab&, const DoubleTab&, DoubleTab&) const;

  void mettre_a_jour(double temps)
  {
    ;
  };


protected :

  double C1, C2, C3;
  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  REF(Equation_base) eq_hydraulique;
  REF(Transport_K_Eps_Bas_Reynolds)  mon_eq_transport_K_Eps_Bas_Re_;
  REF(Transport_Fluctuation_Temperature)  mon_eq_transport_Fluctu_Temp_;
  REF(Convection_Diffusion_Temperature) eq_thermique;
  REF(Transport_Flux_Chaleur_Turbulente)  mon_eq_transport_Flux_Chaleur_Turb_;
  REF(Champ_Don) beta_t;
  REF(Champ_Don_base) gravite_;
  REF(Modele_turbulence_hyd_K_Eps_Bas_Reynolds) le_modele;
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );

};

inline Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::
Source_Transport_Flux_Chaleur_Turbulente_VDF_Face(double cte1, double cte2,  double cte3)
  : C1(cte1), C2(cte2), C3(cte3) {}

#endif

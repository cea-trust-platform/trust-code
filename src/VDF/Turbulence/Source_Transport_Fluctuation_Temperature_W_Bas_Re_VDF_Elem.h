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
// File:        Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem_included
#define Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem_included


//
// .DESCRIPTION class Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem
//
// .SECTION voir aussi

#define Ca_DEF 0.8   // Valeurs par defaut des constantes qui interviennent
#define Cb_DEF 2.2   // dans le calcul des termes sources des equations
#define Cc_DEF 1.8  // de transport des fluctuations thermiques.
#define Cd_DEF 0.72

#include <Source_Transport_K_Eps_VDF_Elem.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Ref_Transport_Fluctuation_Temperature_W_Bas_Re.h>
#include <Transport_K_Eps_Bas_Reynolds.h>
#include <Transport_Fluctuation_Temperature_W_Bas_Re.h>

class Probleme_base;
class Champ_Don_base;
class DoubleVect;
class DoubleTab;
class Zone_dis;
class Zone_Cl_dis;
class Zone_Cl_VDF;
class Champ_Face;

// La classe derive de Source_base et peut etre d'un terme source
class Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem : public Source_base,
  public Calcul_Production_K_VDF
{
  Declare_instanciable_sans_constructeur(Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem);

public :

  void associer_pb(const Probleme_base& );
  inline Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem(double ctea = Ca_DEF, double cteb = Cb_DEF,  double ctec = Cc_DEF,  double cted = Cd_DEF);
  DoubleTab& calculer_Prod_uteta_T(const Zone_VDF&,const Zone_Cl_VDF&, const DoubleTab&,const  DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  DoubleTab& calculer_gteta2(const Zone_VDF&,DoubleTab&,const DoubleTab&,double,const DoubleVect&) const;
  DoubleTab& calculer_gteta2(const Zone_VDF&,DoubleTab&,const DoubleTab&, const DoubleTab&,const DoubleVect&) const;
  DoubleTab& calculer_u_teta_W(const Zone_VDF&,const Zone_Cl_VDF&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,DoubleTab&) const;
  DoubleTab& calculer_terme_destruction_K_W(const Zone_VDF&,const Zone_Cl_VDF&,DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,double,const DoubleVect&) const;
  DoubleTab& calculer_terme_destruction_K_W(const Zone_VDF&,const Zone_Cl_VDF&,DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleVect&) const;
  void mettre_a_jour(double temps)
  {
    Calcul_Production_K_VDF::mettre_a_jour(temps);
  }


protected :

  double Ca, Cb, Cc, Cd;
  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  REF(Equation_base) eq_hydraulique;
  REF(Transport_K_Eps_Bas_Reynolds)  mon_eq_transport_K_Eps_Bas_Re_;
  REF(Transport_Fluctuation_Temperature_W_Bas_Re) mon_eq_transport_Fluctu_Temp;
  REF(Convection_Diffusion_Temperature) eq_thermique;
  REF(Champ_Don) beta_t;
  REF(Champ_Don_base) gravite_;
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );

};

inline Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem::
Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem(double ctea, double cteb,  double ctec,  double cted)
  : Ca(ctea), Cb(cteb), Cc(ctec), Cd(cted) {}

#endif

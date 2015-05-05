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
// File:        Modele_Jones_Launder_Thermique_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_Jones_Launder_Thermique_VDF_included
#define Modele_Jones_Launder_Thermique_VDF_included

#include <Modele_Fonc_Bas_Reynolds_Thermique_Base.h>
#include <Transport_Fluctuation_Temperature_W_Bas_Re.h>
#include <Ref_Transport_K_Eps_Bas_Reynolds.h>
#include <Ref_Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>

class Transport_Fluctuation_Temperature_W_Bas_Re;
class Zone_dis;
class Zone_Cl_dis;
class Champ_Don_base;
class DoubleVect;
class DoubleTab;
class Zone_Cl_VDF;
class Champ_Face;

class Modele_Jones_Launder_Thermique_VDF : public Modele_Fonc_Bas_Reynolds_Thermique_Base
{

  Declare_instanciable(Modele_Jones_Launder_Thermique_VDF);

public :

  DoubleTab& Calcul_D(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&, double) const;
  DoubleTab& Calcul_E(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&,double,const DoubleTab& ) const;
  DoubleTab& Calcul_F1(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  DoubleTab& Calcul_F2(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  DoubleTab& Calcul_F3(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  DoubleTab& Calcul_F4(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  DoubleTab& Calcul_Flambda ( DoubleTab&,const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  Entree& lire(const Motcle&, Entree&);
  void associer_pb(const Probleme_base& );
  void associer(const Zone_dis& , const Zone_Cl_dis& );
  void mettre_a_jour(double);

protected:

  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  REF(Fluide_Incompressible) le_fluide;
  REF(Champ_Inc) la_vitesse_transportante;
  REF(Transport_Fluctuation_Temperature_W_Bas_Re) eq_transport_Fluctu_Temp_Bas_Re;

};

#endif




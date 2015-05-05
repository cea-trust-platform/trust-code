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
// File:        Modele_Fonc_Bas_Reynolds_Thermique_Base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_Fonc_Bas_Reynolds_Thermique_Base_included
#define Modele_Fonc_Bas_Reynolds_Thermique_Base_included


#include <Ref_Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re.h>
#include <Ref_Transport_Fluctuation_Temperature_W_Bas_Re.h>
#include <Ref_Champ_base.h>
#include <Champ_Fonc.h>
#include <Champs_compris.h>
#include <Champs_compris_interface.h>
/*
  #include <Ref_Zone_VDF.h>
  #include <Ref_Zone_Cl_VDF.h>
  #include <Zone_VDF.h>
  #include <Zone_Cl_VDF.h>
*/
class Motcle;
class Zone_dis;
class Zone_Cl_dis;

class Equation_base;
class Probleme_base;
class Modele_Fonc_Bas_Reynolds_Thermique_Base : public Champs_compris_interface, public Objet_U
{

  Declare_base(Modele_Fonc_Bas_Reynolds_Thermique_Base);

public:

  inline const Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& modele_turbulence_Bas_Re() const;
  inline  Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& modele_turbulence_Bas_Re();
  inline const Transport_Fluctuation_Temperature_W_Bas_Re& equation() const;
  inline  Transport_Fluctuation_Temperature_W_Bas_Re& equation();
  // inline const Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem& sources_Fluctu_Temp_Bas_Re() const;
  virtual int preparer_calcul();
  virtual void mettre_a_jour(double ) =0;
  virtual void discretiser();
  virtual void completer();
  virtual void associer_pb(const Probleme_base& ) = 0;
  virtual void associer_eqn(const Equation_base& );
  virtual void associer(const Zone_dis& , const Zone_Cl_dis& )= 0;
  virtual int sauvegarder(Sortie& ) const;
  virtual int reprendre(Entree& );

  virtual DoubleTab& Calcul_D(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&, double) const = 0;
  virtual DoubleTab& Calcul_E(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&,double,const DoubleTab& ) const = 0;
  virtual DoubleTab& Calcul_F1(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const = 0;
  virtual DoubleTab& Calcul_F2(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const = 0;
  virtual DoubleTab& Calcul_F3(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const = 0;
  virtual DoubleTab& Calcul_F4(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const = 0;
  virtual DoubleTab& Calcul_Flambda ( DoubleTab&,const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const = 0;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

protected :

  REF(Transport_Fluctuation_Temperature_W_Bas_Re) mon_equation;
  REF(Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re) mon_modele_turb_Bas_Re;
  //REF(Source_Transport_Fluctuation_Temperature_W_Bas_Re_VDF_Elem) mes_sources;

private :

  Champs_compris champs_compris_;

};

//
// fonction inline
//

inline const Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& Modele_Fonc_Bas_Reynolds_Thermique_Base::modele_turbulence_Bas_Re() const
{
  assert(mon_modele_turb_Bas_Re.non_nul());
  return mon_modele_turb_Bas_Re.valeur();
}

inline  Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& Modele_Fonc_Bas_Reynolds_Thermique_Base::modele_turbulence_Bas_Re()
{
  assert(mon_modele_turb_Bas_Re.non_nul());
  return mon_modele_turb_Bas_Re.valeur();
}


inline const Transport_Fluctuation_Temperature_W_Bas_Re& Modele_Fonc_Bas_Reynolds_Thermique_Base::equation() const
{
  return mon_equation.valeur();
}

inline Transport_Fluctuation_Temperature_W_Bas_Re& Modele_Fonc_Bas_Reynolds_Thermique_Base::equation()
{
  return mon_equation.valeur();
}


#endif




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
// File:        Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem_included
#define Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem_included


#define C11_DEFAULT 1.55   // Valeurs par defaut des constantes qui interviennent
#define C21_DEFAULT 2.   // dans l'equation de k_esp

//
// .DESCRIPTION class Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem
//
// .SECTION voir aussi

#include <Source_Transport_K_Eps_VDF_Elem.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Modele_Fonc_Bas_Reynolds.h>
#include <Ref_Zone_dis.h>
#include <Zone_Cl_dis.h>

class Probleme_base;
class Champ_Don_base;
class DoubleVect;
class DoubleTab;
class Champ_Face;

class Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem : public Source_Transport_K_Eps_VDF_Elem


{
  Declare_instanciable_sans_constructeur(Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem);

public :

  inline Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem(double cte1 = C11_DEFAULT,
                                                        double cte2 = C21_DEFAULT );
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  inline Modele_Fonc_Bas_Reynolds&  associe_modele_fonc();
  inline const Modele_Fonc_Bas_Reynolds&  associe_modele_fonc() const;
  virtual void associer_pb(const Probleme_base& );

protected :

  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );
  double C1;
  double C2;

  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  REF(Transport_K_Eps_Bas_Reynolds) eqn_keps_bas_re;

};



//.DESCRIPTION class Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W_VDF_Elem
//
//Cette classe represente le terme source qui figure dans l'equation
// de transport du couple (k,eps) dans le cas ou les equations de Navier_Stokes
// sont couplees a l'equation de la thermique
// On suppose que le coefficient de variation de la masse volumique
// du fluide en fonction de ce scalaire est un coefficient uniforme.
//
//////////////////////////////////////////////////////////////////////////////

class Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W_VDF_Elem :
  public Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem
{

  Declare_instanciable_sans_constructeur(Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W_VDF_Elem);

public:

  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  virtual void associer_pb(const Probleme_base& );
  inline Modele_Fonc_Bas_Reynolds&  associe_modele_fonc();
  inline const Modele_Fonc_Bas_Reynolds&  associe_modele_fonc() const;
  inline Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W_VDF_Elem(double cte1 = C11_DEFAULT,
                                                                    double cte2 = C21_DEFAULT );
  DoubleTab& calculer_gteta2(const Zone_VDF&,DoubleTab&,const DoubleTab&,double,const DoubleVect&) const;
  DoubleTab& calculer_gteta2(const Zone_VDF&,DoubleTab&,const DoubleTab&, const DoubleTab&,const DoubleVect&) const;
  DoubleTab& calculer_u_teta_W(const Zone_VDF&,const Zone_Cl_VDF&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,DoubleTab&) const;
  DoubleTab& calculer_terme_destruction_K_W(const Zone_VDF&,const Zone_Cl_VDF&,DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,double,const DoubleVect&) const;
  DoubleTab& calculer_terme_destruction_K_W(const Zone_VDF&,const Zone_Cl_VDF&,DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleVect&) const;
protected:

  REF(Convection_Diffusion_Temperature) eq_thermique;
  REF(Champ_Don) beta_t;
  REF(Champ_Don_base) gravite_;
};


//////////////////////////////////////////////////////////////////////////////
//
//   Fonctions inline de la classe Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem
//
//////////////////////////////////////////////////////////////////////////////

inline Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem::
Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem(double cte1,double cte2)

  : C1(cte1), C2(cte2) {}


inline Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W_VDF_Elem::
Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W_VDF_Elem(double cte1 ,double cte2 )

  : Source_Transport_K_Eps_Bas_Reynolds_W_VDF_Elem(cte1,cte2) {}


#endif

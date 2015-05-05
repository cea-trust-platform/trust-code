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
// File:        Modele_Fonc_Bas_Reynolds_Thermique.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_Fonc_Bas_Reynolds_Thermique_included
#define Modele_Fonc_Bas_Reynolds_Thermique_included

#include <MorEqn.h>
#include <Modele_Fonc_Bas_Reynolds_Thermique_Base.h>

Declare_deriv(Modele_Fonc_Bas_Reynolds_Thermique_Base);



class Modele_Fonc_Bas_Reynolds_Thermique : public MorEqn, public DERIV(Modele_Fonc_Bas_Reynolds_Thermique_Base)
{
  Declare_instanciable(Modele_Fonc_Bas_Reynolds_Thermique);

public:

  inline int preparer_calcul();
  inline void mettre_a_jour(double );
  inline void discretiser();
  inline void completer();
  inline int sauvegarder(Sortie& os) const;
  inline int reprendre(Entree& is);
  inline DoubleTab& Calcul_D(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&, double) const;
  inline DoubleTab& Calcul_E(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&,double,const DoubleTab& ) const;
  inline DoubleTab& Calcul_F1(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  inline DoubleTab& Calcul_F2(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  inline DoubleTab& Calcul_F3(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  inline DoubleTab& Calcul_F4(DoubleTab&, const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;
  inline DoubleTab& Calcul_Flambda ( DoubleTab&,const Zone_dis&,const DoubleTab&,const DoubleTab&,double,double) const ;

private :

};

///
//    implementation des fonctions inline
///

inline int Modele_Fonc_Bas_Reynolds_Thermique::preparer_calcul()
{
  return valeur().preparer_calcul();
}

inline void Modele_Fonc_Bas_Reynolds_Thermique::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}

inline int Modele_Fonc_Bas_Reynolds_Thermique::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

inline int Modele_Fonc_Bas_Reynolds_Thermique::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

inline void Modele_Fonc_Bas_Reynolds_Thermique::discretiser()
{
  valeur().discretiser();
}

inline void Modele_Fonc_Bas_Reynolds_Thermique::completer()
{
  valeur().completer();
}

inline DoubleTab& Modele_Fonc_Bas_Reynolds_Thermique::Calcul_D(DoubleTab& D,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                                               const DoubleTab& vitesse,const DoubleTab& FluctuTemp_Bas_Re,double diffu ) const
{
  return  valeur().Calcul_D(D,zone_dis, zone_Cl_dis, vitesse,FluctuTemp_Bas_Re,diffu);
}

inline  DoubleTab& Modele_Fonc_Bas_Reynolds_Thermique::Calcul_E(DoubleTab& E,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& vitesse,const DoubleTab& FluctuTemp_Bas_Re,double diffu, const DoubleTab& diffu_turb ) const
{
  return valeur().Calcul_E(E, zone_dis,zone_Cl_dis,  vitesse,FluctuTemp_Bas_Re,diffu,diffu_turb );
}

inline DoubleTab&  Modele_Fonc_Bas_Reynolds_Thermique::Calcul_F1(DoubleTab& F1, const Zone_dis& zone_dis,const DoubleTab& KEps_Bas_Re,const DoubleTab& FluctuTemp_Bas_Re,double visco,double diffu) const
{
  return valeur().Calcul_F1( F1, zone_dis, KEps_Bas_Re, FluctuTemp_Bas_Re, visco, diffu);
}

inline DoubleTab&  Modele_Fonc_Bas_Reynolds_Thermique::Calcul_F2(DoubleTab& F1, const Zone_dis& zone_dis,const DoubleTab& KEps_Bas_Re,const DoubleTab& FluctuTemp_Bas_Re,double visco,double diffu) const
{
  return valeur().Calcul_F2( F1, zone_dis, KEps_Bas_Re, FluctuTemp_Bas_Re, visco, diffu);
}

inline DoubleTab&  Modele_Fonc_Bas_Reynolds_Thermique::Calcul_F3(DoubleTab& F1, const Zone_dis& zone_dis,const DoubleTab& KEps_Bas_Re,const DoubleTab& FluctuTemp_Bas_Re,double visco,double diffu) const
{
  return valeur().Calcul_F3( F1, zone_dis, KEps_Bas_Re, FluctuTemp_Bas_Re, visco, diffu);
}

inline DoubleTab&  Modele_Fonc_Bas_Reynolds_Thermique::Calcul_F4(DoubleTab& F1, const Zone_dis& zone_dis,const DoubleTab& KEps_Bas_Re,const DoubleTab& FluctuTemp_Bas_Re,double visco,double diffu) const
{
  return valeur().Calcul_F4( F1, zone_dis, KEps_Bas_Re, FluctuTemp_Bas_Re, visco, diffu);
}

inline DoubleTab&  Modele_Fonc_Bas_Reynolds_Thermique::Calcul_Flambda(DoubleTab& Flambda,const Zone_dis& zone_dis,const DoubleTab& KEps_Bas_Re,const DoubleTab& FluctuTemp_Bas_Re,double visco, double diffu ) const
{
  return valeur().Calcul_Flambda( Flambda,zone_dis, KEps_Bas_Re, FluctuTemp_Bas_Re, visco, diffu );
}

#endif

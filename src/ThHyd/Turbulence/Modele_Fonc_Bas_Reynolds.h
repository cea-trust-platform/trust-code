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
// File:        Modele_Fonc_Bas_Reynolds.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_Fonc_Bas_Reynolds_included
#define Modele_Fonc_Bas_Reynolds_included

#include <MorEqn.h>
#include <Modele_Fonc_Bas_Reynolds_Base.h>

Declare_deriv(Modele_Fonc_Bas_Reynolds_Base);



class Modele_Fonc_Bas_Reynolds : public MorEqn, public DERIV(Modele_Fonc_Bas_Reynolds_Base)
{
  Declare_instanciable(Modele_Fonc_Bas_Reynolds);

public:

  inline int preparer_calcul();
  inline void mettre_a_jour(double );
  inline void discretiser();
  inline void completer();
  inline int sauvegarder(Sortie& os) const;
  inline int reprendre(Entree& is);
  inline DoubleTab& Calcul_D(DoubleTab&, const Zone_dis&, const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&, const Champ_Don&) const;
  inline DoubleTab& Calcul_E(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const DoubleTab&,const Champ_Don&,const DoubleTab& ) const;
//  inline DoubleTab& Calcul_F1(DoubleTab&, const Zone_dis& ) const;
  inline DoubleTab& Calcul_F1( DoubleTab&, const Zone_dis&, const Zone_Cl_dis&, const DoubleTab&,const DoubleTab&,const Champ_base& ) const;
  inline DoubleTab& Calcul_F2(DoubleTab&, DoubleTab&,const Zone_dis&,const DoubleTab&,const Champ_base& ) const;
  inline DoubleTab& Calcul_Fmu(DoubleTab&,const Zone_dis&,const Zone_Cl_dis&,const DoubleTab&,const Champ_Don& ) const;
  inline int Calcul_is_Cmu_constant() const;
  inline int Calcul_is_Reynolds_stress_isotrope() const;
  inline DoubleTab& Calcul_Cmu(DoubleTab&,const Zone_dis&, const Zone_Cl_dis&, const DoubleTab&, const DoubleTab&, const double) const;
  inline DoubleTab& Calcul_Cmu_Paroi(DoubleTab&, const Zone_dis&, const Zone_Cl_dis&,
                                     const DoubleTab& , const DoubleTab& ,
                                     const DoubleTab& ,const int,
                                     const DoubleTab&, const DoubleTab&,
                                     const double) const;
  inline DoubleTab calcul_tenseur_Re(const Discretisation_base& dis, const Zone_dis&, const Zone_Cl_dis&, const DoubleTab&,  const DoubleTab&, const Champ_base& K_Eps) const;
  inline void lire_distance_paroi();
private :

};

///
//    implementation des fonctions inline
///

inline int Modele_Fonc_Bas_Reynolds::preparer_calcul()
{
  return valeur().preparer_calcul();
}

inline void Modele_Fonc_Bas_Reynolds::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}

inline int Modele_Fonc_Bas_Reynolds::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

inline int Modele_Fonc_Bas_Reynolds::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

inline void Modele_Fonc_Bas_Reynolds::discretiser()
{
  valeur().discretiser();
}

inline void Modele_Fonc_Bas_Reynolds::completer()
{
  valeur().completer();
}

inline DoubleTab& Modele_Fonc_Bas_Reynolds::Calcul_D(DoubleTab& D, const Zone_dis& zone_dis, const Zone_Cl_dis& zcl_VDF,
                                                     const DoubleTab& vitesse,const DoubleTab& K_eps_Bas_Re,const Champ_Don& visco ) const
{
  return  valeur().Calcul_D(D, zone_dis, zcl_VDF, vitesse,K_eps_Bas_Re,visco);
}

inline  DoubleTab& Modele_Fonc_Bas_Reynolds::Calcul_E(DoubleTab& E,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& vitesse,const DoubleTab& K_eps_Bas_Re,const Champ_Don& visco, const DoubleTab& visco_turb ) const
{
  return valeur().Calcul_E(E, zone_dis,zone_Cl_dis,  vitesse,K_eps_Bas_Re,visco,visco_turb );
}

/*inline DoubleTab&  Modele_Fonc_Bas_Reynolds::Calcul_F1(DoubleTab& F1, const Zone_dis& zone_dis) const
{
  return valeur().Calcul_F1( F1, zone_dis);
}
*/
inline DoubleTab&  Modele_Fonc_Bas_Reynolds::Calcul_F1(DoubleTab& F1, const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& P, const DoubleTab& K_eps_Bas_Re,const Champ_base& ch_visco) const
{
  return valeur().Calcul_F1( F1, zone_dis, zone_Cl_dis, P, K_eps_Bas_Re,ch_visco);
}

inline DoubleTab&  Modele_Fonc_Bas_Reynolds::Calcul_F2(DoubleTab& F2, DoubleTab& D,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,const Champ_base& visco ) const
{
  return valeur().Calcul_F2(F2, D, zone_dis,K_eps_Bas_Re,visco );
}
inline DoubleTab&  Modele_Fonc_Bas_Reynolds::Calcul_Fmu(DoubleTab& Fmu,const Zone_dis& zone_dis,const Zone_Cl_dis& zone_Cl_dis,const DoubleTab& K_eps_Bas_Re,const Champ_Don& visco ) const
{
  return valeur().Calcul_Fmu( Fmu, zone_dis, zone_Cl_dis, K_eps_Bas_Re, visco );
}

inline int Modele_Fonc_Bas_Reynolds::Calcul_is_Cmu_constant() const
{
  return valeur().Calcul_is_Cmu_constant();
}

inline int Modele_Fonc_Bas_Reynolds::Calcul_is_Reynolds_stress_isotrope() const
{
  return valeur().Calcul_is_Reynolds_stress_isotrope();
}

inline DoubleTab& Modele_Fonc_Bas_Reynolds::Calcul_Cmu(DoubleTab& Cmu,
                                                       const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                                       const DoubleTab& vitesse, const DoubleTab& K_Eps, const double EPS_MIN) const
{
  return valeur().Calcul_Cmu(Cmu, zone_dis, zone_Cl_dis, vitesse, K_Eps, EPS_MIN);
}
inline DoubleTab& Modele_Fonc_Bas_Reynolds::Calcul_Cmu_Paroi(DoubleTab& Cmu,
                                                             const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                                             const DoubleTab& visco, const DoubleTab& visco_turb,
                                                             const DoubleTab& loi_paroi,const int idt,
                                                             const DoubleTab& vitesse, const DoubleTab& K_Eps, const double EPS_MIN) const
{
  return valeur().Calcul_Cmu_Paroi(Cmu,zone_dis,zone_Cl_dis, visco,visco_turb,loi_paroi,idt,vitesse,K_Eps,EPS_MIN);
}
inline DoubleTab  Modele_Fonc_Bas_Reynolds::calcul_tenseur_Re(const Discretisation_base& dis,
                                                              const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                                              const DoubleTab& grad, const DoubleTab& visco_elem,
                                                              const Champ_base& K_Eps) const
{
  return valeur().calcul_tenseur_Re(dis,zone_dis,zone_Cl_dis,grad,visco_elem,K_Eps);
}

inline void Modele_Fonc_Bas_Reynolds::lire_distance_paroi()
{
  return valeur().lire_distance_paroi();
}

#endif

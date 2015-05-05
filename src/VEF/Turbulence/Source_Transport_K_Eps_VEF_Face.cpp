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
// File:        Source_Transport_K_Eps_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Transport_K_Eps_VEF_Face.h>
#include <Mod_turb_hyd_base.h>
#include <Transport_K_Eps.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Zone_VEF.h>
#include <Champ_P1NC.h>
#include <DoubleTrav.h>
#include <Fluide_Quasi_Compressible.h>
#include <Pb_Hydraulique_Turbulent.h>
#include <Pb_Hydraulique_Concentration_Turbulent.h>
#include <Pb_Thermohydraulique_Turbulent_QC.h>
#include <Pb_Thermohydraulique_Turbulent.h>
#include <Pb_Thermohydraulique_Concentration_Turbulent.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_VEF_Face,"Source_Transport_K_Eps_VEF_P1NC",Source_base);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_anisotherme_VEF_Face,"Source_Transport_K_Eps_anisotherme_VEF_P1NC",Source_Transport_K_Eps_VEF_Face);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_aniso_concen_VEF_Face,"Source_Transport_K_Eps_aniso_concen_VEF_P1NC",Source_Transport_K_Eps_VEF_Face);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_aniso_therm_concen_VEF_Face,"Source_Transport_K_Eps_aniso_therm_concen_VEF_P1NC",Source_Transport_K_Eps_VEF_Face);


Sortie& Source_Transport_K_Eps_VEF_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_anisotherme_VEF_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_aniso_concen_VEF_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_aniso_therm_concen_VEF_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

inline void error(const Nom& source, const Nom& problem)
{
  Cerr << "Error ! You can't use the " << source << " source term for the K-Eps equation of the problem: " << problem << finl;
  Cerr << "Check the user's manual. It is may be another source term Source_Transport_K_Eps_.... which should be used." << finl;
  Process::exit();
}

///
/// readOn
///
Entree& Source_Transport_K_Eps_VEF_Face::readOn(Entree& is)
{
  const Probleme_base& problem = mon_equation->probleme();
  if (!sub_type(Pb_Hydraulique_Turbulent,problem) && !sub_type(Pb_Thermohydraulique_Turbulent_QC,problem)) error(que_suis_je(),problem.que_suis_je());
  Param param(que_suis_je());
  param.ajouter("C1_eps", &C1);
  param.ajouter("C2_eps", &C2);
  param.lire_avec_accolades(is);
  Cerr << "C1_eps = " << C1 << finl;
  Cerr << "C2_eps = " << C2 << finl;
  return is;
}

Entree& Source_Transport_K_Eps_anisotherme_VEF_Face::readOn(Entree& is)
{
  const Probleme_base& problem = mon_equation->probleme();
  if (!sub_type(Pb_Thermohydraulique_Turbulent,problem)) error(que_suis_je(),problem.que_suis_je());
  Param param(que_suis_je());
  param.ajouter("C1_eps", &C1);
  param.ajouter("C2_eps", &C2);
  param.ajouter("C3_eps", &C3);
  param.lire_avec_accolades(is);
  Cerr << "C1_eps = " << C1 << finl;
  Cerr << "C2_eps = " << C2 << finl;
  Cerr << "C3_eps = " << C3 << finl;
  return is;
}

Entree& Source_Transport_K_Eps_aniso_concen_VEF_Face::readOn(Entree& is)
{
  const Probleme_base& problem = mon_equation->probleme();
  if (!sub_type(Pb_Hydraulique_Concentration_Turbulent,problem)) error(que_suis_je(),problem.que_suis_je());
  Param param(que_suis_je());
  param.ajouter("C1_eps", &C1);
  param.ajouter("C2_eps", &C2);
  param.ajouter("C3_eps", &C3);
  param.lire_avec_accolades(is);
  Cerr << "C1_eps = " << C1 << finl;
  Cerr << "C2_eps = " << C2 << finl;
  Cerr << "C3_eps = " << C3 << finl;
  return is;
}

Entree& Source_Transport_K_Eps_aniso_therm_concen_VEF_Face::readOn(Entree& is)
{
  const Probleme_base& problem = mon_equation->probleme();
  if (!sub_type(Pb_Thermohydraulique_Concentration_Turbulent,problem)) error(que_suis_je(),problem.que_suis_je());
  Param param(que_suis_je());
  param.ajouter("C1_eps", &C1);
  param.ajouter("C2_eps", &C2);
  param.ajouter("C3_eps", &C3);
  param.lire_avec_accolades(is);
  Cerr << "C1_eps = " << C1 << finl;
  Cerr << "C2_eps = " << C2 << finl;
  Cerr << "C3_eps = " << C3 << finl;
  return is;
}

void Source_Transport_K_Eps_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                     const Zone_Cl_dis& )
{
  la_zone_VEF = ref_cast(Zone_VEF, zone_dis.valeur());
}


///
/// associer_pb
///
void Source_Transport_K_Eps_VEF_Face::associer_pb(const Probleme_base& pb)
{
  eq_hydraulique = pb.equation(0);
  mon_eq_transport_K_Eps = ref_cast(Transport_K_Eps,equation());
}

void Source_Transport_K_Eps_anisotherme_VEF_Face::associer_pb(const Probleme_base& pb)
{
  if (pb.nombre_d_equations()<2)
    {
      Cerr<<"The K_Eps source term "<<que_suis_je()<<" cannot be activated"<<finl;
      Cerr<<"for a "<<pb.que_suis_je()<<" problem."<<finl;
    }
  const Equation_base& eqn = pb.equation(1);
  const Milieu_base& milieu = eqn.milieu();
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,milieu);

  if (sub_type(Fluide_Quasi_Compressible,fluide))
    {
      Cerr<<"The K_Eps source term "<<que_suis_je()<<" cannot be activated"<<finl;
      Cerr<<"with a "<<milieu.que_suis_je()<<" medium."<<finl;
      exit();
    }
  Source_Transport_K_Eps_VEF_Face::associer_pb(pb);
  const Convection_Diffusion_Temperature& eqn_th = ref_cast(Convection_Diffusion_Temperature,eqn);
  eq_thermique = eqn_th;
  beta_t=fluide.beta_t();
  gravite = fluide.gravite();
}

void Source_Transport_K_Eps_aniso_concen_VEF_Face::associer_pb(const Probleme_base& pb)
{
  if (pb.nombre_d_equations()<2)
    {
      Cerr<<"The K_Eps source term "<<que_suis_je()<<" cannot be activated"<<finl;
      Cerr<<"for a "<<pb.que_suis_je()<<" problem."<<finl;
    }

  const Equation_base& eqn = pb.equation(1);
  const Milieu_base& milieu = pb.equation(0).milieu();
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,milieu);

  if (sub_type(Fluide_Quasi_Compressible,fluide))
    {
      Cerr<<"The K_Eps source term "<<que_suis_je()<<" cannot be activated"<<finl;
      Cerr<<"with a "<<milieu.que_suis_je()<<" medium."<<finl;
      exit();
    }

  Source_Transport_K_Eps_VEF_Face::associer_pb(pb);

  const Convection_Diffusion_Concentration& eqn_c = ref_cast(Convection_Diffusion_Concentration,eqn);
  eq_concentration = eqn_c;
  if (!fluide.beta_c().non_nul())
    {
      Cerr << "You forgot to define beta_co field in the fluid." << finl;
      Cerr << "It is mandatory when using the K-Eps model (buoyancy effects)." << finl;
      Cerr << "If you don't want buoyancy effects, then specify: beta_co champ_uniforme 1 0." << finl;
      exit();
    }
  beta_c = fluide.beta_c();
  gravite = fluide.gravite();
}

void Source_Transport_K_Eps_aniso_therm_concen_VEF_Face::associer_pb(const Probleme_base& pb)
{
  if (pb.nombre_d_equations()<3)
    {
      Cerr<<"The K_Eps source term "<<que_suis_je()<<" cannot be activated"<<finl;
      Cerr<<"for a "<<pb.que_suis_je()<<" problem."<<finl;
    }
  const Equation_base& eqn_therm = pb.equation(1);
  const Equation_base& eqn_conc = pb.equation(2);
  const Milieu_base& milieu = eqn_therm.milieu();
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,milieu);

  if (sub_type(Fluide_Quasi_Compressible,fluide))
    {
      Cerr<<"The K_Eps source term "<<que_suis_je()<<" cannot be activated"<<finl;
      Cerr<<"with a "<<milieu.que_suis_je()<<" medium."<<finl;
      exit();
    }
  Source_Transport_K_Eps_VEF_Face::associer_pb(pb);

  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,eqn_therm);
  eq_thermique = eqn_th;
  const Convection_Diffusion_Concentration& eqn_c =
    ref_cast(Convection_Diffusion_Concentration,eqn_conc);
  eq_concentration = eqn_c;
  beta_t = fluide.beta_t();
  if (!fluide.beta_c().non_nul())
    {
      Cerr << "You forgot to define beta_co field in the fluid." << finl;
      Cerr << "It is mandatory when using the K-Eps model (buoyancy effects)." << finl;
      Cerr << "If you don't want buoyancy effects, then specify: beta_co champ_uniforme 1 0." << finl;
      exit();
    }
  beta_c = fluide.beta_c();
  gravite = fluide.gravite();
}

DoubleTab& Source_Transport_K_Eps_VEF_Face::calculer(DoubleTab& resu) const
{
  resu =0.;
  return ajouter(resu);
}

DoubleTab& Source_Transport_K_Eps_anisotherme_VEF_Face::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}

DoubleTab& Source_Transport_K_Eps_aniso_concen_VEF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

DoubleTab& Source_Transport_K_Eps_aniso_therm_concen_VEF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

DoubleTab& Source_Transport_K_Eps_VEF_Face::ajouter(DoubleTab& resu) const
{

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,eq_hydraulique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& visco_turb = mon_eq_transport_K_Eps->modele_turbulence().viscosite_turbulente().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  int nb_faces_ = zone_VEF.nb_faces();
  DoubleTrav P(nb_faces_);

  calculer_terme_production_K(zone_VEF,zone_Cl_VEF,P,K_eps,vit,visco_turb);
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();

  for (int fac=0; fac<nb_faces_; fac++)
    {
      resu(fac,0) += (P(fac)-K_eps(fac,1))*volumes_entrelaces(fac);

      if (K_eps(fac,0) >= LeK_MIN)
        resu(fac,1) += (C1*P(fac)-C2*K_eps(fac,1))*volumes_entrelaces(fac)*K_eps(fac,1)/K_eps(fac,0);
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_anisotherme_VEF_Face::ajouter(DoubleTab& resu) const
{
  Source_Transport_K_Eps_VEF_Face::ajouter(resu);
  //
  // Modifs VB : plutot que de calculer P, on appelle Source_Transport_K_Eps_VEF_Face::ajouter(resu)
  // et on ajoute directement G
  //
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zcl_VEF_th = ref_cast(Zone_Cl_VEF,eq_thermique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& scalaire = eq_thermique->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire = ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb = le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleTab& g = gravite->valeurs();
  const Champ_Don& ch_beta = beta_t.valeur();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();

  int nb_face = zone_VEF.nb_faces();
  DoubleTrav G(nb_face);

  // C'est l'objet de type zone_Cl_dis de l'equation thermique
  // qui est utilise dans le calcul de G
  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_th,G,scalaire,alpha_turb,ch_beta,g);

  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();

  for (int face=0; face<nb_face; face++)
    {
      resu(face,0) += G(face)*volumes_entrelaces(face);
      if (K_eps(face,0) >= LeK_MIN)
        {
          double C3_loc = C3 ;
          if ( G(face) > 0.) C3_loc = 0 ;
          resu(face,1) += C1*(1-C3_loc)*G(face)*volumes_entrelaces(face)
                          *K_eps(face,1)/K_eps(face,0);
        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_aniso_concen_VEF_Face::ajouter(DoubleTab& resu) const
{
  Source_Transport_K_Eps_VEF_Face::ajouter(resu);
  //
  // Modifs VB : plutot que de calculer P, on appelle Source_Transport_K_Eps_VEF_Face::ajouter(resu)
  // et on ajoute directement G
  // On en profite pour faire des tests sur LeK_MIN
  //
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zcl_VEF_co = ref_cast(Zone_Cl_VEF,eq_concentration->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& concen = eq_concentration->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_concentration->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb = le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleVect& g = gravite->valeurs();
  const Champ_Don& ch_beta_concen = beta_c.valeur();

  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  int nb_face = zone_VEF.nb_faces();

  DoubleTrav G(nb_face);

  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_co,G,concen,alpha_turb,ch_beta_concen,g);

  double C3_loc;
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN() ;
  for (int face=0; face<nb_face; face++)
    {

      resu(face,0) += G(face)*volumes_entrelaces(face);

      if (K_eps(face,0) >= LeK_MIN)
        {
          C3_loc = C3 ;
          if ( G(face) > 0.) C3_loc = 0 ;
          resu(face,1) += C1*(1-C3_loc)*G(face)*volumes_entrelaces(face)
                          *K_eps(face,1)/K_eps(face,0);

        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_aniso_therm_concen_VEF_Face::ajouter(DoubleTab& resu) const
{
  Source_Transport_K_Eps_VEF_Face::ajouter(resu);
  //
  // Modifs VB : plutot que de calculer P, on appelle Source_Transport_K_Eps_VEF_Face::ajouter(resu)
  // et on ajoute directement G
  // On en profite pour faire des tests sur LeK_MIN
  //
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zcl_VEF_th = ref_cast(Zone_Cl_VEF,eq_thermique->zone_Cl_dis().valeur());
  const Zone_Cl_VEF& zcl_VEF_co = ref_cast(Zone_Cl_VEF,eq_concentration->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& temper = eq_thermique->inconnue().valeurs();
  const DoubleTab& concen = eq_concentration->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb = le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleVect& g = gravite->valeurs();
  const Champ_Don& ch_beta_temper = beta_t.valeur();
  const Champ_Don& ch_beta_concen = beta_c.valeur();

  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  int nb_face = zone_VEF.nb_faces();

  DoubleTrav P(nb_face);
  DoubleTrav G_t(nb_face);
  DoubleTrav G_c(nb_face);

  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_th,G_t,temper,alpha_turb,ch_beta_temper,g);
  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_co,G_c,concen,alpha_turb,ch_beta_concen,g);

  double C3_loc, G_sum ;
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();
  for (int face=0; face<nb_face; face++)
    {
      G_sum = G_t(face)+G_c(face) ;

      resu(face,0) += G_sum *volumes_entrelaces(face);

      if (K_eps(face,0) >= LeK_MIN)
        {
          C3_loc = C3 ;
          if ( G_sum > 0.) C3_loc = 0 ;
          resu(face,1) +=  C1*(1-C3_loc)*G_sum *volumes_entrelaces(face)
                           *K_eps(face,1)/K_eps(face,0);
        }
    }
  return resu;
}

void Source_Transport_K_Eps_VEF_Face::contribuer_a_avec(const DoubleTab& a, Matrice_Morse& matrice) const
{
  const DoubleTab& K_eps=equation().inconnue().valeurs();
  int size=K_eps.dimension(0);
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  // on implicite le -eps et le -eps^2/k
  const DoubleVect& volumes_entrelaces=zone_VEF.volumes_entrelaces();
  for (int face=0; face<size; face++)
    {
      // -eps*vol  donne +vol dans la bonne case
      if (K_eps(face,0) >= LeK_MIN)
        {
          double coef_k=porosite_face(face)*volumes_entrelaces(face)*K_eps(face,1)/K_eps(face,0);
          matrice(face*2,face*2)+=coef_k;
          double coef_eps=C2*K_eps(face,1)/K_eps(face,0)*volumes_entrelaces(face)*porosite_face(face);
          matrice(face*2+1,face*2+1)+=coef_eps;
        }
    }
}




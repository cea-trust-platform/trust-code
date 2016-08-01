/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
#include <Paroi_negligeable_VEF.h>
#include <Mod_turb_hyd_base.h>
#include <Transport_K_Eps.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
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
#include <Constituant.h>

#include <Modele_turbulence_hyd_K_Eps.h>

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
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();
  /*Paroi*/
  const Modele_turbulence_hyd_K_Eps& mod  =
    ref_cast(Modele_turbulence_hyd_K_Eps,mon_eq_transport_K_Eps->modele_turbulence());
  const DoubleTab& tab = mod.loi_paroi().valeur().Cisaillement_paroi();
  /**/
  int nb_faces_ = zone_VEF.nb_faces();
  DoubleTrav P(nb_faces_);

  calculer_terme_production_K(zone_VEF,zone_Cl_VEF,P,K_eps,vit,visco_turb);


  const Modele_Fonc_Bas_Reynolds& mon_modele_fonc=ref_cast(Modele_turbulence_hyd_K_Eps,mon_eq_transport_K_Eps->modele_turbulence()).associe_modele_fonction();
  int is_modele_fonc=(mon_modele_fonc.non_nul());
  //is_modele_fonc=0;
  if (is_modele_fonc)
    {
      /*
      DoubleTrav D(nb_faces_);
      DoubleTrav E(nb_faces_);
      DoubleTrav F1(nb_faces_);
      DoubleTrav F2(nb_faces_); */
      DoubleTab& D=ref_cast_non_const(DoubleTab,mon_modele_fonc.valeur().get_champ("D").valeurs());
      DoubleTab& E=ref_cast_non_const(DoubleTab,mon_modele_fonc.valeur().get_champ("E").valeurs());
      DoubleTab& F1=ref_cast_non_const(DoubleTab,mon_modele_fonc.valeur().get_champ("F1").valeurs());
      DoubleTab& F2=ref_cast_non_const(DoubleTab,mon_modele_fonc.valeur().get_champ("F2").valeurs());

      const Fluide_Incompressible& fluide=ref_cast(Fluide_Incompressible,eq_hydraulique.valeur().milieu());
      const Champ_Don& ch_visco_cin = fluide.viscosite_cinematique();
      // const DoubleTab& tab_visco = ch_visco_cin->valeurs();
      const Zone_Cl_dis& zcl_keps=mon_eq_transport_K_Eps->zone_Cl_dis();
      const Zone_dis& zone_dis_keps =mon_eq_transport_K_Eps->zone_dis();
      mon_modele_fonc.Calcul_D(D,zone_dis_keps,zcl_keps,vit,K_eps,ch_visco_cin);
      mon_modele_fonc.Calcul_E(E,zone_dis_keps,zcl_keps,vit,K_eps,ch_visco_cin,visco_turb);

      D.echange_espace_virtuel();
      E.echange_espace_virtuel();

      const Champ_base& ch_visco_cin_ou_dyn =ref_cast(Op_Diff_K_Eps_base, equation().operateur(0).l_op_base()).diffusivite();
      mon_modele_fonc.Calcul_F1(F1,zone_dis_keps,zcl_keps, P, K_eps,ch_visco_cin_ou_dyn);
      mon_modele_fonc.Calcul_F2(F2,D,zone_dis_keps,K_eps, ch_visco_cin_ou_dyn  );

      // Pour modele EASM
      const Zone_dis& zone_dis = eq_hydraulique->zone_dis();
      int nb_elem_tot = zone_VEF.nb_elem_tot();
      const Zone_Cl_dis& zone_Cl_dis = eq_hydraulique->zone_Cl_dis();

      int is_Reynolds_stress_isotrope = mon_modele_fonc.Calcul_is_Reynolds_stress_isotrope();
      if (is_Reynolds_stress_isotrope==0)
        {
          Cerr << "On utilise une diffusion turbulente non linaire dans le terme source P" << finl;
          const DoubleTab& visco_scal = ch_visco_cin->valeurs();
          DoubleTab visco_tab(nb_elem_tot);
          assert(sub_type(Champ_Uniforme,ch_visco_cin.valeur()));
          visco_tab = visco_scal(0,0);
          DoubleTab gradient_elem(nb_elem_tot,Objet_U::dimension,Objet_U::dimension);
          gradient_elem=0.;
          const Discretisation_base& dis = eq_hydraulique->discretisation();
          const Champ_base& K_Eps = mon_eq_transport_K_Eps->inconnue().valeur();
          Champ_P1NC::calcul_gradient(vit,gradient_elem,zone_Cl_VEF);
          /*Paroi*/
          Nom lp=mod.loi_paroi().valeur().que_suis_je();
          if (lp!="negligeable_VEF")
            {
              if (mon_equation->schema_temps().nb_pas_dt()>0)
                Champ_P1NC::calcul_duidxj_paroi(gradient_elem,visco_tab,visco_turb,tab,0,0,zone_Cl_VEF);
            }
          gradient_elem.echange_espace_virtuel();
          DoubleTab Re = mon_modele_fonc.calcul_tenseur_Re(dis,zone_dis,zone_Cl_dis,gradient_elem,visco_turb,K_Eps);
          Re.echange_espace_virtuel();

          calculer_terme_production_K_EASM(zone_VEF,zone_Cl_VEF,P,K_eps,gradient_elem,visco_turb,Re);
        }
      // Fin pour modele EASM
      for (int fac=0; fac<nb_faces_; fac++)
        {
          resu(fac,0) += (P(fac)-K_eps(fac,1)-D(fac))*volumes_entrelaces(fac);

          if (K_eps(fac,0) >= LeK_MIN)
            resu(fac,1) += ((C1*P(fac)*F1(fac)-C2*K_eps(fac,1)*F2(fac))*K_eps(fac,1)/(K_eps(fac,0))+E(fac)) *volumes_entrelaces(fac);
        }
    }
  else
    {

      for (int fac=0; fac<nb_faces_; fac++)
        {
          resu(fac,0) += (P(fac)-K_eps(fac,1))*volumes_entrelaces(fac);

          if (K_eps(fac,0) >= LeK_MIN)
            resu(fac,1) += (C1*P(fac)-C2*K_eps(fac,1))*volumes_entrelaces(fac)*K_eps(fac,1)/K_eps(fac,0);
        }
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
  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_th,G,scalaire,alpha_turb,ch_beta,g,0);

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

  int nb_consti = eq_concentration->constituant().nb_constituants();

  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_co,G,concen,alpha_turb,ch_beta_concen,g,nb_consti);

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

  int nb_consti = eq_concentration->constituant().nb_constituants();

  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_th,G_t,temper,alpha_turb,ch_beta_temper,g,0);
  calculer_terme_destruction_K_gen(zone_VEF,zcl_VEF_co,G_c,concen,alpha_turb,ch_beta_concen,g,nb_consti);

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




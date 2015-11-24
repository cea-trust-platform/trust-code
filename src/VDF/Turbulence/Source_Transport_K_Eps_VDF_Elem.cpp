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
// File:        Source_Transport_K_Eps_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/41
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Transport_K_Eps_VDF_Elem.h>
#include <Transport_K_Eps.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Probleme_base.h>
#include <IntTrav.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Champ_Face.h>
#include <Zone_Cl_VDF.h>
#include <Fluide_Quasi_Compressible.h>
#include <Debog.h>
#include <DoubleTrav.h>
#include <Pb_Hydraulique_Turbulent.h>
#include <Pb_Hydraulique_Concentration_Turbulent.h>
#include <Pb_Thermohydraulique_Turbulent_QC.h>
#include <Pb_Thermohydraulique_Turbulent.h>
#include <Pb_Thermohydraulique_Concentration_Turbulent.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_VDF_Elem,"Source_Transport_K_Eps_VDF_P0_VDF",Source_base);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_anisotherme_VDF_Elem,"Source_Transport_K_Eps_anisotherme_VDF_P0_VDF",Source_Transport_K_Eps_VDF_Elem);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_concen_VDF_Elem,"Source_Transport_K_Eps_aniso_concen_VDF_P0_VDF",Source_Transport_K_Eps_VDF_Elem);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem,"Source_Transport_K_Eps_aniso_therm_concen_VDF_P0_VDF",Source_Transport_K_Eps_VDF_Elem);

//// printOn
//

Sortie& Source_Transport_K_Eps_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_anisotherme_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_concen_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}


//// readOn
//
inline void error(const Nom& source, const Nom& problem)
{
  Cerr << "Error ! You can't use the " << source << " source term for the K-Eps equation of the problem: " << problem << finl;
  Cerr << "Check the user's manual. It is may be another source term Source_Transport_K_Eps_.... which should be used." << finl;
  Process::exit();
}

Entree& Source_Transport_K_Eps_VDF_Elem::readOn(Entree& is)
{
  const Probleme_base& problem = mon_equation->probleme();
  if (!sub_type(Pb_Hydraulique_Turbulent,problem) && !sub_type(Pb_Thermohydraulique_Turbulent_QC,problem)) error(que_suis_je(),problem.que_suis_je());
  Param param(que_suis_je());
  param.ajouter("C1_eps", &C1);
  param.ajouter("C2_eps", &C2);
  param.lire_avec_accolades(is);
  Cerr << "C1_eps = " << C1 << finl;
  Cerr << "C2_eps = " << C2 << finl;
  return is ;
}

Entree& Source_Transport_K_Eps_anisotherme_VDF_Elem::readOn(Entree& is)
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
  return is ;
}

Entree& Source_Transport_K_Eps_concen_VDF_Elem::readOn(Entree& is)
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
  return is ;
}

Entree& Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem::readOn(Entree& is)
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
  return is ;
}



/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

void Source_Transport_K_Eps_VDF_Elem::associer_zones(const Zone_dis& zone_dis,
                                                     const Zone_Cl_dis&  )
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
}

// remplit les references eq_hydraulique et mon_eq_transport_K_Eps
void Source_Transport_K_Eps_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  eq_hydraulique = pb.equation(0);
  mon_eq_transport_K_Eps = ref_cast(Transport_K_Eps,equation());
}

DoubleTab& Source_Transport_K_Eps_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& visco_turb = mon_eq_transport_K_Eps->modele_turbulence().viscosite_turbulente().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();

  int nb_elem = zone_VDF.nb_elem();

  // Ajout d'un espace virtuel au tableu P
  DoubleVect P;
  zone_VDF.zone().creer_tableau_elements(P);

  if (axi)
    {
      const Champ_Face& vitesse = ref_cast(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps,visco_turb);
    }
  else
    {
      const Champ_Face& vitesse = ref_cast(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps,vit,vitesse,visco_turb);
    }

  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();
  //const Mod_turb_hyd_RANS& mod_turb_RANS = ref_cast(Mod_turb_hyd_RANS,eq_hydraulique->modele_turbulence().valeur());
  //double LeK_MIN = mod_turb_RANS.get_LeK_MIN() ;
  for (int elem=0; elem<nb_elem; elem++)
    {
      resu(elem,0) += (P(elem)-K_eps(elem,1))*volumes(elem)*porosite_vol(elem);

      if (K_eps(elem,0) >= LeK_MIN)
        resu(elem,1) += (C1*P(elem)- C2*K_eps(elem,1))*volumes(elem)*porosite_vol(elem)*K_eps(elem,1)/K_eps(elem,0);

    }

  //Debog::verifier("Source_Transport_K_Eps_VDF_Elem::ajouter resu",resu);
  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Source_Transport_K_Eps_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu =0.;
  return ajouter(resu);
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_anisotherme_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////
//

// remplit les references eq_thermique et beta
void Source_Transport_K_Eps_anisotherme_VDF_Elem::associer_pb(const Probleme_base& pb)
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
  Source_Transport_K_Eps_VDF_Elem::associer_pb(pb);

  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,eqn);
  eq_thermique = eqn_th;

  beta_t=fluide.beta_t();
  gravite = fluide.gravite();
}

DoubleTab& Source_Transport_K_Eps_anisotherme_VDF_Elem::ajouter(DoubleTab& resu) const
{
  Source_Transport_K_Eps_VDF_Elem::ajouter(resu);
  //
  //// Modifs VB : plutot que de calculer P, on appelle Source_Transport_K_Eps_VDF_Elem::ajouter(resu)
  //// et on ajoute directement G
  ////
  //
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& scalaire = eq_thermique->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb =        le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleTab& g = gravite->valeurs();
  const Champ_Don& ch_beta = beta_t.valeur();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();

  // Ajout d'un espace virtuel au tableau G
  DoubleVect G;
  zone_VDF.zone().creer_tableau_elements(G);

  //else
  //  calculer_terme_production_K(zone_VDF,P,K_eps,vit,visco_turb);

  // C'est l'objet de type zone_Cl_dis de l'equation thermique
  // qui est utilise dans le calcul de G

  const DoubleTab& tab_beta = ch_beta.valeurs();
  if (sub_type(Champ_Uniforme,ch_beta.valeur()))
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta(0,0),g);
  else
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta,g);

  double C3_loc ;
  //const Mod_turb_hyd_RANS& mod_turb_RANS = ref_cast(Mod_turb_hyd_RANS,eq_hydraulique->modele_turbulence().valeur());
  //double LeK_MIN = mod_turb_RANS.get_LeK_MIN() ;
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN() ;
  int nb_elem = zone_VDF.nb_elem();
  for (int elem=0; elem<nb_elem; elem++)
    {
      resu(elem,0) += G(elem)*volumes(elem)*porosite_vol(elem);

      if (K_eps(elem,0) >= LeK_MIN)
        {
          C3_loc = C3 ;
          if ( G(elem) > 0. ) C3_loc = 0. ;

          resu(elem,1) += (1-C3_loc)*G(elem)  *volumes(elem)*porosite_vol(elem)
                          * K_eps(elem,1)/K_eps(elem,0);
        }
    }

  //Debog::verifier("Source_Transport_K_Eps_VDF_Elem::ajouter : " ,resu);
  return resu;
}

DoubleTab& Source_Transport_K_Eps_anisotherme_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_concen_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////
//

// remplit les references
void Source_Transport_K_Eps_concen_VDF_Elem::associer_pb(const Probleme_base& pb)
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
  Source_Transport_K_Eps_VDF_Elem::associer_pb(pb);

  const Convection_Diffusion_Concentration& eqn_c =
    ref_cast(Convection_Diffusion_Concentration,eqn);
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

DoubleTab& Source_Transport_K_Eps_concen_VDF_Elem::ajouter(DoubleTab& resu) const
{
  Source_Transport_K_Eps_VDF_Elem::ajouter(resu);
  //
  //// Modifs VB : plutot que de calculer P, on appelle Source_Transport_K_Eps_VDF_Elem::ajouter(resu)
  //// et on ajoute directement G
  ////
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF_co = ref_cast(Zone_Cl_VDF,eq_concentration->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& concen = eq_concentration->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_concentration->get_modele(TURBULENCE).valeur());
  const DoubleTab& diffu_turb  = le_modele_scalaire.diffusivite_turbulente().valeurs();
  const Champ_Uniforme& ch_beta_concen = ref_cast(Champ_Uniforme, beta_c->valeur());
  const DoubleVect& g = gravite->valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  int nb_consti = eq_concentration->nb_constituants();

  // Ajout d'un espace virtuel au tableau G
  DoubleVect G;
  zone_VDF.zone().creer_tableau_elements(G);

  if (nb_consti == 1)
    {
      double d_beta_c = ch_beta_concen(0,0);
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_co,G,
                                   concen,diffu_turb,d_beta_c,g);
    }
  else
    {
      const DoubleVect& d_beta_c = ch_beta_concen.valeurs();
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_co,G,
                                   concen,diffu_turb,d_beta_c,g,
                                   nb_consti);
    }

  double C3_loc;
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();
  //const Mod_turb_hyd_RANS& mod_turb_RANS = ref_cast(Mod_turb_hyd_RANS,eq_hydraulique->modele_turbulence().valeur());
  //double LeK_MIN = mod_turb_RANS.get_LeK_MIN() ;
  for (int elem=0; elem<nb_elem; elem++)
    {

      resu(elem,0) += G(elem)*volumes(elem)*porosite_vol(elem);

      if (K_eps(elem,0) >= LeK_MIN)
        {
          C3_loc = C3 ;
          if ( G(elem) > 0. ) C3_loc = 0. ;
          resu(elem,1) += (1.-C3_loc)*G(elem) *volumes(elem)*porosite_vol(elem)
                          * K_eps(elem,1)/K_eps(elem,0);
        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_concen_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

// remplit les references eq_thermique, eq_concentration, beta_t_, beta_c_
void Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem::associer_pb(const Probleme_base& pb)
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
  Source_Transport_K_Eps_VDF_Elem::associer_pb(pb);

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

DoubleTab& Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem::ajouter(DoubleTab& resu) const
{

  Source_Transport_K_Eps_VDF_Elem::ajouter(resu);
  //
  //
  //// Modifs VB : plutot que de calculer P, on appelle Source_Transport_K_Eps_VDF_Elem::ajouter(resu)
  //// et on ajoute directement G
  ////
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF_co = ref_cast(Zone_Cl_VDF,eq_concentration->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& temper = eq_thermique->inconnue().valeurs();
  const DoubleTab& concen = eq_concentration->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb =        le_modele_scalaire.diffusivite_turbulente().valeurs();
  const Modele_turbulence_scal_base& le_modele_scal_co =
    ref_cast(Modele_turbulence_scal_base,eq_concentration->get_modele(TURBULENCE).valeur());
  const DoubleTab& diffu_turb =        le_modele_scal_co.diffusivite_turbulente().valeurs();
  const DoubleVect& g = gravite->valeurs();
  const Champ_Don& ch_beta_temper = beta_t.valeur();
  if (!beta_c->non_nul())
    {
      Cerr << finl << "Le champ beta_co n'a pas ete renseigne pour le fluide dans le jeu de donnees." << finl;
      exit();
    }
  const Champ_Uniforme& ch_beta_concen = ref_cast(Champ_Uniforme, beta_c->valeur());

  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  int nb_consti = eq_concentration->nb_constituants();

  // Ajout d'un espace virtuel au tableaux Gt et Gc
  DoubleVect G_t;
  DoubleVect G_c;
  zone_VDF.zone().creer_tableau_elements(G_t);
  zone_VDF.zone().creer_tableau_elements(G_c);
  //Cerr<<"!!!!!>>>!!!! nb elem de G_t :"<<G_t.dimension(0);
  //Cerr<<"!!!!!>>>!!!! nb elem de P :"<<P.dimension(0);
  //  DoubleTrav P(nb_elem_tot);
  //   DoubleTrav G_t(nb_elem_tot);
  //   DoubleTrav G_c(nb_elem_tot);

  //else
  //calculer_terme_production_K(zone_VDF,P,K_eps,vit,visco_turb);

  const DoubleTab& tab_beta_t = ch_beta_temper.valeurs();
  if (sub_type(Champ_Uniforme,ch_beta_temper.valeur()))
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G_t,temper,alpha_turb,tab_beta_t(0,0),g);
  else
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G_t,temper,alpha_turb,tab_beta_t,g);

  if (nb_consti == 1)
    {
      double d_beta_c = ch_beta_concen(0,0);
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_co,G_c,
                                   concen,diffu_turb,d_beta_c,g);
    }
  else
    {
      const DoubleVect& d_beta_c = ch_beta_concen.valeurs();
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_co,G_c,
                                   concen,diffu_turb,d_beta_c,g,
                                   nb_consti);
    }

  double C3_loc,G_sum;
  //const Mod_turb_hyd_RANS& mod_turb_RANS = ref_cast(Mod_turb_hyd_RANS,eq_hydraulique->modele_turbulence().valeur());
  //double LeK_MIN = mod_turb_RANS.get_LeK_MIN() ;
  double LeK_MIN = mon_eq_transport_K_Eps->modele_turbulence().get_LeK_MIN();

  for (int elem=0; elem<nb_elem; elem++)
    {

      G_sum = G_t(elem)+G_c(elem) ;

      resu(elem,0) += G_sum *volumes(elem)*porosite_vol(elem);

      if (K_eps(elem,0) >= LeK_MIN)
        {
          C3_loc = C3 ;
          if ( G_sum > 0. ) C3_loc = 0. ;
          resu(elem,1) += (1.-C3_loc)*G_sum *volumes(elem)*porosite_vol(elem)
                          * K_eps(elem,1)/K_eps(elem,0);
        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_aniso_therm_concen_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}


void  Source_Transport_K_Eps_VDF_Elem::contribuer_a_avec(const DoubleTab& a,  Matrice_Morse& matrice) const
{

  const DoubleTab& val=equation().inconnue().valeurs();
  int size=val.dimension(0);

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleVect& porosite = zone_VDF.porosite_elem();
  // on implicite le -eps et le -eps^2/k

  {
    const DoubleVect& volumes=zone_VDF.volumes();
    for (int c=0; c<size; c++)
      {
        // -eps*vol  donne +vol dans la bonne case
        if (val(c,0)>DMINFLOAT)

          {

            double coef_k=porosite(c)*volumes(c)*val(c,1)/val(c,0);
            matrice(c*2,c*2)+=coef_k;

            double coef_eps=C2*coef_k;
            //    if (is_modele_fonc) coef_eps*=F2(c);
            matrice(c*2+1,c*2+1)+=coef_eps;
          }
      }
  }
}


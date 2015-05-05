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
// File:        Source_Transport_K_Eps_V2_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Transport_K_Eps_V2_VDF_Elem.h>
#include <Modele_turbulence_hyd_K_Eps_V2_VDF.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
//#include <Debog.h>

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_V2_VDF_Elem,"Source_Transport_K_Eps_V2_VDF_P0_VDF",Source_base);

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_V2_anisotherme_VDF_Elem,"Source_Transport_K_Eps_V2_anisotherme_VDF_P0_VDF",Source_Transport_K_Eps_V2_VDF_Elem);

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem,"Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_P0_VDF",Source_Transport_K_Eps_V2_VDF_Elem);

//// printOn
//

Sortie& Source_Transport_K_Eps_V2_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_V2_anisotherme_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Source_Transport_K_Eps_V2_VDF_Elem::readOn(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_K_Eps" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_K_Eps_V2" << finl;
  Motcles les_mots(2);
  {
    les_mots[0] = "C1_eps";
    les_mots[1] = "C2_eps";
  }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> C1;
            break;
          }
        case 1 :
          {
            is >> C2;
            break;
          }
        default :
          {
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_K_Eps_V2" << finl;
            exit();
          }
        }

      is >> motlu;
    }

  return is ;
}

Entree& Source_Transport_K_Eps_V2_anisotherme_VDF_Elem::readOn(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_K_Eps_V2_anisotherme" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_K_Eps_V2" << finl;
  Motcles les_mots(2);
  {
    les_mots[0] = "C1_eps";
    les_mots[1] = "C2_eps";
    les_mots[2] = "C3_eps";
  }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> C1;
            break;
          }
        case 1 :
          {
            is >> C2;
            break;
          }
        case 2 :
          {
            is >> C3;
            break;
          }
        default :
          {
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_K_Eps_V2_anisotherme" << finl;
            exit();
          }
        }

      is >> motlu;
    }

  return is ;
}

Entree& Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem::readOn(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_K_Eps_V2_anisotherme" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_K_Eps" << finl;
  Motcles les_mots(2);
  {
    les_mots[0] = "C1_eps";
    les_mots[1] = "C2_eps";
    les_mots[2] = "C3_eps";
  }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> C1;
            break;
          }
        case 1 :
          {
            is >> C2;
            break;
          }
        case 2 :
          {
            is >> C3;
            break;
          }
        default :
          {
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_K_Eps_V2_anisotherme" << finl;
            exit();
          }
        }

      is >> motlu;
    }

  return is ;
}


/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_V2_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

void Source_Transport_K_Eps_V2_VDF_Elem::associer_zones(const Zone_dis& zone_dis,
                                                        const Zone_Cl_dis&  zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
}

// remplit les references eq_hydraulique et mon_eq_transport_K_Eps
void Source_Transport_K_Eps_V2_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  eq_hydraulique = pb.equation(0);
  mon_eq_transport_K_Eps = ref_cast(Transport_K_Eps_V2,equation());
  const Modele_turbulence_hyd_K_Eps_V2& modele = ref_cast(Modele_turbulence_hyd_K_Eps_V2,mon_eq_transport_K_Eps->modele_turbulence());
  mon_eq_transport_V2 = ref_cast(Transport_V2,modele.eqn_V2());
  mon_eq_f22 = ref_cast(EqnF22base,modele.eqn_F22());
}

DoubleTab& Source_Transport_K_Eps_V2_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& visco_turb = mon_eq_transport_K_Eps->modele_turbulence().viscosite_turbulente().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  Champ_Face& vit_bord = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
  const DoubleTab& v2 = mon_eq_transport_V2->inconnue().valeurs();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();

  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  //double Cmu = 0.19;

  // Ajout d'un espace virtuel au tableau P
  DoubleTab P;
  zone_VDF.zone().creer_tableau_elements(P);
  DoubleTab S(nb_elem);
  DoubleTab T;
  T.resize(nb_elem, Array_base::NOCOPY_NOINIT);
  T = 1.;

  //   if (axi)
  //     {
  //       Cerr << " axi n'est pas code " << finl ;
  //       exit() ;
  //       // calculer_terme_production_K_Axi(zone_VDF,vit_bord,P,K_eps,visco_turb);
  //     }
  //   else {
  //     calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps,vit,vit_bord,visco_turb);
  //   }



  //Extraction de la viscosite moleculaire
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  DoubleTab& tab_visco = ref_cast_non_const(DoubleTab,ch_visco_cin->valeurs());
  double visco=-1;
  int l_unif;

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;


  Modele_turbulence_hyd_K_Eps_V2_VDF& le_modele = ref_cast_non_const(Modele_turbulence_hyd_K_Eps_V2_VDF, mon_eq_transport_K_Eps->modele_turbulence());
  le_modele.calculer_Sij(zone_VDF,zone_Cl_VDF,S,vit,vit_bord);

  // Calcul production
  P = 0. ;
  for (int i=0; i<nb_elem; i++)
    {
      P(i) += 2.*visco_turb(i) ;
      P(i) *= S(i) ;
    }
  ref_cast_non_const( Modele_turbulence_hyd_K_Eps_V2_VDF,le_modele).Calculer_Echelle_T(K_eps,v2,S,tab_visco,visco,l_unif,T);
  //Cerr << " dans les sourcse k_eps_v2 T= " << T << finl;
  for (int elem=0; elem<nb_elem; elem++)
    {
      double C1_prime;
      // nouvelle constante C1
      if (v2(elem) != 0.)
        {
          double temporaire = sqrt(K_eps(elem,0)/v2(elem));
          C1_prime = 1.4*(1+0.045*temporaire);
        }
      else
        C1_prime = 1.4;

      // Cerr << " source ke v2 C1_prime = "   << C1_prime  << " c2 = " << C2 << finl ;
      resu(elem,0) += (P(elem)-K_eps(elem,1))*volumes(elem)*porosite_vol(elem);
      resu(elem,1) += (C1_prime*P(elem)- C2*K_eps(elem,1))/T(elem)*volumes(elem)*porosite_vol(elem);
    }
  //Debog::verifier("Source_Transport_K_Eps_VDF_Elem::ajouter resu",resu);
  resu.echange_espace_virtuel();

  return resu;
}

DoubleTab& Source_Transport_K_Eps_V2_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu =0.;
  return ajouter(resu);
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_V2_anisotherme_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////
//

// remplit les references eq_thermique et beta
void Source_Transport_K_Eps_V2_anisotherme_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  Source_Transport_K_Eps_V2_VDF_Elem::associer_pb(pb);
  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,pb.equation(1));
  eq_thermique = eqn_th;
  const Fluide_Incompressible& fluide = eq_thermique->fluide();
  beta_t = fluide.beta_t();
  gravite = fluide.gravite();
}


DoubleTab& Source_Transport_K_Eps_V2_anisotherme_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& scalaire = eq_thermique->inconnue().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleTab& visco_turb = mon_eq_transport_K_Eps->modele_turbulence().viscosite_turbulente().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb = le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleTab& g = gravite->valeurs();
  const Champ_Don& ch_beta = beta_t.valeur();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();

  int nb_elem = zone_VDF.nb_elem();
  //  int nb_elem_tot = zone_VDF.nb_elem_tot();
  // Ajout d'un espace virtuel au tableaux P et G
  DoubleTab P, G;
  zone_VDF.zone().creer_tableau_elements(P);
  zone_VDF.zone().creer_tableau_elements(G);
  double T;
  //Cerr<<"!!!!!>>>!!!! nb elem de G :"<<G.dimension(0);
  //Cerr<<"!!!!!>>>!!!! nb elem de P :"<<P.dimension(0);
  //  DoubleTrav P(nb_elem_tot);
  //  DoubleTrav G(nb_elem_tot);

  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps,vit,vitesse,visco_turb);
    }

  //Extraction de la viscosite moleculaire
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  double visco=-1;
  int l_unif;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;
  // C'est l'objet de type zone_Cl_dis de l'equation thermique
  // qui est utilise dans le calcul de G

  const DoubleTab& tab_beta = ch_beta.valeurs();
  if (sub_type(Champ_Uniforme,ch_beta.valeur()))
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta(0,0),g);
  else
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta,g);

  for (int elem=0; elem<nb_elem; elem++)
    {
      if ( l_unif == 1)
        {
          if ( K_eps(elem,1)>1.e-10)
            T = max(K_eps(elem,0)/K_eps(elem,1),6*sqrt(visco/K_eps(elem,1)));
          else T = 1.;
        }
      else
        {
          if ( K_eps(elem,1)>1.e-10)
            T = max(K_eps(elem,0)/K_eps(elem,1),6*sqrt(tab_visco[elem]/K_eps(elem,1)));
          else T = 1.;
        }
      resu(elem,0) += (P(elem)-K_eps(elem,1))*volumes(elem)*porosite_vol(elem);

      resu(elem,1) += (C1*P(elem)- C2*K_eps(elem,1))/T*volumes(elem)*porosite_vol(elem);

      if ( (G(elem)>0) && (K_eps(elem,0) >= 10.e-8) )
        {
          resu(elem,0) += G(elem)*volumes(elem)*porosite_vol(elem);
          resu(elem,1) += C1*(1-C3)*G(elem)*K_eps(elem,1)*volumes(elem)*porosite_vol(elem)
                          /K_eps(elem,0);
        }
    }
  //Debog::verifier("Source_Transport_K_Eps_VDF_Elem::ajouter : " ,resu);
  return resu;
}

DoubleTab& Source_Transport_K_Eps_V2_anisotherme_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

// remplit les references eq_thermique, eq_concentration, beta_t_, beta_c_
void Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  Source_Transport_K_Eps_V2_VDF_Elem::associer_pb(pb);

  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,pb.equation(1));
  eq_thermique = eqn_th;
  const Convection_Diffusion_Concentration& eqn_c =
    ref_cast(Convection_Diffusion_Concentration,pb.equation(2));
  eq_concentration = eqn_c;
  const Fluide_Incompressible& fluide = eq_thermique->fluide();
  beta_t = fluide.beta_t();
  beta_c = fluide.beta_c();
  gravite = fluide.gravite();
}

DoubleTab& Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF_co = ref_cast(Zone_Cl_VDF,eq_concentration->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& temper = eq_thermique->inconnue().valeurs();
  const DoubleTab& concen = eq_concentration->inconnue().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleTab& visco_turb = mon_eq_transport_K_Eps->modele_turbulence().viscosite_turbulente().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb = le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleVect& g = gravite->valeurs();
  const Champ_Don& ch_beta_temper = beta_t.valeur();
  if (!beta_c->non_nul())
    {
      Cerr << finl << "The beta_co field has not been indicated for the medium in the data set." << finl;
      exit();
    }
  const Champ_Uniforme& ch_beta_concen = ref_cast(Champ_Uniforme, beta_c->valeur());
  double T;
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  //  int nb_elem_tot = zone_VDF.nb_elem_tot();
  int nb_consti = eq_concentration->nb_constituants();

  // Ajout d'un espace virtuel au tableaux P, Gt et Gc
  DoubleTab P, G_t, G_c;
  zone_VDF.zone().creer_tableau_elements(P);
  zone_VDF.zone().creer_tableau_elements(G_t);
  zone_VDF.zone().creer_tableau_elements(G_c);
  //Cerr<<"!!!!!>>>!!!! nb elem de G_t :"<<G_t.dimension(0);
  //Cerr<<"!!!!!>>>!!!! nb elem de P :"<<P.dimension(0);
  //  DoubleTrav P(nb_elem_tot);
  //   DoubleTrav G_t(nb_elem_tot);
  //   DoubleTrav G_c(nb_elem_tot);

  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps,vit,vitesse,visco_turb);
    }

  //Extraction de la viscosite moleculaire
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  double visco=-1;
  int l_unif;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;

  const DoubleTab& tab_beta_t = ch_beta_temper.valeurs();
  if (sub_type(Champ_Uniforme,ch_beta_temper.valeur()))
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G_t,temper,alpha_turb,tab_beta_t(0,0),g);
  else
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G_t,temper,alpha_turb,tab_beta_t,g);

  if (nb_consti == 1)
    {
      double d_beta_c = ch_beta_concen(0,0);
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_co,G_c,
                                   concen,alpha_turb,d_beta_c,g);
    }
  else
    {
      const DoubleVect& d_beta_c = ch_beta_concen.valeurs();
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_co,G_c,
                                   concen,alpha_turb,d_beta_c,g,
                                   nb_consti);
    }

  for (int elem=0; elem<nb_elem; elem++)
    {
      if ( l_unif == 1)
        {
          if ( K_eps(elem,1)>1.e-10)
            T = max(K_eps(elem,0)/K_eps(elem,1),6*sqrt(visco/K_eps(elem,1)));
          else T = 1.;
        }
      else
        {
          if ( K_eps(elem,1)>1.e-10)
            T = max(K_eps(elem,0)/K_eps(elem,1),6*sqrt(tab_visco[elem]/K_eps(elem,1)));
          else T = 1.;
        }
      resu(elem,0) += (P(elem)-K_eps(elem,1))*volumes(elem)*porosite_vol(elem);

      resu(elem,1) += (C1*P(elem)- C2*K_eps(elem,1))/T*volumes(elem)*porosite_vol(elem);

      if ( ((G_t(elem)+G_c(elem))>0 ) && (K_eps(elem,0) >= 1.e-8) )
        {
          resu(elem,0) += (G_t(elem)+G_c(elem))*volumes(elem)*porosite_vol(elem);
          resu(elem,1) += C1*(1-C3)*(G_t(elem)+G_c(elem))*K_eps(elem,1)*volumes(elem)*porosite_vol(elem)
                          /K_eps(elem,0);
        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_V2_aniso_therm_concen_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}



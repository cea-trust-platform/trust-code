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
// File:        Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////


#include <Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Face.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <DoubleTrav.h>
#include <Fluide_Quasi_Compressible.h>

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem,"Source_Transport_K_Eps_Bas_Reynolds_VDF_P0_VDF",Source_base);

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem,"Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_P0_VDF",Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem);
Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem,"Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_P0_VDF",Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem);

Implemente_instanciable_sans_constructeur(Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen_VDF_Elem,"Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen_VDF_P0_VDF",Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem);


//// printOn
//

Sortie& Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}
Sortie& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_K_Eps_Bas_Reynolds" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_K_Eps_Bas_Reynolds" << finl;
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
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_K_Eps_Bas_Reynolds" << finl;
            exit();
          }
        }

      is >> motlu;
    }
  return is;
}

Entree& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem::readOn(Entree& s)
{
  return s ;
}
Entree& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem::readOn(Entree& s)
{
  return s ;
}

Entree& Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen_VDF_Elem::readOn(Entree& s)
{
  return s ;
}



/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

void Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::associer_pb(const Probleme_base& pb )
{
  eq_hydraulique = pb.equation(0);
  eqn_keps_bas_re = ref_cast(Transport_K_Eps_Bas_Reynolds,equation());
}

void Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::associer_zones(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}

DoubleTab& Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_Cl_dis& zcl_keps=eqn_keps_bas_re->zone_Cl_dis();
  const Zone_dis& zone_dis_keps =eqn_keps_bas_re ->zone_dis();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,eq_hydraulique->zone_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const  DoubleTab& K_eps_Bas_Re = eqn_keps_bas_re->inconnue().valeurs();
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,eqn_keps_bas_re->modele_turbulence());
  const DoubleTab& visco_turb = mod_turb.viscosite_turbulente().valeurs();
  const Modele_Fonc_Bas_Reynolds& mon_modele_fonc = mod_turb.associe_modele_fonction();
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_cin = fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
    }
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  //  int nb_elem_tot = zone_VDF.nb_elem_tot();
  DoubleTrav P(visco_turb);
  DoubleTrav D(visco_turb);
  DoubleTrav E(visco_turb);
  DoubleTrav F1(nb_elem);
  DoubleTrav F2(nb_elem);

  mon_modele_fonc.Calcul_D(D,zone_dis_keps,zcl_keps,vit,K_eps_Bas_Re,visco);
  D.echange_espace_virtuel();
  mon_modele_fonc.Calcul_E(E,zone_dis_keps,zcl_keps,vit,K_eps_Bas_Re,visco,visco_turb);
  E.echange_espace_virtuel();
  mon_modele_fonc.Calcul_F1(F1,zone_dis_keps);
  mon_modele_fonc.Calcul_F2(F2,D,zone_dis_keps,K_eps_Bas_Re,visco );

  // Rq : la distinction entre zone_cl est importante pour les deux equations pour l imposition des conditions aux limites!!!!!!
  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps_Bas_Re,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps_Bas_Re,vit,vitesse,visco_turb);
    }

  P.echange_espace_virtuel();

  for (int elem=0; elem<nb_elem; elem++)
    {
      if (K_eps_Bas_Re(elem,0) > 1.e-10 && K_eps_Bas_Re(elem,1) > 1.e-10)
        {
          resu(elem,0) += (P(elem)-K_eps_Bas_Re(elem,1)-D(elem))*volumes(elem)*porosite_vol(elem);
          resu(elem,1) += ((C1*F1(elem)*P(elem)- C2*F2(elem)*K_eps_Bas_Re(elem,1))*K_eps_Bas_Re(elem,1)/K_eps_Bas_Re(elem,0)+E(elem))*volumes(elem)*porosite_vol(elem);
        }
      else
        {
          resu(elem,0) += 0.;
          resu(elem,1) += 0.;
        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(resu);
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////
//

// remplit les references eq_thermique et beta

void Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem::associer_pb(const Probleme_base& pb)
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
  Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::associer_pb(pb);

  const Convection_Diffusion_Temperature& eqn_th = ref_cast(Convection_Diffusion_Temperature,eqn);
  eq_thermique = eqn_th;
  beta_t = fluide.beta_t();
  gravite = fluide.gravite();
}

DoubleTab& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_Cl_dis& zcl=eq_hydraulique->zone_Cl_dis();
  const Zone_Cl_dis& zcl_keps=eqn_keps_bas_re->zone_Cl_dis();
  const Zone_dis& zone_dis_keps =eqn_keps_bas_re ->zone_dis();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,eq_hydraulique->zone_dis().valeur());
  const Zone_Cl_VDF& zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zcl.valeur());
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps_Bas_Re = eqn_keps_bas_re->inconnue().valeurs();
  const DoubleTab& scalaire = eq_thermique->inconnue().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleTab& visco_turb = eqn_keps_bas_re->modele_turbulence().viscosite_turbulente().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb =        le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleTab& g = gravite->valeurs();
  const Champ_Don& ch_beta = beta_t.valeur();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_cin = fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
    }

  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,eqn_keps_bas_re->modele_turbulence());
  const Modele_Fonc_Bas_Reynolds& mon_modele_fonc = mod_turb.associe_modele_fonction();
  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  //  int nb_faces= zone_VDF.nb_faces();
  //  const DoubleVect& porosite_face = zone_VDF.porosite_face();
  DoubleTrav P(nb_elem_tot);
  DoubleTrav G(nb_elem_tot);
  DoubleTrav G1(nb_elem_tot);
  DoubleTrav D(nb_elem_tot);
  DoubleTrav E(nb_elem_tot);
  DoubleTrav F1(nb_elem_tot);
  DoubleTrav F2(nb_elem_tot);
  mon_modele_fonc.Calcul_D(D,zone_dis_keps,zcl_keps,vit,K_eps_Bas_Re,visco);
  mon_modele_fonc.Calcul_E(E,zone_dis_keps,zcl_keps,vit,K_eps_Bas_Re,visco,visco_turb);
  mon_modele_fonc.Calcul_F1(F1,zone_dis_keps);
  mon_modele_fonc.Calcul_F2(F2,D,zone_dis_keps,K_eps_Bas_Re,visco);

  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps_Bas_Re,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zone_Cl_VDF,P,K_eps_Bas_Re,vit,vitesse,visco_turb);
    }

  // C'est l'objet de type zone_Cl_dis de l'equation thermique
  // qui est utilise dans le calcul de G

  const DoubleTab& tab_beta = ch_beta.valeurs();

  // Nous utilisons le modele de fluctuation thermique pour le calcul du terme de destruction G.
  if (sub_type(Champ_Uniforme,ch_beta.valeur()))
    {
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta(0,0),g);
    }
  else
    {
      calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta,g);
    }
  for (int elem=0; elem<nb_elem; elem++)
    {
      resu(elem,0) += (P(elem)-K_eps_Bas_Re(elem,1)-D(elem))*volumes(elem)*porosite_vol(elem);
      if (K_eps_Bas_Re(elem,0) >= 1.e-10)
        resu(elem,1) += (C1*F1(elem)*P(elem)- C2*F2(elem)*K_eps_Bas_Re(elem,1))*K_eps_Bas_Re(elem,1)/K_eps_Bas_Re(elem,0)*volumes(elem)*porosite_vol(elem)  + E(elem)*volumes(elem)*porosite_vol(elem);

      if ( (G1(elem)>0) && (K_eps_Bas_Re(elem,1) >= 10.e-10) )
        {
          resu(elem,0) += G(elem)*volumes(elem)*porosite_vol(elem);
          resu(elem,1) += C1*F1(elem)*G(elem)*volumes(elem)*porosite_vol(elem)*K_eps_Bas_Re(elem,1)/K_eps_Bas_Re(elem,0);
        }
    }
  return resu;
}

DoubleTab& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_VDF_Elem::calculer(DoubleTab& resu) const
{

  resu = 0.;
  return ajouter(resu);

}
/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////
//
DoubleTab& Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_dis& z = eq_hydraulique->zone_dis();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,z.valeur());
  const Zone_Cl_VDF& zcl_VDF = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleTab& K_eps_Bas_Re = eqn_keps_bas_re->inconnue().valeurs();
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_dyn = fluide.viscosite_dynamique();
  const DoubleTab& tab_visco = ch_visco_dyn->valeurs();
  const DoubleTab& visco_turb = eqn_keps_bas_re->modele_turbulence().viscosite_turbulente().valeurs();

  int nb_elem_tot = zone_VDF.nb_elem_tot();
  int nb_elem = zone_VDF.nb_elem();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();

  //Calcul des fonctions F1 et F2
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,eqn_keps_bas_re->modele_turbulence());
  const Modele_Fonc_Bas_Reynolds& mon_modele_fonc = mod_turb.associe_modele_fonction();
  DoubleTrav P(nb_elem_tot);
  DoubleTrav F1(nb_elem_tot);
  DoubleTrav F2(nb_elem_tot);
  mon_modele_fonc.Calcul_F1(F1,z);
  if (sub_type(Champ_Uniforme,ch_visco_dyn.valeur()))
    {
      double visco;
      visco = max(tab_visco(0,0),DMINFLOAT);
      mon_modele_fonc.Calcul_F2(F2,P,z,K_eps_Bas_Re,visco);
    }
  else
    {
      mon_modele_fonc.Calcul_F2(F2,P,z,K_eps_Bas_Re,tab_visco);
    }

  //Calcul du terme de production
  Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
  calculer_terme_production_K(zone_VDF,zcl_VDF,P,K_eps_Bas_Re,vit,vitesse,visco_turb);

  //essai
  //   const Zone_Cl_dis& zcl=eq_hydraulique->zone_Cl_dis();
  //   DoubleTrav D(nb_elem_tot);
  //   if (sub_type(Champ_Uniforme,ch_visco_dyn.valeur())) {
  //     double visco;
  //     visco = max(tab_visco(0,0),DMINFLOAT);
  //     mon_modele_fonc.Calcul_D(D,z,zcl,vit,K_eps_Bas_Re,visco);
  //   } else {
  //     Cerr<<"Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem visco non const non implemente"<<finl;
  //     abort();
  // //     mon_modele_fonc.Calcul_D(D,z,zcl,vit,K_eps_Bas_Re,tab_visco);
  //   }
  //   DoubleTrav E(nb_elem);
  //   if (sub_type(Champ_Uniforme,ch_visco_dyn.valeur())) {
  //     double visco;
  //     visco = max(tab_visco(0,0),DMINFLOAT);
  //     mon_modele_fonc.Calcul_E(E,z,zcl, vit,K_eps_Bas_Re,visco,visco_turb);
  //   } else {
  //     Cerr<<"Source_Transport_K_Eps_Bas_Reynolds_anisotherme_QC_VDF_Elem visco non const non implemente"<<finl;
  //     abort();
  // //     mon_modele_fonc.Calcul_E(E,z,zcl, vit,K_eps_Bas_Re,tab_visco,visco_turb);
  //   }
  //essai


  //Calcul du terme source
  //  sur k  :  P-eps
  //  sur eps:  (C1.f1.P - C2.F2.eps) * eps/k
  for (int elem=0; elem<nb_elem; elem++)
    {
      resu(elem,0) += (P(elem)-K_eps_Bas_Re(elem,1))*volumes(elem)*porosite_vol(elem);
      //     resu(elem,0) += (P(elem)-K_eps_Bas_Re(elem,1)-D(elem))*volumes(elem)*porosite_vol(elem);
      if (K_eps_Bas_Re(elem,0) >= 1.e-10)
        {
          resu(elem,1) += (C1*F1(elem)*P(elem)- C2*F2(elem)*K_eps_Bas_Re(elem,1)) * volumes(elem)*porosite_vol(elem) *(K_eps_Bas_Re(elem,1)/K_eps_Bas_Re(elem,0));
          //       resu(elem,1) += (C1*F1(elem)*P(elem)- C2*F2(elem)*K_eps_Bas_Re(elem,1)+E(elem)) * volumes(elem)*porosite_vol(elem) *(K_eps_Bas_Re(elem,1)/K_eps_Bas_Re(elem,0));
        }
    }

  return resu;
}

/////////////////////////////////////////////////////////////////////////////
//
//            Implementation des fonctions de la classe
//
//             Source_Transport_K_Eps_Bas_Reynold_aniso_therm_concen_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

// remplit les references eq_thermique, eq_concentration, beta_t_, beta_c_

void Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen_VDF_Elem::associer_pb(const Probleme_base& pb)
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
  Source_Transport_K_Eps_Bas_Reynolds_VDF_Elem::associer_pb(pb);

  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,eqn_therm);
  eq_thermique = eqn_th;
  const Convection_Diffusion_Concentration& eqn_c =
    ref_cast(Convection_Diffusion_Concentration,eqn_conc);
  eq_concentration = eqn_c;

  beta_t = fluide.beta_t();
  beta_c = fluide.beta_c();
  gravite = fluide.gravite();
}

DoubleTab& Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_Cl_dis& zcl=eq_hydraulique->zone_Cl_dis();
  const Zone_Cl_dis& zcl_keps=eqn_keps_bas_re->zone_Cl_dis();
  const Zone_dis& zone_dis_keps =eqn_keps_bas_re ->zone_dis();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,eq_hydraulique->zone_dis().valeur());
  const Zone_Cl_VDF& zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zcl.valeur());
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const Zone_Cl_VDF& zcl_VDF_co = ref_cast(Zone_Cl_VDF,eq_concentration->zone_Cl_dis().valeur());
  const DoubleTab& K_eps_Bas_Re = eqn_keps_bas_re->inconnue().valeurs();
  const DoubleTab& temper = eq_thermique->inconnue().valeurs();
  const DoubleTab& concen = eq_concentration->inconnue().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleTab& visco_turb = eqn_keps_bas_re->modele_turbulence().viscosite_turbulente().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire =
    ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const DoubleTab& alpha_turb =        le_modele_scalaire.diffusivite_turbulente().valeurs();
  const DoubleVect& g = gravite->valeurs();
  const Champ_Don& ch_beta_temper = beta_t.valeur();
  if (!beta_c->non_nul())
    {
      Cerr << finl << "Le champ beta_co n'a pas ete renseigne pour le fluide dans le jeu de donnees." << finl;
      exit();
    }
  const Champ_Uniforme& ch_beta_concen = ref_cast(Champ_Uniforme, beta_c->valeur());
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,eq_hydraulique->milieu());
  const Champ_Don& ch_visco_cin = fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
    }
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  int nb_consti = eq_concentration->nb_constituants();
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,eqn_keps_bas_re->modele_turbulence());
  const Modele_Fonc_Bas_Reynolds& mon_modele_fonc = ref_cast(Modele_Fonc_Bas_Reynolds,mod_turb.associe_modele_fonction());
  DoubleTrav P(nb_elem_tot);
  DoubleTrav G_t(nb_elem_tot);
  DoubleTrav G_c(nb_elem_tot);
  DoubleTrav D(nb_elem_tot);
  DoubleTrav E(nb_elem_tot);
  DoubleTrav F1(nb_elem_tot);
  DoubleTrav F2(nb_elem_tot);
  mon_modele_fonc.Calcul_D(D,zone_dis_keps,zcl_keps,vit,K_eps_Bas_Re,visco);
  mon_modele_fonc.Calcul_E(E,zone_dis_keps,zcl_keps,vit,K_eps_Bas_Re,visco,visco_turb);
  mon_modele_fonc.Calcul_F1(F1,zone_dis_keps);
  //  mon_modele_fonc.Calcul_F1(F1,z,K_eps_Bas_Re,visco);
  mon_modele_fonc.Calcul_F2(F2,D,zone_dis_keps,K_eps_Bas_Re,visco);

  //        double T_durbin, T_kolmo, T_ke;

  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps_Bas_Re,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zone_Cl_VDF,P,K_eps_Bas_Re,vit,vitesse,visco_turb);
    }

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
      resu(elem,0) += (P(elem)-K_eps_Bas_Re(elem,1)-D(elem))*volumes(elem)*porosite_vol(elem);

      if (K_eps_Bas_Re(elem,0) >= 1.e-10)
        resu(elem,1) += (C1*F1(elem)*P(elem)- C2*F2(elem)*K_eps_Bas_Re(elem,1))*volumes(elem)*porosite_vol(elem) *K_eps_Bas_Re(elem,0)/K_eps_Bas_Re(elem,1) + E(elem)*volumes(elem)*porosite_vol(elem);

      if ( ((G_t(elem)+G_c(elem))>0 ) && (K_eps_Bas_Re(elem,1) >= 1.e-10) )
        {
          resu(elem,0) += (G_t(elem)+G_c(elem))*volumes(elem)*porosite_vol(elem);
          resu(elem,1) += C1*F1(elem)*(G_t(elem)+G_c(elem))*volumes(elem)*porosite_vol(elem)
                          *K_eps_Bas_Re(elem,0)/K_eps_Bas_Re(elem,1)+E(elem)*volumes(elem)*porosite_vol(elem);
        }
    }
  return resu;
}

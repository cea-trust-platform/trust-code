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
// File:        Source_Transport_Fluctuation_Temperature_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////


#include <Source_Transport_Fluctuation_Temperature_VDF_Elem.h>
#include <Convection_Diffusion_Temperature.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <IntTrav.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Champ_Face.h>
#include <Zone_Cl_VDF.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature.h>
#include <DoubleTrav.h>

Implemente_instanciable_sans_constructeur(Source_Transport_Fluctuation_Temperature_VDF_Elem,"Source_Transport_Fluctuation_Temperature_VDF_P0_VDF",Source_base);

//// printOn
//

Sortie& Source_Transport_Fluctuation_Temperature_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Source_Transport_Fluctuation_Temperature_VDF_Elem::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_Fluctuation_Temperature" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_Fluctuation_Temperature" << finl;
  Motcles les_mots(4);
  {
    les_mots[0] = "Ca";
    les_mots[1] = "Cb";
    les_mots[2] = "Cc";
    les_mots[3] = "Cd";
  }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> Ca;
            break;
          }
        case 1 :
          {
            is >> Cb;
            break;
          }
        case 2 :
          {
            is >> Cc;
            break;
          }
        case 3 :
          {
            is >> Cd;
            break;
          }
        default :
          {
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_Fluctuation_Temperature" << finl;
            exit();
          }
        }

      is >> motlu;
    }

  return is ;
}

void Source_Transport_Fluctuation_Temperature_VDF_Elem::associer_pb(const Probleme_base& pb )
{
  eq_hydraulique = pb.equation(0);
  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,pb.equation(1));
  eq_thermique = eqn_th;
  mon_eq_transport_Fluctu_Temp = ref_cast(Transport_Fluctuation_Temperature,equation());
  const Fluide_Incompressible& fluide = eq_thermique->fluide();
  beta_t = fluide.beta_t();
  gravite = fluide.gravite();
}

void Source_Transport_Fluctuation_Temperature_VDF_Elem::associer_zones(const Zone_dis& zone_dis,
                                                                       const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}


////////////////////////////////////////////////////////////
//
//   Methode pour determiner la production par le gradient
//   moyen de la temperature
////////////////////////////////////////////////////////////

DoubleTab& Source_Transport_Fluctuation_Temperature_VDF_Elem::calculer_Prod_uteta_T(const Zone_VDF& zone_VDF,
                                                                                    const Zone_Cl_VDF& zcl_VDF,
                                                                                    const DoubleTab& temper,
                                                                                    const  DoubleTab& u_teta,
                                                                                    DoubleTab& uteta_T) const
{
  int nb_faces= zone_VDF.nb_faces();
  const Zone& la_zone=zone_VDF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();
  IntTrav numfa(nb_faces_elem);
  const IntTab& les_elem_faces = zone_VDF.elem_faces();
  DoubleTrav grad_T(nb_faces);
  int nb_elem = zone_VDF.nb_elem();
  int n0,n1;
  double dist;
  int face;
  const IntTab& face_voisins = zone_VDF.face_voisins();
  uteta_T = 0;
  const DoubleVect& porosite_face = zone_VDF.porosite_face();
  // on calcul tout d'abord le gradient de temperature par faces.
  // Traitement des faces internes

  int premiere_face_int=zone_VDF.premiere_face_int();

  if (Objet_U::axi)

    for (face=premiere_face_int; face<nb_faces; face++)
      {
        n0 = face_voisins(face,0);
        n1 = face_voisins(face,1);
        dist = zone_VDF.dist_norm_axi(face);
        grad_T[face] = (temper[n1] - temper[n0])/dist;
      }
  else
    for (face=premiere_face_int; face<nb_faces; face++)
      {
        n0 = face_voisins(face,0);
        n1 = face_voisins(face,1);
        dist = zone_VDF.dist_norm(face);
        grad_T[face] = (temper[n1] - temper[n0])/dist;
      }

  // Traitement des conditions limites de type Entree_fluide_K_Eps_impose :

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zcl_VDF.les_conditions_limites(n_bord);

      if (sub_type(Entree_fluide_temperature_imposee,la_cl.valeur()) )
        {
          const Entree_fluide_temperature_imposee& la_cl_diri=ref_cast(Entree_fluide_temperature_imposee,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (face=ndeb; face<nfin; face++)
            {
              double T_imp = la_cl_diri.val_imp(face-ndeb);
              n0 = face_voisins(face,0);
              n1 = face_voisins(face,1);
              if (Objet_U::axi)
                // dist = 2*zone_VDF.dist_norm_bord_axi(face);
                dist = zone_VDF.dist_norm_bord_axi(face);
              else
                dist = zone_VDF.dist_norm_bord(face);
              if (n0 != -1)
                {
                  grad_T[face] = (T_imp-temper[n0])/dist;
                }
              else   // n1 != -1
                {
                  grad_T[face] = (temper[n1]-T_imp)/dist;
                }
            }
        }
    }  // fin des conditions limites

  // On calcul ensuite uteta gradT sur chaque element.
  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int i=0; i<nb_faces_elem; i++)
        numfa[i] = les_elem_faces(elem,i);

      uteta_T[elem] = (( 0.5*(u_teta[numfa[0]]*porosite_face(numfa[0]) + u_teta[numfa[dimension]]*porosite_face(numfa[dimension]))) * (0.5*(grad_T[numfa[0]]*porosite_face(numfa[0]) + grad_T[numfa[dimension]]*porosite_face(numfa[dimension]))))
                      + (( 0.5*(u_teta[numfa[1]]*porosite_face(numfa[1]) + u_teta[numfa[dimension+1]]*porosite_face(numfa[dimension+1])))*(0.5*(grad_T[numfa[1]]*porosite_face(numfa[1]) + grad_T[numfa[dimension+1]]*porosite_face(numfa[dimension+1]))));

      if (dimension ==3)
        uteta_T[elem] += ((0.5*(u_teta[numfa[2]]*porosite_face(numfa[2]) + u_teta[numfa[5]]*porosite_face(numfa[5])))
                          *(0.5*(grad_T[numfa[2]]*porosite_face(numfa[2])
                                 + grad_T[numfa[5]]*porosite_face(numfa[5]))));
    }
  return uteta_T;
}

DoubleTab& Source_Transport_Fluctuation_Temperature_VDF_Elem::ajouter(DoubleTab& resu) const
{

  const Zone_Cl_dis& zcl = eq_hydraulique->zone_Cl_dis();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,eq_hydraulique->zone_dis().valeur());
  const Zone& la_zone=zone_VDF.zone();
  const Zone_Cl_VDF& zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zcl.valeur());
  const RefObjU& modele_turbulence_hydr = eq_hydraulique->get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& modele_bas_Re = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,le_modele);
  const Transport_K_Eps_base& mon_eq_transport_K_Eps_Bas_Re = modele_bas_Re.eqn_transp_K_Eps();
  const DoubleTab& K_eps_Bas_Re = mon_eq_transport_K_Eps_Bas_Re.inconnue().valeurs();
  const DoubleTab& scalaire = eq_thermique->inconnue().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleTab& visco_turb = le_modele.viscosite_turbulente().valeurs();
  const DoubleTab& Fluctu_Temperature = mon_eq_transport_Fluctu_Temp->inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire = ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const Modele_turbulence_scal_Fluctuation_Temperature& modele_Flux_Chaleur = ref_cast(Modele_turbulence_scal_Fluctuation_Temperature,le_modele_scalaire);
  const Transport_Flux_Chaleur_Turbulente& mon_eq_transport_Flux_Chaleur_Turb =  modele_Flux_Chaleur.equation_Chaleur();
  const DoubleTab& Flux_Chaleur_Turb = mon_eq_transport_Flux_Chaleur_Turb.inconnue().valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  const DoubleTab& g = gravite->valeurs();
  const Champ_Don& ch_beta = beta_t.valeur();
  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  //  int nb_face = zone_VDF.nb_faces();
  //   const IntTab& face_voisins = zone_VDF.face_voisins();
  //   const IntVect& orientation = zone_VDF.orientation();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  //   const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  DoubleTrav uteta_T(nb_elem_tot);
  DoubleTrav P(nb_elem_tot);
  DoubleTrav G(nb_elem_tot);
  calculer_Prod_uteta_T(zone_VDF,zone_Cl_VDF,scalaire,Flux_Chaleur_Turb,uteta_T);
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
  if (sub_type(Champ_Uniforme,ch_beta.valeur()))
    // calculer_terme_g(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta(0,0),g);
    {
      int nb_faces_elem =la_zone.nb_faces_elem();
      IntTrav numfa(nb_faces_elem);
      DoubleVect coef(Objet_U::dimension);
      const IntTab& les_elem_faces = zone_VDF.elem_faces();
      for (int elem=0; elem<nb_elem; elem++)
        {
          for (int i=0; i<nb_faces_elem; i++)
            numfa[i] = les_elem_faces(elem,i);

          coef(0) = 0.5*(Flux_Chaleur_Turb(numfa[0])*porosite_surf(numfa[0])
                         + Flux_Chaleur_Turb(numfa[dimension])*porosite_surf(numfa[dimension]));
          coef(1) = 0.5*(Flux_Chaleur_Turb(numfa[1])*porosite_surf(numfa[1])
                         + Flux_Chaleur_Turb(numfa[dimension+1])*porosite_surf(numfa[dimension+1]));

          if (dimension ==2)
            G[elem] = tab_beta(0,0)*(g(0,0)*coef(0) + g(0,1)*coef(1));

          else if (dimension == 3)
            {
              coef(2) = 0.5*(Flux_Chaleur_Turb(numfa[2])*porosite_surf(numfa[2])
                             + Flux_Chaleur_Turb(numfa[5])*porosite_surf(numfa[5]));
              G[elem] = tab_beta(0,0)*(g(0,0)*coef(0) + g(0,1)*coef(1) + g(0,2)*coef(2));
            }
        }
    }
  else
    {
      // calculer_terme_g(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta,g);
      G = 0;
      int nb_faces_elem = la_zone.nb_faces_elem();
      IntTrav numfa(nb_faces_elem);
      const IntTab& les_elem_faces = zone_VDF.elem_faces();
      DoubleVect coef(dimension);

      for (int elem=0; elem<nb_elem; elem++)
        {
          for (int i=0; i<nb_faces_elem; i++)
            numfa[i] = les_elem_faces(elem,i);

          coef(0) = 0.5*(Flux_Chaleur_Turb(numfa[0])*porosite_surf(numfa[0])
                         + Flux_Chaleur_Turb(numfa[dimension])*porosite_surf(numfa[dimension]));
          coef(1) = 0.5*(Flux_Chaleur_Turb(numfa[1])*porosite_surf(numfa[1])
                         + Flux_Chaleur_Turb(numfa[dimension+1])*porosite_surf(numfa[dimension+1]));

          if (dimension ==2)
            G[elem] = tab_beta(elem)*(g(0)*coef(0) + g(1)*coef(1));

          else if (dimension == 3)
            {
              coef(2) = 0.5*(Flux_Chaleur_Turb(numfa[2])*porosite_surf(numfa[2])
                             + Flux_Chaleur_Turb(numfa[5])*porosite_surf(numfa[5]));
              G[elem] = tab_beta(elem)*(g(0)*coef(0) + g(1)*coef(1) + g(2)*coef(2));
            }
        }
    }
  for (int elem=0; elem<nb_elem; elem++)
    {

      resu(elem,0) += -2*(uteta_T(elem)+Fluctu_Temperature(elem,1))*volumes(elem)*porosite_vol(elem);

      double A=0;
      if (K_eps_Bas_Re(elem,0)>1.e-10)
        A=-Ca*Fluctu_Temperature(elem,1)*K_eps_Bas_Re(elem,1)/K_eps_Bas_Re(elem,0);
      double B=0;
      if (Fluctu_Temperature(elem,0)>1.e-10)
        B=-Cb*(Fluctu_Temperature(elem,1)*Fluctu_Temperature(elem,1))/Fluctu_Temperature(elem,0);
      double C=0;
      if (Fluctu_Temperature(elem,0)>1.e-10)
        C=-Cc*(Fluctu_Temperature(elem,1)/Fluctu_Temperature(elem,0))*uteta_T(elem);
      double D=0;
      if (K_eps_Bas_Re(elem,0)>1.e-10)
        D=-Cd*(P(elem)+G(elem))*Fluctu_Temperature(elem,1)/K_eps_Bas_Re(elem,0);
      //if (elem==45) {
      //   Cerr << "Fluctuations thermiques:" << finl;
      //   Cerr << "P=" << P(elem) << " G=" << G(elem) << " ut=" << uteta_T(elem) << finl;
      //   Cerr << "A=" << A << " B=" << B << " C=" << C << " D=" << D << finl;
      // }
      resu(elem,1) += (A+B+C+D)*volumes(elem)*porosite_vol(elem);
    }
  // Cerr << "FIN DE AJOUTER SOURCES FLUCTU TEMP resu = " << resu << finl;
  return resu;
}

DoubleTab& Source_Transport_Fluctuation_Temperature_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}




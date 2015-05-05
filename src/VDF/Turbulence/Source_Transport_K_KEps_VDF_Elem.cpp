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
// File:        Source_Transport_K_KEps_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Transport_K_KEps_VDF_Elem.h>
#include <Transport_K_KEps.h>
#include <Convection_Diffusion_Temperature.h>
#include <Modele_turbulence_scal_base.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Champ_Face.h>
#include <Zone_Cl_VDF.h>
#include <Paroi_std_hyd_VDF.h>
#include <DoubleTrav.h>


Implemente_instanciable_sans_constructeur(Source_Transport_K_KEps_VDF_Elem,"Source_Transport_K_KEps_VDF_P0_VDF",Source_base);

Implemente_instanciable_sans_constructeur(Source_Transport_K_KEps_anisotherme_VDF_Elem,"Source_Transport_K_KEps_anisotherme_VDF_P0_VDF",Source_Transport_K_KEps_VDF_Elem);


//// printOn
//

Sortie& Source_Transport_K_KEps_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Sortie& Source_Transport_K_KEps_anisotherme_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Source_Transport_K_KEps_VDF_Elem::readOn(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_K_KEps" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_K_KEps" << finl;
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
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_K_KEps" << finl;
            exit();
          }
        }

      is >> motlu;
    }

  return is ;
}

Entree& Source_Transport_K_KEps_anisotherme_VDF_Elem::readOn(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_K_KEps_anisotherme" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_K_KEps" << finl;
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
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_K_KEps_anisotherme" << finl;
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
//             Source_Transport_K_KEps_VDF_Elem
//
/////////////////////////////////////////////////////////////////////////////

void Source_Transport_K_KEps_VDF_Elem::associer_zones(const Zone_dis& zone_dis,
                                                      const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
}

// remplit les references eq_hydraulique et mon_eq_transport_K_Eps
void Source_Transport_K_KEps_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  eq_hydraulique = pb.equation(0);
  mon_eq_transport_K_Eps = ref_cast(Transport_K_KEps,equation());
}

DoubleTab& Source_Transport_K_KEps_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_VDF& zone_VDF_NS = ref_cast(Zone_VDF,eq_hydraulique->zone_dis().valeur());
  const Zone_Cl_VDF& zone_Cl_VDF_NS = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const DoubleTab& K_eps = mon_eq_transport_K_Eps->inconnue().valeurs();
  const DoubleTab& visco_turb = mon_eq_transport_K_Eps->modele_turbulence().viscosite_turbulente().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_vol = zone_VDF.porosite_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const int nbcouches = mon_eq_transport_K_Eps->get_nbcouches();
  int ndeb,nfin,elem,face_courante,elem_courant;
  double dist;
  double d_visco,y_etoile, critere_switch;
  Loi_2couches_base& loi2couches =ref_cast_non_const(Transport_K_KEps,mon_eq_transport_K_Eps.valeur()).loi2couches();
  int valswitch;
  int icouche;
  const int impr2 =  mon_eq_transport_K_Eps->get_impr();

  const int typeswitch = mon_eq_transport_K_Eps->get_switch();
  if ( typeswitch == 0 )
    valswitch = mon_eq_transport_K_Eps->get_yswitch();
  else
    valswitch = mon_eq_transport_K_Eps->get_nutswitch();

  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  DoubleTrav P(nb_elem_tot);
  DoubleTrav Eps(nb_elem_tot);
  DoubleTrav tab_couches(nb_elem_tot);

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



  //essayer de calculer u+d+ ici pour chaque elt du bord?!

  for (elem=0; elem<nb_elem; elem++)
    {
      Eps[elem] = K_eps(elem,1);
      tab_couches[elem]=0;
    }


  // Boucle sur les bords
  //Cerr << " nb bord = " << zone_VDF_NS.nb_front_Cl() << finl;
  for (int n_bord=0; n_bord<zone_VDF_NS.nb_front_Cl(); n_bord++)
    {

      //pour chaque condition limite on regarde son type
      const Cond_lim& la_cl = zone_Cl_VDF_NS.les_conditions_limites(n_bord);
      //Cerr << " la cl = " << la_cl.valeur() << finl;

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                if ( (elem =face_voisins(num_face,0)) != -1) ;
                else
                  elem = face_voisins(num_face,1);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];
                dist=zone_VDF_NS.distance_normale(num_face);
                //Cerr << "dist = " << dist << finl;

                face_courante = num_face;
                elem_courant = elem;


                for(icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    double kSqRt=1.e-3;
                    if (K_eps(elem_courant,0)>0)
                      kSqRt=sqrt(K_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = visco_turb[elem_courant]/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        tab_couches[elem_courant]=1;
                        double Leps,Lmu,vvSqRt;
                        if (K_eps(elem_courant,0)>0)
                          loi2couches.LepsLmu(K_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        else
                          loi2couches.LepsLmu(1.e-3,d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);

                        if ( Leps!=0.)
                          Eps[elem_courant] = K_eps(elem_courant,0)*kSqRt/Leps;
                        else Eps[elem_courant] = 1.e-3;
                      }
                    else
                      {
                        break;
                      }
                    //Cerr << "face courante  " << face_courante << finl;
                    //Cerr << "face 0  " << elem_faces(elem_courant,0) << finl;
                    if ( elem_faces(elem_courant,0) == face_courante)
                      face_courante = elem_faces(elem_courant,2);
                    else if ( elem_faces(elem_courant,1) == face_courante)
                      face_courante = elem_faces(elem_courant,3);
                    else if ( elem_faces(elem_courant,2) == face_courante)
                      face_courante = elem_faces(elem_courant,0);
                    else if ( elem_faces(elem_courant,3) == face_courante)
                      face_courante = elem_faces(elem_courant,1);
                    //Cerr << "elem courant   " << elem_courant << finl;
                    //Cerr << "nve elem =  " << face_voisins(face_courante,0) << finl;
                    if ( face_voisins(face_courante,0) != elem_courant)
                      elem_courant = face_voisins(face_courante,0);
                    else
                      elem_courant = face_voisins(face_courante,1);
                    //Cerr << "dist = "  << dist << finl;
                    dist+=zone_VDF_NS.distance_normale(face_courante);

                  }
                if ((eq_hydraulique->schema_temps().limpr()) && (impr2 == 1) )
                  {
                    if ( (typeswitch == 0) )        //selon le critere de switch choisit:y* ou nu_t
                      Cout << "Changement de couche a la maille " << icouche <<  " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1) << ") y* = " << critere_switch << finl;
                    else
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1)  << ") nu_t/nu = " << critere_switch << finl;
                  }

              }

          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                if ( (elem =face_voisins(num_face,0)) != -1) ;
                else
                  elem = face_voisins(num_face,1);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];
                dist=zone_VDF_NS.distance_normale(num_face);
                //Cerr << "dist = " << dist << finl;

                face_courante = num_face;
                elem_courant = elem;

                for(icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt=1.e-3;
                    if (K_eps(elem_courant,0)>0)
                      kSqRt=sqrt(K_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = visco_turb[elem_courant]/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        tab_couches[elem_courant]=1;
                        double Leps,Lmu,vvSqRt;
                        if (K_eps(elem_courant,0)>0)
                          loi2couches.LepsLmu(K_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        else
                          loi2couches.LepsLmu(1.e-3,d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( Leps!=0.)
                          Eps[elem_courant] = K_eps(elem_courant,0)*kSqRt/Leps;
                        else Eps[elem_courant] = 1.e-3;
                      }
                    else
                      {
                        break;
                      }

                    //Cerr << "face cournate  " << face_courante << finl;
                    //Cerr << "face 0  " << elem_faces(elem_courant,0) << finl;

                    if ( elem_faces(elem_courant,0) == face_courante)
                      face_courante = elem_faces(elem_courant,3);
                    else if ( elem_faces(elem_courant,1) == face_courante)
                      face_courante = elem_faces(elem_courant,4);
                    else if ( elem_faces(elem_courant,2) == face_courante)
                      face_courante = elem_faces(elem_courant,5);
                    else if ( elem_faces(elem_courant,3) == face_courante)
                      face_courante = elem_faces(elem_courant,0);
                    else if ( elem_faces(elem_courant,4) == face_courante)
                      face_courante = elem_faces(elem_courant,1);
                    else if ( elem_faces(elem_courant,5) == face_courante)
                      face_courante = elem_faces(elem_courant,2);

                    if ( face_voisins(face_courante,0) != elem_courant)
                      elem_courant = face_voisins(face_courante,0);
                    else
                      elem_courant = face_voisins(face_courante,1);
                    dist+=zone_VDF_NS.distance_normale(face_courante);
                  }
                if ((eq_hydraulique->schema_temps().limpr()) && (impr2 == 1) && (elem_courant != -1))
                  {
                    if ( (typeswitch == 0) )        //selon le critere de switch choisit:y* ou nu_t
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1) << ";" << zone_VDF_NS.xp(elem_courant,2) << ") y* = " << critere_switch << finl;
                    else
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1) << ";" << zone_VDF_NS.xp(elem_courant,2) << ") nu_t/nu = " << critere_switch << finl;
                  }
              }
        }
    }

  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zone_Cl_VDF_NS,P,K_eps,vit,vitesse,visco_turb);
    }

  for (elem=0; elem<nb_elem; elem++)
    {
      resu(elem,0) += (P(elem)-Eps(elem))*volumes(elem)*porosite_vol(elem);
      if (K_eps(elem,0) >= 10.e-10)
        resu(elem,1) += (C1*P(elem)- C2*Eps(elem))*volumes(elem)*porosite_vol(elem)
                        *Eps(elem)/K_eps(elem,0);
    }

  /* if ((eq_hydraulique->schema_temps().limpr()) && (impr == 1) )
     {
     Cout << "Zone des 2 couches : " << finl;
     for (elem=0; elem<nb_elem; elem++)
     Cout << "elem = " << elem << " couche = " << tab_couches[elem] << finl;
     }*/
  return resu;
}

DoubleTab& Source_Transport_K_KEps_VDF_Elem::calculer(DoubleTab& resu) const
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
void Source_Transport_K_KEps_anisotherme_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  Source_Transport_K_KEps_VDF_Elem::associer_pb(pb);
  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,pb.equation(1));
  eq_thermique = eqn_th;
  const Fluide_Incompressible& fluide = eq_thermique->fluide();
  beta_t = fluide.beta_t();
  gravite = fluide.gravite();
}

DoubleTab& Source_Transport_K_KEps_anisotherme_VDF_Elem::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zcl_VDF_th = ref_cast(Zone_Cl_VDF,eq_thermique->zone_Cl_dis().valeur());
  const Zone_VDF& zone_VDF_NS = ref_cast(Zone_VDF,eq_hydraulique->zone_dis().valeur());
  const Zone_Cl_VDF& zone_Cl_VDF_NS = ref_cast(Zone_Cl_VDF,eq_hydraulique->zone_Cl_dis().valeur());
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
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
  const int nbcouches = mon_eq_transport_K_Eps->get_nbcouches();
  Loi_2couches_base& loi2couches =ref_cast_non_const(Transport_K_KEps,mon_eq_transport_K_Eps.valeur()).loi2couches();

  int valswitch;
  int icouche;
  const int limpr =  mon_eq_transport_K_Eps->get_impr();

  const int typeswitch = mon_eq_transport_K_Eps->get_switch();
  if ( typeswitch == 0 )
    valswitch = mon_eq_transport_K_Eps->get_yswitch();
  else
    valswitch = mon_eq_transport_K_Eps->get_nutswitch();


  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  DoubleTrav P(nb_elem_tot);
  DoubleTrav G(nb_elem_tot);
  DoubleTrav Eps(nb_elem_tot);
  DoubleTrav tab_couches(nb_elem_tot);
  int ndeb,nfin,elem,face_courante,elem_courant;
  double dist;
  double d_visco,y_etoile, critere_switch;

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


  for (elem=0; elem<nb_elem; elem++)
    {
      Eps[elem] = K_eps(elem,1);
      tab_couches[elem]=0;
    }


  // Boucle sur les bords
  for (int n_bord=0; n_bord<zone_VDF_NS.nb_front_Cl(); n_bord++)
    {

      //pour chaque condition limite on regarde son type
      const Cond_lim& la_cl = zone_Cl_VDF_NS.les_conditions_limites(n_bord);
      //Cerr << " la cl = " << la_cl.valeur() << finl;

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                if ( (elem =face_voisins(num_face,0)) != -1) ;
                else
                  elem = face_voisins(num_face,1);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];
                dist=zone_VDF_NS.distance_normale(num_face);
                //Cerr << "dist = " << dist << finl;

                face_courante = num_face;
                elem_courant = elem;


                for(icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt=1.e-3;
                    if (K_eps(elem_courant,0)>0)
                      kSqRt=sqrt(K_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = visco_turb[elem_courant]/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        tab_couches[elem_courant]=1;
                        double Leps,Lmu,vvSqRt;
                        if (K_eps(elem_courant,0)>0)
                          loi2couches.LepsLmu(K_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        else
                          loi2couches.LepsLmu(1.e-3,d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( Leps!=0.)
                          Eps[elem_courant] = K_eps(elem_courant,0)*kSqRt/Leps;
                        else Eps[elem_courant] = 1.e-3;
                      }
                    else
                      break;
                    //Cerr << "face courante  " << face_courante << finl;
                    //Cerr << "face 0  " << elem_faces(elem_courant,1) << finl;
                    if ( elem_faces(elem_courant,0) == face_courante)
                      face_courante = elem_faces(elem_courant,2);
                    else if ( elem_faces(elem_courant,1) == face_courante)
                      face_courante = elem_faces(elem_courant,3);
                    else if ( elem_faces(elem_courant,2) == face_courante)
                      face_courante = elem_faces(elem_courant,0);
                    else if ( elem_faces(elem_courant,3) == face_courante)
                      face_courante = elem_faces(elem_courant,1);
                    //Cerr << "elem courant   " << elem_courant << finl;
                    //Cerr << "nve elem =  " << face_voisins(face_courante,0) << finl;
                    if ( face_voisins(face_courante,0) != elem_courant)
                      elem_courant = face_voisins(face_courante,0);
                    else
                      elem_courant = face_voisins(face_courante,1);
                    //Cerr << "dist = "  << dist << finl;
                    dist+=zone_VDF_NS.distance_normale(face_courante);

                  }
                if ((eq_hydraulique->schema_temps().limpr()) && (limpr == 1) && (elem_courant != -1))
                  {
                    if ( (typeswitch == 0) )        //selon le critere de switch choisit:y* ou nu_t
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1)  << ") y* = " << critere_switch << finl;
                    else
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1)  << ") nu_t/nu = " << critere_switch << finl;
                  }

              }

          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                if ( (elem =face_voisins(num_face,0)) != -1) ;
                else
                  elem = face_voisins(num_face,1);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];
                dist=zone_VDF_NS.distance_normale(num_face);
                //Cerr << "dist = " << dist << finl;

                face_courante = num_face;
                elem_courant = elem;

                for(icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt=1.e-3;
                    if (K_eps(elem_courant,0)>0)
                      kSqRt=sqrt(K_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    //Cerr << " sqrt(k) = " << kSqRt << finl;
                    //Cerr << " y* = " << y_etoile << finl;
                    if (typeswitch == 0)
                      critere_switch = y_etoile;
                    else
                      critere_switch = visco_turb[elem_courant]/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        tab_couches[elem_courant]=1;
                        double Leps,Lmu,vvSqRt;
                        if (K_eps(elem_courant,0)>0)
                          loi2couches.LepsLmu(K_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        else
                          loi2couches.LepsLmu(1.e-3,d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( Leps!=0.)
                          Eps[elem_courant] = K_eps(elem_courant,0)*kSqRt/Leps;
                        else Eps[elem_courant] = 1.e-3;
                      }
                    else
                      {
                        break;
                      }
                    //Cerr << "face cournate  " << face_courante << finl;
                    //Cerr << "face 0  " << elem_faces(elem_courant,0) << finl;

                    if ( elem_faces(elem_courant,0) == face_courante)
                      face_courante = elem_faces(elem_courant,3);
                    else if ( elem_faces(elem_courant,1) == face_courante)
                      face_courante = elem_faces(elem_courant,4);
                    else if ( elem_faces(elem_courant,2) == face_courante)
                      face_courante = elem_faces(elem_courant,5);
                    else if ( elem_faces(elem_courant,3) == face_courante)
                      face_courante = elem_faces(elem_courant,0);
                    else if ( elem_faces(elem_courant,4) == face_courante)
                      face_courante = elem_faces(elem_courant,1);
                    else if ( elem_faces(elem_courant,5) == face_courante)
                      face_courante = elem_faces(elem_courant,2);

                    if ( face_voisins(face_courante,0) != elem_courant)
                      elem_courant = face_voisins(face_courante,0);
                    else
                      elem_courant = face_voisins(face_courante,1);
                    dist+=zone_VDF_NS.distance_normale(face_courante);
                  }
                if ((eq_hydraulique->schema_temps().limpr()) && (limpr == 1) )
                  {
                    if ( (typeswitch == 0) )        //selon le critere de switch choisit:y* ou nu_t
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1) << ";" << zone_VDF_NS.xp(elem_courant,2) << ") y* = " << critere_switch << finl;
                    else
                      Cout << "Changement de couche a la maille " << icouche << " (" << zone_VDF_NS.xp(elem_courant,0) << ";" << zone_VDF_NS.xp(elem_courant,1) << ";" << zone_VDF_NS.xp(elem_courant,2) << ") nu_t/nu = " << critere_switch << finl;
                  }

              }
        }
    }



  if (axi)
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K_Axi(zone_VDF,vitesse,P,K_eps,visco_turb);
    }
  else
    {
      Champ_Face& vitesse = ref_cast_non_const(Champ_Face,eq_hydraulique->inconnue().valeur());
      calculer_terme_production_K(zone_VDF,zone_Cl_VDF_NS,P,K_eps,vit,vitesse,visco_turb);
    }

  // C'est l'objet de type zone_Cl_dis de l'equation thermique
  // qui est utilise dans le calcul de G

  const DoubleTab& tab_beta = ch_beta.valeurs();
  if (sub_type(Champ_Uniforme,ch_beta.valeur()))
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta(0,0),g);
  else
    calculer_terme_destruction_K(zone_VDF,zcl_VDF_th,G,scalaire,alpha_turb,tab_beta,g);

  for (elem=0; elem<nb_elem; elem++)
    {
      //Cerr << " eps " << " " << K_eps(elem,1) << finl;
      resu(elem,0) += (P(elem)-Eps(elem))*volumes(elem)*porosite_vol(elem);

      if (K_eps(elem,0) >= 1.e-10)
        resu(elem,1) += (C1*P(elem)- C2*Eps(elem))*volumes(elem)*porosite_vol(elem)
                        *Eps(elem)/K_eps(elem,0);

      if ( (G(elem)>0) && (K_eps(elem,0) >= 10.e-8) )
        {
          resu(elem,0) += G(elem)*volumes(elem)*porosite_vol(elem);
          resu(elem,1) += C1*(1-C3)*G(elem)*Eps(elem)*volumes(elem)*porosite_vol(elem)
                          /K_eps(elem,0);
        }
    }


  /*  if ( (eq_hydraulique->schema_temps().limpr()) && (impr == 1))
      {
      Cout << "Zone des 2 couches : " << finl;
      for (elem=0; elem<nb_elem; elem++)
      Cout << "elem = " << elem << " couche = " << tab_couches[elem] << finl;
      }*/
  return resu;
}

DoubleTab& Source_Transport_K_KEps_anisotherme_VDF_Elem::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}

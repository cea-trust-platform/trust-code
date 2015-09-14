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
// File:        Paroi_2couches_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_2couches_VDF.h>
#include <Modele_turbulence_hyd_K_Eps_2_Couches.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Paroi_std_hyd_VDF.h>
#include <Debog.h>
#include <EcrFicPartage.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Paroi_2couches_VDF,"loi_paroi_2_couches_VDF",Paroi_hyd_base_VDF);

//     printOn()
/////

Sortie& Paroi_2couches_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_2couches_VDF::readOn(Entree& s)
{
  Paroi_hyd_base_VDF::readOn(s);
  return s ;
}

void Paroi_2couches_VDF::set_param(Param& param)
{
  Paroi_hyd_base_VDF::set_param(param);
  Paroi_log_QDM::set_param(param);
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_2couches_VDF
//
/////////////////////////////////////////////////////////////////////

int Paroi_2couches_VDF::init_lois_paroi()
{
  dplus_.resize(la_zone_VDF->nb_faces_bord());
  uplus_.resize(la_zone_VDF->nb_faces_bord());
  init_lois_paroi_();

  return init_lois_paroi_hydraulique();
}

// Remplissage de la table

int Paroi_2couches_VDF::init_lois_paroi_hydraulique()
{
  Cmu = mon_modele_turb_hyd->get_Cmu();
  init_lois_paroi_hydraulique_();
  return 1;
}

int Paroi_2couches_VDF::calculer_hyd(DoubleTab& tab_k_eps)
{
  Cerr << "Paroi_2couches_VDF::calculer_hyd(DoubleTab& tab_k_eps) : Erreur on ne devrait pas etre ici!!!" << finl;
  return 1;
}

int Paroi_2couches_VDF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k_eps)
{
  //Cerr << "Dans Paroi_2couches_VDF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k) : Ok ! " << finl;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  double visco=-1.;
  Modele_turbulence_hyd_K_Eps_2_Couches& mod_2couches = ref_cast(Modele_turbulence_hyd_K_Eps_2_Couches,mon_modele_turb_hyd.valeur());
  const int nbcouches = mod_2couches.get_nbcouches();

  Loi_2couches_base& loi2couches = ref_cast(Transport_K_KEps,mod_2couches.eqn_transp_K_Eps()).loi2couches();

  int valswitch;

  const int typeswitch = mod_2couches.get_switch();
  if ( typeswitch == 0 )
    valswitch = mod_2couches.get_yswitch();
  else
    valswitch = mod_2couches.get_nutswitch();

  int l_unif;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;

  // preparer_calcul_hyd(tab);
  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    { 
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    } 
  //    tab_visco+=DMINFLOAT;

  int ndeb,nfin,face_courante,elem_courant;
  int elem,ori;
  double norm_v;
  double dist;
  double u_plus_d_plus,d_visco,y_etoile, critere_switch;
  double val,val1,val2;

  // Boucle sur les bords
  //Cerr << " nb frontieres = " << zone_VDF.nb_front_Cl() << finl;
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
      //      Cerr << "n = " << n_bord << " = " << la_cl.valeur() << finl;
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                ori = orientation(num_face);
                if ( (elem =face_voisins(num_face,0)) != -1)
                  norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                  }
                dist=zone_VDF.distance_normale(num_face);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];
                u_plus_d_plus = norm_v*dist/d_visco;


                face_courante = num_face;
                elem_courant = elem;

                // Loi pour Epsilon et visco_turb
                for(int icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt = sqrt(tab_k_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = tab_nu_t(elem_courant)/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        double vvSqRt,Leps,Lmu;
                        loi2couches.LepsLmu(tab_k_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( (Leps != 0) && (Lmu!=0.) )
                          {
                            tab_k_eps(elem_courant,1) = tab_k_eps(elem_courant,0)*vvSqRt/Leps;
                            tab_nu_t(elem_courant) = vvSqRt*Lmu;
                          }
                        else
                          {
                            tab_k_eps(elem_courant,1) = 0;
                            tab_nu_t(elem_courant) = 0;
                          }
                      }
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
                    dist+=zone_VDF.distance_normale(face_courante);

                  }

                // Calcul de u* et des grandeurs turbulentes:

                double valmin = table_hyd.val(5);
                double valmax = table_hyd.val(40);
                if (u_plus_d_plus <= valmin)
                  tab_u_star_(num_face) = sqrt(norm_v*d_visco/dist);
                else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
                  {
                    double d_plus;
                    calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
                  }
                else if (u_plus_d_plus >= valmax)
                  calculer_u_star_sous_couche_log(norm_v,d_visco,dist,num_face);


                // Calcul de la contrainte tangentielle

                Cisaillement_paroi_(num_face,1-ori) = tab_u_star(num_face)*tab_u_star(num_face)*val;

                // Calcul de u+ d+
                calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;
              }

          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                ori = orientation(num_face);
                if ( (elem = face_voisins(num_face,0)) != -1)
                  norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                  }
                dist = zone_VDF.distance_normale(num_face);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];
                u_plus_d_plus = norm_v*dist/d_visco;

                face_courante = num_face;
                elem_courant = elem;

                // Loi pour Epsilon et visco_turb
                for(int icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt = sqrt(tab_k_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = tab_nu_t(elem_courant)/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        double vvSqRt,Leps,Lmu;
                        loi2couches.LepsLmu(tab_k_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( (Leps != 0) && (Lmu!=0.) )
                          {
                            tab_k_eps(elem_courant,1) = tab_k_eps(elem_courant,0)*vvSqRt/Leps;
                            tab_nu_t(elem_courant) = vvSqRt*Lmu;
                          }
                        else
                          {
                            tab_k_eps(elem_courant,1) = 0;
                            tab_nu_t(elem_courant) = 0;
                          }
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

                    //Cerr << "elem courant   " << elem_courant << finl;
                    //Cerr << "nve elem =  " << face_voisins(face_courante,0) << finl;
                    if ( face_voisins(face_courante,0) != elem_courant)
                      elem_courant = face_voisins(face_courante,0);
                    else
                      elem_courant = face_voisins(face_courante,1);
                    //Cerr << "dist = "  << dist << finl;
                    dist+=zone_VDF.distance_normale(face_courante);

                  }

                // Calcul de u* et des grandeurs turbulentes:

                double valmin = table_hyd.val(5);
                double valmax = table_hyd.val(40);

                if (u_plus_d_plus <= valmin)
                  tab_u_star_(num_face) = sqrt(norm_v*d_visco/dist);
                else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
                  {
                    double d_plus;
                    calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
                  }
                else if (u_plus_d_plus >= valmax)
                  calculer_u_star_sous_couche_log(norm_v,d_visco,dist,num_face);

                // Calcul des deux composantes de la contrainte tangentielle:

                double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                if (ori == 0)
                  {
                    Cisaillement_paroi_(num_face,1) = vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = vit_frot*val2;
                  }
                else if (ori == 1)
                  {
                    Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = vit_frot*val2;
                  }
                else
                  {
                    Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                    Cisaillement_paroi_(num_face,1) = vit_frot*val2;
                  }
                // Calcul de u+ d+
                calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;
              }
        }

      else if (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
        {
          const Dirichlet_paroi_defilante& cl_diri = ref_cast(Dirichlet_paroi_defilante,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          DoubleTab vitesse_imposee_face_bord(le_bord.nb_faces(),dimension);
          for (int face=ndeb; face<nfin; face++)
            for (int k=0; k<dimension; k++)
              vitesse_imposee_face_bord(face-ndeb,k) = cl_diri.val_imp(face-ndeb,k);

          if (dimension == 2 )

            // On calcule au centre des mailles de bord
            // la vitesse parallele a l'axe de la face de bord
            //
            // On calcule la norme de cette vitesse

            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                ori = orientation(num_face);
                int rang = num_face-ndeb;
                if ( (elem = face_voisins(num_face,0)) != -1)
                  norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                     vitesse_imposee_face_bord(rang,1),val);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),val);
                  }

                dist = zone_VDF.distance_normale(num_face);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                u_plus_d_plus = norm_v*dist/d_visco;

                face_courante = num_face;
                elem_courant = elem;

                // Loi pour Epsilon et visco_turb
                for(int icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt = sqrt(tab_k_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = tab_nu_t(elem_courant)/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        double vvSqRt,Leps,Lmu;
                        loi2couches.LepsLmu(tab_k_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( (Leps != 0) && (Lmu!=0.) )
                          {
                            tab_k_eps(elem_courant,1) = tab_k_eps(elem_courant,0)*vvSqRt/Leps;
                            tab_nu_t(elem_courant) = vvSqRt*Lmu;
                          }
                        else
                          {
                            tab_k_eps(elem_courant,1) = 0;
                            tab_nu_t(elem_courant) = 0;
                          }
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
                    dist+=zone_VDF.distance_normale(face_courante);

                  }
                // Calcul de u* et des grandeurs turbulentes:

                //   calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);
                double valmin = table_hyd.val(5);
                double valmax = table_hyd.val(40);

                if (u_plus_d_plus <= valmin)
                  tab_u_star_(num_face) = sqrt(norm_v*d_visco/dist);
                else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
                  {
                    double d_plus;
                    calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
                  }
                else if (u_plus_d_plus >= valmax)
                  calculer_u_star_sous_couche_log(norm_v,d_visco,dist,num_face);

                // Calcul de la contrainte tangentielle a la paroi:

                Cisaillement_paroi_(num_face,1-ori) = tab_u_star(num_face)*tab_u_star(num_face)*val;
                // Calcul de u+ d+
                calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;
              }

          else if (dimension == 3)
            // On calcule au centre des mailles de bord
            // la vitesse dans le plan parallele a celui de la face de bord
            // On calcule la norme de cette vitesse

            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                ori = orientation(num_face);
                int rang = num_face-ndeb;
                if ( (elem = face_voisins(num_face,0)) != -1)
                  norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                     vitesse_imposee_face_bord(rang,1),
                                     vitesse_imposee_face_bord(rang,2),val1,val2);
                else
                  {
                    elem =  face_voisins(num_face,1);
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),
                                       vitesse_imposee_face_bord(rang,2),val1,val2);
                  }
                dist = zone_VDF.distance_normale(num_face);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                u_plus_d_plus = norm_v*dist/d_visco;

                face_courante = num_face;
                elem_courant = elem;

                // Loi pour Epsilon et visco_turb
                for(int icouche=0; (icouche<nbcouches) && (elem_courant != -1); icouche++)
                  {
                    //Cerr << "***********Couche " << icouche << "**************" << finl;
                    double kSqRt = sqrt(tab_k_eps(elem_courant,0));
                    y_etoile = kSqRt*dist/d_visco;
                    if (typeswitch == 0)        //selon le critere de switch choisit:y* ou nu_t
                      critere_switch = y_etoile;
                    else
                      critere_switch = tab_nu_t(elem_courant)/d_visco;
                    if (critere_switch < valswitch)        // si on est en proche paroi
                      {
                        double vvSqRt,Leps,Lmu;
                        loi2couches.LepsLmu(tab_k_eps(elem_courant,0),d_visco,dist,y_etoile,Leps,Lmu,vvSqRt);
                        if ( (Leps != 0) && (Lmu!=0.) )
                          {
                            tab_k_eps(elem_courant,1) = tab_k_eps(elem_courant,0)*vvSqRt/Leps;
                            tab_nu_t(elem_courant) = vvSqRt*Lmu;
                          }
                        else
                          {
                            tab_k_eps(elem_courant,1) = 0;
                            tab_nu_t(elem_courant) = 0;
                          }
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

                    //Cerr << "elem courant   " << elem_courant << finl;
                    //Cerr << "nve elem =  " << face_voisins(face_courante,0) << finl;
                    if ( face_voisins(face_courante,0) != elem_courant)
                      elem_courant = face_voisins(face_courante,0);
                    else
                      elem_courant = face_voisins(face_courante,1);
                    //Cerr << "dist = "  << dist << finl;
                    dist+=zone_VDF.distance_normale(face_courante);

                  }
                // Calcul de u* et des grandeurs turbulentes:

                // calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);
                double valmin = table_hyd.val(5);
                double valmax = table_hyd.val(40);

                if (u_plus_d_plus <= valmin)
                  tab_u_star_(num_face) = sqrt(norm_v*d_visco/dist);
                else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
                  {
                    double d_plus;
                    calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
                  }
                else if (u_plus_d_plus >= valmax)
                  calculer_u_star_sous_couche_log(norm_v,d_visco,dist,num_face);

                // Calcul des deux composantes de la contrainte tangentielle:

                double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                if (ori == 0)
                  {
                    Cisaillement_paroi_(num_face,1) = vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = vit_frot*val2;
                  }
                else if (ori == 1)
                  {
                    Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = vit_frot*val2;
                  }
                else
                  {
                    Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                    Cisaillement_paroi_(num_face,1) = vit_frot*val2;
                  }
                // Calcul de u+ d+
                calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;
              }

        }
    }
  Cisaillement_paroi_.echange_espace_virtuel();
  tab_nu_t.echange_espace_virtuel();
  tab_k_eps.echange_espace_virtuel();

  Debog::verifier("Cisaillement_paroi : ", Cisaillement_paroi_);
  Debog::verifier("tab_nu_t : ", tab_nu_t);
  Debog::verifier("tab_k_eps : ", tab_k_eps);

  return 1;
}

// Calcul de u+ d+
void Paroi_2couches_VDF::calculer_uplus_dplus(DoubleVect& uplus, DoubleVect& dplus, DoubleVect& tab_ustar,
                                              int num_face, double dist, double d_visco, double norm_v)
{
  double ustar=tab_ustar(num_face);
  dplus(num_face)=ustar*dist/d_visco;
  if (ustar != 0)
    uplus(num_face)=norm_v/ustar;
}


int Paroi_2couches_VDF::calculer_u_star_sous_couche_tampon(double& d_plus,double u_plus_d_plus,
                                                           double d_visco,double dist,int face)
{

  // Calcul de d_plus solution de table_hyd(inconnue) = u_plus_d_plus
  // puis calcul de u* dans la sous-couche tampon : u* = nu*d+/dist
  double d_plus_min = 5;
  double d_plus_max = 40;
  double valeur,u_star;
  double epsilon = 1.e-12;
  double gauche = table_hyd.val(d_plus_min);
  double droite = table_hyd.val(d_plus_max);
  double deriv;
  if (gauche == droite)
    d_plus = d_plus_min;
  else
    {
      while(1)
        {
          deriv = (droite-gauche)/(d_plus_max-d_plus_min);
          d_plus = d_plus_min + (u_plus_d_plus - gauche)/deriv;
          valeur = table_hyd.val(d_plus);
          if(dabs(valeur-u_plus_d_plus) < epsilon)
            {
              u_star = (d_visco*d_plus)/dist;
              tab_u_star_(face) = u_star;
              return 1;
            }
          if(valeur>u_plus_d_plus)
            {
              droite=valeur;
              d_plus_max=d_plus;
            }
          else
            {
              gauche=valeur;
              d_plus_min=d_plus;
            }
        }
    }
  return -1;
}

int Paroi_2couches_VDF::calculer_u_star_sous_couche_log(double norm_vit,double d_visco,
                                                        double dist, int face)
{
  // Dans la sous couche inertielle u* est solution d'une equation
  // differentielle resolue de maniere iterative

  const double Xpini = 40.;
  const int itmax  = 25;
  const double seuil = 0.01;
  const double c1 = Kappa*norm_vit;
  const double c2 = log(Erugu*dist/d_visco);  // log = logarithme neperien
  double u_star,u_star1;

  u_star = Xpini*d_visco/dist;

  int iter = 0;
  u_star1 = 1;
  while ((iter++<itmax) && (dabs(u_star1) > seuil))
    {
      u_star1 = (c1-u_star*(log(u_star) + c2))/(c1 + u_star);
      u_star = (1+u_star1)*u_star;
    }
  if (dabs(u_star1) >= seuil) erreur_non_convergence();

  tab_u_star_(face)= u_star;

  return 1;
}

void Paroi_2couches_VDF::imprimer_ustar(Sortie& os) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int ndeb,nfin;
  double upmoy,dpmoy,utaumoy;
  upmoy=0.;
  dpmoy=0.;
  utaumoy=0.;
  int compt=0;

  EcrFicPartage Ustar;
  ouvrir_fichier_partage(Ustar,"Ustar");

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur())) ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) ))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          if(je_suis_maitre())
            {
              Ustar << finl;
              Ustar << "Bord " << le_bord.le_nom() << finl;
              if (dimension == 2)
                {
                  Ustar << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                  Ustar << "\tFace a\t\t\t\t|\t\t\t\t\t\t\t\t\t| TAU=Nu.Grad(Ut) [m2/s2]" << finl;
                  Ustar << "----------------------------------------|-----------------------------------------------------------------------|-----------------------------------------------" << finl;
                  Ustar << "X\t\t| Y\t\t\t| u+\t\t\t| d+\t\t\t| u*\t\t\t| |TAUx|\t\t| |TAUy|" << finl;
                  Ustar << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                }
              if (dimension == 3)
                {
                  Ustar << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                  Ustar << "\tFace a\t\t\t\t\t\t\t|\t\t\t\t\t\t\t\t\t| TAU=Nu.Grad(Ut) [m2/s2]" << finl;
                  Ustar << "----------------------------------------------------------------|-----------------------------------------------------------------------|----------------------------------------------------------------" << finl;
                  Ustar << "X\t\t| Y\t\t\t| Z\t\t\t| u+\t\t\t| d+\t\t\t| u*\t\t\t| |TAUx|\t\t| |TAUy|\t\t| |TAUz|" << finl;
                  Ustar << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|----------------" << finl;
                }
            }
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              double x=zone_VDF.xv(num_face,0);
              double y=zone_VDF.xv(num_face,1);
              if (dimension == 2)
                Ustar << x << "\t| " << y;
              if (dimension == 3)
                {
                  double z=zone_VDF.xv(num_face,2);
                  Ustar << x << "\t| " << y << "\t| " << z;
                }
              Ustar << "\t| " << uplus_(num_face) << "\t| " << dplus_(num_face) << "\t| " << tab_u_star(num_face);
              Ustar << "\t| " << Cisaillement_paroi_(num_face,1) << "\t| " << Cisaillement_paroi_(num_face,0) ;
              if (dimension == 3)
                Ustar << "\t| " << Cisaillement_paroi_(num_face,2) << finl;
              else
                Ustar << finl;

              upmoy +=uplus_(num_face);
              dpmoy +=dplus_(num_face);
              utaumoy +=tab_u_star(num_face);
              compt +=1;
            }
          Ustar.syncfile();
        }
    }
  upmoy = mp_sum(upmoy);
  dpmoy = mp_sum(dpmoy);
  utaumoy = mp_sum(utaumoy);
  compt = mp_sum(compt);
  if (compt && je_suis_maitre())
    {
      Ustar << finl;
      Ustar << "-------------------------------------------------------------" << finl;
      Ustar << "Calcul des valeurs moyennes (en supposant maillage regulier):" << finl;
      Ustar << "<u+>= " << upmoy/compt << " <d+>= " << dpmoy/compt << " <u*>= " << utaumoy/compt << finl;
    }
  if(je_suis_maitre())
    Ustar << finl << finl;
  Ustar.syncfile();
}




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
// File:        ParoiVDF_TBLE_LRM.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////


#include <ParoiVDF_TBLE_LRM.h>
#include <Motcle.h>
#include <Front_VF.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Fluide_Incompressible.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Diffu_k.h>
#include <Paroi_TBLE_scal_VDF.h>
#include <Modele_turbulence_scal_base.h>
#include <Probleme_base.h>
#include <Terme_Boussinesq_VDF_Face.h>
#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd_base.h>

Implemente_instanciable_sans_constructeur(ParoiVDF_TBLE_LRM,"Paroi_TBLE_LRM_VDF",Paroi_hyd_base_VDF);

//     printOn()
/////

Sortie& ParoiVDF_TBLE_LRM::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& ParoiVDF_TBLE_LRM::readOn(Entree& is)
{

  //Cerr << "debut readon";

  Motcle mot_lu;

  // Valeurs par defaut
  nb_pts=-1; // nb de pts dans le maillage fin
  modele_visco = "Diffu_k"; //modele de viscosite turbulente
  nb_comp = 2; //nb de composantes dans le maillage fin
  fac=1.; // facteur de raffinement du maillage fin
  epsilon = 1.e-5; // seuil de resolution dans le maillage fin
  max_it = 1000; // max d'iterations dans le maillage fin
  // k_init=-1; //condition initiale pour k
  avec_boussi=1;//par defaut on met les termes de Boussinesq
  // FIN valeurs par defaut
  nb_post_pts=0;

  Motcles les_mots(13);
  {
    les_mots[0]="N";
    les_mots[1]="modele_visco";
    les_mots[2]="N_comp";
    les_mots[3]="facteur";
    // mot cle 4 libre
    les_mots[5]="epsilon";
    les_mots[6]="max_it";
    les_mots[9]="type_regime";
    les_mots[10]="sans_Boussinesq";
    les_mots[11]="k_init";
    les_mots[12]="sonde_tble";

  }
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");
  Motcle type_reg="vide";
  Motcle nom_classe_mod = "Mod_echelle_";


  is >> mot_lu;
  if(mot_lu == acc_ouverte)
    {
      // on passe l'accolade ouvrante
      is >> mot_lu;
    }
  while(mot_lu != acc_fermee)
    {
      int rang=les_mots.search(mot_lu);
      switch(rang)
        {
        case 0 :
          is >> nb_pts;
          Cerr << "N =" << nb_pts << finl;
          break;
        case 1  :
          is >> modele_visco;
          Cerr << "Le modele de viscosite turbulente dans le maillge fin est "
               << modele_visco  << finl;
          break;
        case 2  :
          is >> nb_comp;
          Cerr << "Nombre de composantes de vitesse tangentielles a la paroi dans le maillage fin: "
               << nb_comp << finl;
          break;

        case 3  :
          is >> fac;
          Cerr << "Raison geometrique du maillage fin : " << fac << finl;
          break;
        case 5  :
          is >> epsilon;
          Cerr << "Seuil de convergence pour les equations de couche limites : " << epsilon << finl;
          break;
        case 6  :
          is >> max_it;
          Cerr << "Le maximum d'iterations pour la resolution des equations TBLE est de : " << max_it << finl;
          break;
        case 9 :
          is >> type_reg;
          nom_classe_mod+=type_reg;
          mod_ech.typer(nom_classe_mod);
          break;
        case 10 :
          avec_boussi=0;
          break;
        case 11 :
          is >> k_init;
          Cerr << "k_init = " << k_init << finl;
          break;
        case 12 :
          is >> nb_post_pts;
          sonde_tble.resize(nb_post_pts, dimension);
          for(int i=0; i<nb_post_pts; i++)
            {
              nom_pts.dimensionner(nb_post_pts);
              is >> nom_pts[i];
              Cerr << "Nom"<<i<<"=" << nom_pts[i] <<finl;
              for(int j=0; j<dimension ; j++)
                {
                  is >> sonde_tble(i,j);
                  Cerr << "sonde_tble( "<< i << "," << j <<" ) =" << sonde_tble(i,j)  << finl;
                }
            }
          break;

        default :
          {
            Cerr << mot_lu << " n'est pas un mot compris par ParoiVDF_TBLE_LRM" << finl;
            Cerr << "Les mots compris sont : " << les_mots << finl;
            exit();
          }
        }
      is >> mot_lu;
    }

  if (type_reg=="vide")
    {
      Cerr << "Le modele d'echelles l_eps, l_mu n'est pas precise !" << finl;
      exit();
    }


  return is;

  //Cerr << "fin readon";

}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe ParoiVDF_TBLE_LRM
//
/////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////
// // Initialisation des tableaux
// ////////////////////////////////////////////////

int ParoiVDF_TBLE_LRM::init_lois_paroi()
{
  Cerr << "debut init_lois_paroi" << finl;

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs();
  //  eq_k_U_W.dimensionner(la_zone_VDF->nb_faces_bord());
  int compteur_faces_paroi = 0;
  DoubleVect corresp(la_zone_VDF->nb_faces_bord());

  // B.M. Nouveau codage a tester: je n'ai pas trouve ou est fait le resize()
  // initial du tableau a nb_faces_bord. Je ne sais pas si le tableau contient
  // deja des valeurs ou pas, s'il faut les conserver ou pas !
  exit();
  Cisaillement_paroi_.resize(0, dimension);
  zone_VDF.creer_tableau_faces_bord(Cisaillement_paroi_);

  int ndeb=0,nfin=0;
  int elem, ori;
  double vmoy = 0.;
  double dist; //distance du premier centre de maille a la paroi


  // SFichier fic_corresp("corresp.dat",ios::app); // impression de la correspondance

  // Boucle sur les bords
  //Cerr << "Boucle sur les bords" << finl;


  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              corresp(compteur_faces_paroi)=num_face;
              //          fic_corresp << compteur_faces_paroi << " " << num_face << finl;
              //          Cerr << compteur_faces_paroi << " " << num_face << finl;
              compteur_faces_paroi++;


            }
        }
    }



  compteur_faces_paroi = la_zone_VDF->nb_faces_bord();
  //  Cerr << "compteur_faces_paroi pour dimensionnement = " << compteur_faces_paroi << finl;
  eq_k_U_W.dimensionner(compteur_faces_paroi); //Dimensionnement du vecteur eq_couch_lim
  corresp.resize(compteur_faces_paroi); //Redimensionnement de corresp


  num_faces_post.resize(nb_post_pts);
  num_faces_post2.resize(nb_post_pts);
  for(int j=0; j<nb_post_pts; j++)
    {
      double d0=1.e9;
      int face=-1;
      int face2=-1;
      compteur_faces_paroi=0;
      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  double d1=0.;
                  for (int d=0; d<dimension; d++)
                    {
                      double x=zone_VDF.xv(num_face,d)-sonde_tble(j,d);
                      d1+=x*x;
                    }

                  if (d1<d0)
                    {
                      d0=d1;
                      face=compteur_faces_paroi;
                      face2=num_face;
                    }
                  compteur_faces_paroi++;
                }
            }
        }
      //face en local
      num_faces_post(j)=face;
      //face en global
      num_faces_post2(j)=face2;
    }


  compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi


  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();



          ////////////
          /// 2D  ////
          ////////////

          if (dimension == 2 )
            {
              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {

                  //ici ori=direction perpendiculaire a la paroi
                  ori = orientation(num_face);

                  if ((elem = face_voisins(num_face,0)) == -1)
                    {
                      elem = face_voisins(num_face,1);
                    }


                  eq_k_U_W[compteur_faces_paroi].associer_milieu(le_fluide);

                  //Distance a la paroi du 1er centre de maille
                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);

                  eq_k_U_W[compteur_faces_paroi].set_y0(0.); //ordonnee de la paroi
                  eq_k_U_W[compteur_faces_paroi].set_yn(dist); //ordonnee du 1er centre de maille


                  eq_k_U_W[compteur_faces_paroi].initialiser(nb_comp+1, nb_pts,
                                                             fac, epsilon, max_it,0); //nbre de pts maillage fin

                  eq_k_U_W[compteur_faces_paroi].set_u_y0(0,0.);//1ere composante de la vitesse a la paroi
                  eq_k_U_W[compteur_faces_paroi].set_u_y0(1,0.); //Composante de k a la paroi



                  eq_k_U_W[compteur_faces_paroi].set_diffu(modele_visco); //modele de viscosite turbulente


                  if (sub_type(Diffu_k,eq_k_U_W[compteur_faces_paroi].get_diffu()))
                    {
                      Diffu_k& diffu = ref_cast_non_const(Diffu_k, eq_k_U_W[compteur_faces_paroi].get_diffu());
                      diffu.associer_modele_echelles(mod_ech.valeur());
                    }

                  vmoy = 0.5*(vit(elem_faces(elem,(ori+1)%4)) + vit(elem_faces(elem,(ori+3)%4)));

                  //vitesse sur le maillage fin a l'instant initial
                  eq_k_U_W[compteur_faces_paroi].set_u_yn(0,vmoy); //1ere composante de la vitesse en yn
                  eq_k_U_W[compteur_faces_paroi].set_Uinit_lin(0, 0., vmoy);

                  vmoy = 0.5*(vit(elem_faces(elem,(ori+1)%4)) + vit(elem_faces(elem,(ori+3)%4)));

                  //vmoy /= 2.;


                  eq_k_U_W[compteur_faces_paroi].set_Uinit_lin(1, 0., k_init); // a ameliorer pour la condition initiale de k


                  compteur_faces_paroi++;

                }//Fin boucle sur les faces de bord

            }



          ////////////
          /// 3D  ////
          ////////////

          if (dimension == 3)
            {

              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {

                  //ici ori=direction perpendiculaire a la paroi
                  ori = orientation(num_face);

                  if ((elem = face_voisins(num_face,0)) == -1)
                    {
                      elem = face_voisins(num_face,1);
                    }

                  eq_k_U_W[compteur_faces_paroi].associer_milieu(le_fluide);

                  //Distance a la paroi du 1er centre de maille
                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);

                  eq_k_U_W[compteur_faces_paroi].set_y0(0.); //ordonnee de la paroi
                  eq_k_U_W[compteur_faces_paroi].set_yn(dist); //ordonnee du 1er centre de maille


                  eq_k_U_W[compteur_faces_paroi].initialiser(nb_comp+1, nb_pts,
                                                             fac, epsilon, max_it,0);


                  eq_k_U_W[compteur_faces_paroi].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi
                  eq_k_U_W[compteur_faces_paroi].set_u_y0(1,0.); //2e composante de la vitesse a la paroi
                  eq_k_U_W[compteur_faces_paroi].set_u_y0(2,0.); //k a la paroi

                  eq_k_U_W[compteur_faces_paroi].set_diffu(modele_visco); //modele de viscosite turbulente

                  if (sub_type(Diffu_k,eq_k_U_W[compteur_faces_paroi].get_diffu()))
                    {
                      Diffu_k& diffu = ref_cast_non_const(Diffu_k, eq_k_U_W[compteur_faces_paroi].get_diffu());
                      diffu.associer_modele_echelles(mod_ech.valeur());
                    }

                  ///a voir :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                  vmoy = 0.5*(vit(elem_faces(elem,(ori+1)%6)) + vit(elem_faces(elem,(ori+4)%6)));

                  eq_k_U_W[compteur_faces_paroi].set_u_yn(0,vmoy); //1ere composante de la vitesse en yn


                  //vitesse sur le maillage fin a l'instant initial
                  eq_k_U_W[compteur_faces_paroi].set_Uinit_lin(0, 0., vmoy);

                  vmoy = 0.5*(vit(elem_faces(elem,(ori+2)%6)) + vit(elem_faces(elem,(ori+5)%6)));

                  //vmoy /= 2.;

                  //                 eq_k_U_W[compteur_faces_paroi].set_u_yn(1,1.); //2e composante de la vitesse en yn
                  eq_k_U_W[compteur_faces_paroi].set_u_yn(1,vmoy); //2e composante de la vitesse en yn
                  eq_k_U_W[compteur_faces_paroi].set_Uinit_lin(1, 0., vmoy);


                  //pour initialiser le calcul, on prend le u_tau theorique

                  eq_k_U_W[compteur_faces_paroi].set_Uinit_lin(2, 0., k_init); // a ameliorer pour la condition initiale de k

                  compteur_faces_paroi++;

                }//Fin boucle sur les faces de bord
            }// Fin if dim 3
        }//Fin if Paroi_fixe
    }//Fin boucle sur les bords parietaux

  Cerr << "fin init_lois_paroi" << finl;

  return 1;
}


/////////////////////////////////////////////////
//Calcul du cisaillement a la paroi
////////////////////////////////////////////////

int ParoiVDF_TBLE_LRM::calculer_hyd(DoubleTab& nu_t, DoubleTab& tab_k)
{
  Cerr <<"TBLE LRM non code en LES" << finl;
  exit();
  return 0;
}

int ParoiVDF_TBLE_LRM::calculer_hyd(DoubleTab& tab_k_eps)
{


  if (Process::nproc()>1)
    {
      Cerr << "ParoiVDF_TBLE_LRM::calculer_hyd n'est pas parallelise." << finl;
      exit();
    }
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  Mod_echelle_LRM_base& le_mod_ech= mod_ech.valeur();

  const DoubleTab& nu_t = mon_modele_turb_hyd->viscosite_turbulente().valeur().valeurs();



  double visco=-1;
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


  int ndeb=0,nfin=0;
  int  ori;
  //   int elem_gauche0, elem_gauche1, elem_droite0, elem_droite1;
  //   int face_gauche0, face_gauche1, face_droite0, face_droite1;
  int elem;
  //  int compteur=0;

  int signe;
  int itmax=0;

  double vmoy = 0., ts0 =0., ts1=0.,gradient_de_pression0=0., gradient_de_pression1 = 0.;
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs(); //vitesse
  DoubleVect grad_vit_elemx;
  DoubleVect grad_vit_elem_moyx;
  DoubleVect grad_vit_elemz;
  DoubleVect grad_vit_elem_moyz;
  DoubleVect grad_T;
  DoubleVect grad_T_moy;
  DoubleVect source_T_k;
  DoubleVect source_T_U;


  double eps0;
  double eps;
  double l_eps;
  double vv_bar;
  double u=0;

  grad_vit_elemx.resize(nb_pts);
  grad_vit_elem_moyx.resize(nb_pts);
  grad_vit_elemz.resize(nb_pts);
  grad_vit_elem_moyz.resize(nb_pts);
  grad_T.resize(nb_pts);
  grad_T_moy.resize(nb_pts);
  source_T_k.resize(nb_pts);
  source_T_U.resize(nb_pts);

  const Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std, eqn_hydr);

  DoubleTab grad_p(vit);
  const DoubleTab& p = eqnNS.pression().valeurs();
  const Operateur_Grad& gradient = eqnNS.operateur_gradient();
  gradient.calculer(p, grad_p);  // Calcul du gradient de pression

  DoubleTab termes_sources;
  termes_sources.resize(zone_VDF.nb_faces());
  eqn_hydr.sources().calculer(termes_sources); //les termes sources

  const double& tps = eqnNS.schema_temps().temps_courant();
  const double& dt = eqnNS.schema_temps().pas_de_temps();
  const double& dt_min = eqnNS.schema_temps().pas_temps_min();


  //SFichier fic_v("v.dat",ios::app); // impression de la vitesse normale pour test

  int compteur_faces_paroi = 0;
  DoubleVect corresp(la_zone_VDF->nb_faces_bord());


  // Boucle sur les bords
  //Cerr << "Boucle sur les bords" << finl;

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              corresp(compteur_faces_paroi)=num_face;
              compteur_faces_paroi++;
            }
        }
    }

  corresp.resize(compteur_faces_paroi); //Redimensionnement de corresp
  compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi


  // Boucle sur les bords

  //Cerr << "Boucle sur les bords " << finl;
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois


      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();



          ///////////////////////
          //   Si probleme 2D  //
          ///////////////////////


          if(dimension == 2)
            {
              //eq_k_U_W.dimensionner(2*la_zone_VDF->nb_faces_bord());

              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  //ici ori=direction perpendiculaire a la paroi
                  ori = orientation(num_face);
                  //Cerr << "ori = " << ori << finl;



                  //compteur++;
                  //Selection de l'element associe a une face parietale
                  //et initialisation de l'entier signe

                  if ((elem = face_voisins(num_face,0)) == -1)
                    {
                      elem = face_voisins(num_face,1);
                      signe = 1;
                    }
                  else signe = -1 ;


                  if(dt<dt_min)
                    eq_k_U_W[compteur_faces_paroi].set_dt(1.e12);
                  else
                    eq_k_U_W[compteur_faces_paroi].set_dt(dt);



                  // 1er couple de faces perpendiculaires a la paroi
                  int face1 = elem_faces(elem,(ori+1));
                  int face2 = elem_faces(elem,(ori+3)%4);



                  ////////////////////////////////////////////////////////////
                  ////// couple  de faces perpendiculaires a la paroi /////
                  ////////////////////////////////////////////////////////////

                  vmoy = 0.5*(vit(face1) + vit(face2));

                  //vitesse sur le maillage fin a l'instant initial
                  //eq_k_U_W[compteur_faces_paroi].set_Uinit_lin(0, 0., vmoy);

                  ts0 = 0.5*(termes_sources(face1)/volumes_entrelaces(face1)
                             + termes_sources(face2)/volumes_entrelaces(face2));

                  eq_k_U_W[compteur_faces_paroi].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi
                  eq_k_U_W[compteur_faces_paroi].set_u_yn(0,vmoy); //vitesse en yn
                  //  eq_k_U_W[compteur_faces_paroi].set_u_yn(0,1.); //vitesse en yn



                  //*** Gradient de pression du 1er couple de faces
                  // perpendiculaires a la paroi ***

                  //gradient de pression (constant dans le maillage fin)
                  gradient_de_pression0 = 0.5*(grad_p(face1)+grad_p(face2));

                  //Calcul du gradient  On prend U au temps n
                  for(int i=0 ; i<nb_pts ; i++)
                    {
                      grad_vit_elemx(i)=(eq_k_U_W[compteur_faces_paroi].get_Unp1(0,i+1)-eq_k_U_W[compteur_faces_paroi].get_Unp1(0,i))/(eq_k_U_W[compteur_faces_paroi].get_y(i+1)-eq_k_U_W[compteur_faces_paroi].get_y(i));
                    }

                  //Cacul du gradient a  l'ordonnee y(i)
                  for(int i=1 ; i<nb_pts ; i++)
                    {
                      grad_vit_elem_moyx(i)=0.5*(grad_vit_elemx(i)+grad_vit_elemx(i-1));
                    }

                  //gradient en 0
                  grad_vit_elem_moyx(0)=(eq_k_U_W[compteur_faces_paroi].get_Unp1(0,1)-eq_k_U_W[compteur_faces_paroi].get_Unp1(0,0))/(eq_k_U_W[compteur_faces_paroi].get_y(1)-eq_k_U_W[compteur_faces_paroi].get_y(0));



                  double d_visco;

                  if (l_unif==1)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  // On calcul vv_bar, l_eps, y_nu, y_star avec k au temps n
                  // a la paroi eps est proportionelle a 1

                  Diffu_totale_base& diffu = eq_k_U_W[compteur_faces_paroi].get_diffu();
                  /*

                  double dist;
                  if (axi)
                  dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                  dist = zone_VDF.dist_norm_bord(num_face);
                  */



                  // Definition de la face servant a recuperer le k de la deuxieme maille
                  int face3 = elem_faces(elem,(ori+2)%4);
                  int face4 = elem_faces(elem,ori);

                  int n2=face3;
                  if (face3==num_face)
                    {
                      n2=face4;
                    }

                  int elem1=face_voisins(n2,0);
                  if (elem1 == elem)
                    elem1=face_voisins(n2,1);



                  double dy = zone_VDF.dist_norm(n2);


                  // recuperartion de l'element correspondant a la deuxieme maille

                  double kcl;

                  double a1=d_visco+0.5*(nu_t(elem)+nu_t(elem1));
                  double a2=0.5*(diffu.calculer_a_local(nb_pts)+diffu.calculer_a_local(nb_pts-1));


                  double b1=a1/dy;
                  double b2=a2/(eq_k_U_W[compteur_faces_paroi].get_y(nb_pts)-eq_k_U_W[compteur_faces_paroi].get_y(nb_pts-1));


                  kcl=(b1*tab_k_eps(elem1,0)+b2*eq_k_U_W[compteur_faces_paroi].get_Unp1(1,nb_pts-1))/(b1+b2);


                  //equation en k ;

                  eq_k_U_W[compteur_faces_paroi].set_u_y0(1,0.); //k a la paroi
                  eq_k_U_W[compteur_faces_paroi].set_u_yn(1,kcl);//k a yn


                  //         double Re, Fmu;

                  //initialisation de epsilon pour maillage 2D tel que nut(tble)=nut(2D)


                  /* if(tab_k_eps(elem,0)<0)
                     {
                     //                Cerr << "Warning dans TBLE_LRM : k3D <0 !!! k = " << tab_k_eps(elem,0)  << finl;
                     tab_k_eps(elem,0)=1.e-8;
                     tab_k_eps(elem,1)=0.09*tab_k_eps(elem,0)*tab_k_eps(elem,0)/(diffu.calculer_a_local(nb_pts)-d_visco);
                     } */

                  if(kcl<0)
                    {
                      //                Cerr << "Warning dans TBLE_LRM : k3D <0 !!! k = " << tab_k_eps(elem,0)  << finl;
                      kcl=1.e-8;
                      tab_k_eps(elem,0)=kcl;
                      tab_k_eps(elem,1)=0.09*tab_k_eps(elem,0)*tab_k_eps(elem,0)/(diffu.calculer_a_local(nb_pts)-d_visco);
                    }

                  else
                    {
                      tab_k_eps(elem,0)=kcl;
                      tab_k_eps(elem,1)=   0.09*tab_k_eps(elem,0)*tab_k_eps(elem,0)/(diffu.calculer_a_local(nb_pts)-d_visco);
                    }


                  double T0=0;
                  double beta_t=0.;

                  source_T_k=0.;
                  source_T_U=0.;

                  if (avec_boussi==1)
                    {
                      int boussi_ok=0;
                      const Sources& les_sources=eqnNS.sources();
                      int nb_sources=les_sources.size();

                      for (int j=0; j<nb_sources; j++)
                        {
                          const Source_base& ts = les_sources(j).valeur();
                          if (sub_type(Terme_Boussinesq_base,ts))
                            {
                              const Terme_Boussinesq_base& terme_boussi = ref_cast(Terme_Boussinesq_base,ts);
                              T0 = terme_boussi.Scalaire0(0);
                              boussi_ok=1;
                            }
                        }

                      if (boussi_ok==1)
                        {
                          const Equation_base& eqn_th  = mon_modele_turb_hyd->equation().probleme().equation(1);
                          const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqn_th.get_modele(TURBULENCE).valeur());
                          const Turbulence_paroi_scal_base& loi = le_mod_turb_th.loi_paroi().valeur();
                          Paroi_TBLE_scal_VDF& loi_tble_T = ref_cast_non_const(Paroi_TBLE_scal_VDF,loi);

                          const Champ_Don& ch_beta_t = le_fluide.beta_t();
                          const DoubleTab& tab_champ_beta_t = ch_beta_t->valeurs();
                          if (sub_type(Champ_Uniforme,ch_beta_t.valeur()))
                            {
                              beta_t = max(tab_champ_beta_t(0,0),DMINFLOAT);
                            }
                          else
                            {
                              Cerr << " On ne sait pas traiter un beta_t non uniforme dans Paroi TBLE_LRM !!!"<< finl;
                              exit();
                            }



                          for(int i=0 ; i<nb_pts ; i++)
                            {
                              grad_T(i)=(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i+1)-loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i))/(eq_k_U_W[compteur_faces_paroi].get_y(i+1)-eq_k_U_W[compteur_faces_paroi].get_y(i));
                            }

                          for(int i=1 ; i<nb_pts ; i++)
                            {
                              grad_T_moy(i)=0.5*(grad_T(i)+grad_T(i-1));
                            }


                          grad_T_moy(0)=(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,1)-loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,0))/(eq_k_U_W[compteur_faces_paroi].get_y(1)-eq_k_U_W[compteur_faces_paroi].get_y(0));

                          const Champ_Don_base& ch_gravite=le_fluide.gravite();
                          const DoubleVect& gravite = ch_gravite.valeurs();

                          if (!sub_type(Champ_Uniforme,ch_gravite))
                            {
                              Cerr << " On ne sait pas traiter la gravite non uniforme dans Paroi TBLE_LRM !!!"<< finl;
                              exit();
                            }



                          //calcul de la gravite

                          double norm_n=zone_VDF.face_surfaces(num_face);
                          double gn=0.;

                          for (int idim=0; idim<dimension; idim++)
                            {
                              gn+=gravite(idim)*zone_VDF.face_normales(num_face,idim)/norm_n;
                            }

                          DoubleVect gt_vect(dimension);

                          for (int idim=0; idim<dimension; idim++)
                            {
                              gt_vect(idim) = gravite(idim)-gn*zone_VDF.face_normales(num_face,idim)/norm_n;
                            }

                          double g_t=gt_vect(1-ori);



                          for(int i=0 ; i<nb_pts; i++)
                            {
                              source_T_k(i) = g_t*beta_t*((diffu.calculer_a_local(i)-d_visco)/(0.9))*grad_T_moy(i);
                              source_T_U(i) = g_t*beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);
                            }

                        }
                    }

                  //terme source a la paroi

                  eps0=2*d_visco*eq_k_U_W[compteur_faces_paroi].get_Unp1(1,1)/(eq_k_U_W[compteur_faces_paroi].get_y(1)*eq_k_U_W[compteur_faces_paroi].get_y(1));
                  eq_k_U_W[compteur_faces_paroi].set_F(1,0,-eps0 + source_T_k(0));
                  eq_k_U_W[compteur_faces_paroi].set_F(0,0, -gradient_de_pression0 + ts0  - source_T_U(0));

                  for(int i=1 ; i<nb_pts; i++)
                    {

                      //echelles de vitesse et de longueur (u ne sert pas actuellement, il est mis a 0)
                      double k=eq_k_U_W[compteur_faces_paroi].get_Unp1(1,i);
                      double y=eq_k_U_W[compteur_faces_paroi].get_y(i);

                      vv_bar=le_mod_ech.calculer_vv_bar(y, k, u, d_visco);
                      l_eps=le_mod_ech.calculer_l_eps(y, k, u, d_visco);


                      //terme source en i pour k
                      eps=eq_k_U_W[compteur_faces_paroi].get_Unp1(1,i)*sqrt(vv_bar)/l_eps;

                      eq_k_U_W[compteur_faces_paroi].set_F(1,i,(diffu.calculer_a_local(i)-d_visco)
                                                           *grad_vit_elem_moyx(i)*grad_vit_elem_moyx(i)-eps+source_T_k(i));

                      //terme source pour U
                      eq_k_U_W[compteur_faces_paroi].set_F(0,i, -gradient_de_pression0 + ts0  - source_T_U(i) );

                    }

                  //On resoud les equations aux limites simplifiees
                  //pendant un pas de temps du maillage grossier
                  // On obtient U, V, W au temps n+1

                  eq_k_U_W[compteur_faces_paroi].aller_au_temps(tps);


                  //Message lorsque k est negatif. Modification de la valeur a 1e-9
                  for(int i=0 ; i<nb_pts ; i++)
                    {
                      if (eq_k_U_W[compteur_faces_paroi].get_Unp1(1,i)<0.)
                        {
                          Cerr << "Warning dans TBLE_LRM : k <0 !!! k = " <<  eq_k_U_W[compteur_faces_paroi].get_Unp1(1,i) << finl;
                          eq_k_U_W[compteur_faces_paroi].set_Unp1(1,i,1.e-9);
                        }
                    }

                  if(itmax<eq_k_U_W[compteur_faces_paroi].get_it())
                    {
                      itmax = eq_k_U_W[compteur_faces_paroi].get_it();
                      if(itmax>max_it)
                        Cerr << "WARNING : TOO MANY ITERATIONS ARE NEEDED IN THE TBLE MESH"
                             << finl;
                    }


                  ///////////////////////////////////////////////
                  ///// Determination des deux composantes //////
                  /////     du cisaillement a la paroi     //////
                  ///////////////////////////////////////////////

                  if (ori == 0)
                    {
                      //Cerr << "ori=0" << finl;
                      Cisaillement_paroi_(num_face,0) = 0.;
                      Cisaillement_paroi_(num_face,1) =  eq_k_U_W[compteur_faces_paroi].get_cis(0)*signe;
                    }
                  else if (ori == 1)
                    {

                      Cisaillement_paroi_(num_face,0) = eq_k_U_W[compteur_faces_paroi].get_cis(0)*signe;
                      Cisaillement_paroi_(num_face,1) = 0.;
                    }

                  tab_u_star_(num_face) =pow(eq_k_U_W[compteur_faces_paroi].get_cis(0)*eq_k_U_W[compteur_faces_paroi].get_cis(0),0.25);

                  compteur_faces_paroi++;


                }//fin boucle sur les faces de bords parietaux


            }//fin if(dim2)


          ///////////////////////
          //   Si probleme 3D  //
          ///////////////////////

          else if(dimension == 3)
            {
              //eq_k_U_W.dimensionner(2*la_zone_VDF->nb_faces_bord());

              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  //ici ori=direction perpendiculaire a la paroi
                  ori = orientation(num_face);
                  //Cerr << "ori = " << ori << finl;

                  //compteur++;
                  //Selection de l'element associe a une face parietale
                  //et initialisation de l'entier signe

                  if ((elem = face_voisins(num_face,0)) == -1)
                    {
                      elem = face_voisins(num_face,1);
                      signe = 1;
                    }
                  else signe = -1 ;


                  if(dt<dt_min)
                    eq_k_U_W[compteur_faces_paroi].set_dt(1.e12);
                  else
                    eq_k_U_W[compteur_faces_paroi].set_dt(dt);


                  // 1er couple de faces perpendiculaires a la paroi
                  int face1 = elem_faces(elem,(ori+1));
                  int face2 = elem_faces(elem,(ori+4)%6);
                  // 2e couple de faces perpendiculaires a la paroi
                  int face3 = elem_faces(elem,(ori+2));
                  int face4 = elem_faces(elem,(ori+5)%6);






                  ////////////////////////////////////////////////////////////
                  ////// 1er couple  de faces perpendiculaires a la paroi /////
                  ////////////////////////////////////////////////////////////

                  vmoy = 0.5*(vit(face1) + vit(face2));

                  //vitesse sur le maillage fin a l'instant initial


                  ts0 = 0.5*(termes_sources(face1)/volumes_entrelaces(face1)
                             + termes_sources(face2)/volumes_entrelaces(face2));

                  eq_k_U_W[compteur_faces_paroi].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi

                  eq_k_U_W[compteur_faces_paroi].set_u_yn(0,vmoy); //vitesse en yn

                  //*** Gradient de pression du 1er couple de faces
                  // perpendiculaires a la paroi ***

                  //gradient de pression (constant dans le maillage fin)
                  gradient_de_pression0 = 0.5*(grad_p(face1)+grad_p(face2));

                  //eq_k_U_W[compteur_faces_paroi].set_F(0, gradient_de_pression0 - ts0);


                  //dpzbis += (gradient_de_pression_bis);

                  ////////////////////////////////////////////////////////////
                  ////// 2e couple  de faces perpendiculaires a la paroi /////
                  ////////////////////////////////////////////////////////////

                  vmoy = 0.5*(vit(face3) + vit(face4));

                  ts1 = 0.5*(termes_sources(face3)/volumes_entrelaces(face3)
                             + termes_sources(face4)/volumes_entrelaces(face4));

                  eq_k_U_W[compteur_faces_paroi].set_u_y0(1,0.); //2e composante de la vitesse a la paroi

                  eq_k_U_W[compteur_faces_paroi].set_u_yn(1,vmoy); //vitesse en yn

                  //*** Gradient de pression du 2e couple de faces
                  // perpendiculaires a la paroi ***


                  //gradient de pression (constant dans le maillage fin)

                  gradient_de_pression1 = 0.5*(grad_p(face3)+grad_p(face4));


                  //eq_k_U_W[compteur_faces_paroi].set_F(1, gradient_de_pression1 - ts1);
                  //Calcul du gradient  On prend U au temps n

                  //equation en k ;


                  double d_visco;

                  if (l_unif==1)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];


                  Diffu_totale_base& diffu = eq_k_U_W[compteur_faces_paroi].get_diffu();


                  for(int i=0 ; i<nb_pts; i++)
                    {

                      grad_vit_elemx(i)=(eq_k_U_W[compteur_faces_paroi].get_Unp1(0,i+1)-eq_k_U_W[compteur_faces_paroi].get_Unp1(0,i))/(eq_k_U_W[compteur_faces_paroi].get_y(i+1)-eq_k_U_W[compteur_faces_paroi].get_y(i));
                      grad_vit_elemz(i)=(eq_k_U_W[compteur_faces_paroi].get_Unp1(1,i+1)-eq_k_U_W[compteur_faces_paroi].get_Unp1(1,i))/(eq_k_U_W[compteur_faces_paroi].get_y(i+1)-eq_k_U_W[compteur_faces_paroi].get_y(i));

                    }

                  //gradient en 0
                  grad_vit_elem_moyx(0)=(eq_k_U_W[compteur_faces_paroi].get_Unp1(0,1)-eq_k_U_W[compteur_faces_paroi].get_Unp1(0,0))/(eq_k_U_W[compteur_faces_paroi].get_y(1)-eq_k_U_W[compteur_faces_paroi].get_y(0));
                  grad_vit_elem_moyz(0)=(eq_k_U_W[compteur_faces_paroi].get_Unp1(1,1)-eq_k_U_W[compteur_faces_paroi].get_Unp1(1,0))/(eq_k_U_W[compteur_faces_paroi].get_y(1)-eq_k_U_W[compteur_faces_paroi].get_y(0));

                  //Cacul du gradient a  l'ordonnee y(i)
                  for(int i=1 ; i<nb_pts ; i++)
                    {

                      grad_vit_elem_moyx(i)=0.5*(grad_vit_elemx(i)+grad_vit_elemx(i-1));
                      grad_vit_elem_moyz(i)=0.5*(grad_vit_elemz(i)+grad_vit_elemz(i-1));

                    }

                  // On calcul vv_bar, l_eps, y_nu, y_star avec k au temps n
                  /*
                       double dist;

                       //Distance a la paroi du 1er centre de maille
                       if (axi)
                       dist = zone_VDF.dist_norm_bord_axi(num_face);
                       else
                       dist = zone_VDF.dist_norm_bord(num_face);
                  */


                  // Definition de la face servant a recuperer le k de la deuxieme maille
                  int face5 = elem_faces(elem,(ori+3)%6);
                  int face6 = elem_faces(elem,(ori));



                  int n2=face5;
                  if (face5==num_face)
                    {
                      n2=face6;
                    }
                  int elem1=face_voisins(n2,0);
                  if (elem1 == elem)
                    elem1=face_voisins(n2,1);


                  double dy = zone_VDF.dist_norm(n2);
                  // recuperartion de l'element correspondant a la deuxieme maille

                  double kcl;
                  double a1=d_visco+0.5*(nu_t(elem)+nu_t(elem1));
                  double a2=0.5*(diffu.calculer_a_local(nb_pts)+diffu.calculer_a_local(nb_pts-1));

                  double b1=a1/dy;
                  double b2=a2/(eq_k_U_W[compteur_faces_paroi].get_y(nb_pts)-eq_k_U_W[compteur_faces_paroi].get_y(nb_pts-1));



                  kcl=(b1*tab_k_eps(elem1,0)+b2*eq_k_U_W[compteur_faces_paroi].get_Unp1(2,nb_pts-1))/(b1+b2);


                  //initialisation de epsilon pour maillage 3D tel que nut(tble)=nut(3D)

                  if(tab_k_eps(elem,0)<0)
                    {
                      //Cerr << "Warning dans TBLE_LRM : k3D <0 !!! k = " << tab_k_eps(elem,0)  << finl;
                      tab_k_eps(elem,0)=1.e-8;
                      tab_k_eps(elem,1)=0.09*tab_k_eps(elem,0)*tab_k_eps(elem,0)/(diffu.calculer_a_local(nb_pts)-d_visco);
                    }

                  else
                    {
                      tab_k_eps(elem,0)=kcl;
                      tab_k_eps(elem,1)=0.09*tab_k_eps(elem,0)*tab_k_eps(elem,0)/(diffu.calculer_a_local(nb_pts)-d_visco);
                    }

                  double T0=0;
                  double beta_t=0.;

                  source_T_k=0.;
                  source_T_U=0.;

                  if (avec_boussi==1)
                    {
                      int boussi_ok=0;
                      const Sources& les_sources=eqnNS.sources();
                      int nb_sources=les_sources.size();

                      for (int j=0; j<nb_sources; j++)
                        {
                          const Source_base& ts = les_sources(j).valeur();
                          if (sub_type(Terme_Boussinesq_base,ts))
                            {
                              const Terme_Boussinesq_base& terme_boussi = ref_cast(Terme_Boussinesq_base,ts);
                              T0 = terme_boussi.Scalaire0(0);
                              boussi_ok=1;
                            }
                        }

                      if (boussi_ok==1)
                        {
                          const Equation_base& eqn_th = mon_modele_turb_hyd->equation().probleme().equation(1);
                          const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqn_th.get_modele(TURBULENCE).valeur());
                          const Turbulence_paroi_scal_base& loi = le_mod_turb_th.loi_paroi().valeur();
                          Paroi_TBLE_scal_VDF& loi_tble_T = ref_cast_non_const(Paroi_TBLE_scal_VDF,loi);

                          const Champ_Don& ch_beta_t = le_fluide.beta_t();
                          const DoubleTab& tab_champ_beta_t = ch_beta_t->valeurs();
                          if (sub_type(Champ_Uniforme,ch_beta_t.valeur()))
                            {
                              beta_t = max(tab_champ_beta_t(0,0),DMINFLOAT);
                            }
                          else
                            {
                              Cerr << " On ne sait pas traiter un beta_t non uniforme dans Paroi TBLE_LRM !!!"<< finl;
                              exit();
                            }



                          for(int i=0 ; i<nb_pts ; i++)
                            {
                              grad_T(i)=(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i+1)-loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i))/(eq_k_U_W[compteur_faces_paroi].get_y(i+1)-eq_k_U_W[compteur_faces_paroi].get_y(i));
                            }

                          for(int i=1 ; i<nb_pts ; i++)
                            {
                              grad_T_moy(i)=0.5*(grad_T(i)+grad_T(i-1));
                            }


                          grad_T_moy(0)=(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,1)-loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,0))/(eq_k_U_W[compteur_faces_paroi].get_y(1)-eq_k_U_W[compteur_faces_paroi].get_y(0));

                          const Champ_Don_base& ch_gravite=le_fluide.gravite();
                          const DoubleVect& gravite = ch_gravite.valeurs();

                          if (!sub_type(Champ_Uniforme,ch_gravite))
                            {
                              Cerr << " On ne sait pas traiter la gravite non uniforme dans Paroi TBLE_LRM !!!"<< finl;
                              exit();
                            }



                          //calcul de la gravite


                          double norm_n=zone_VDF.face_surfaces(num_face);
                          double gn=0.;

                          for (int idim=0; idim<dimension; idim++)
                            {
                              gn+=gravite(idim)*zone_VDF.face_normales(num_face,idim)/norm_n;
                            }

                          DoubleVect gt_vect(dimension);

                          for (int idim=0; idim<dimension; idim++)
                            {
                              gt_vect(idim) = gravite(idim)-gn*zone_VDF.face_normales(num_face,idim)/norm_n;
                            }

                          double g_t=gt_vect(1-ori);

                          for(int i=1 ; i<nb_pts; i++)
                            {
                              source_T_k(i) =g_t*beta_t*((diffu.calculer_a_local(i)-d_visco)/(0.9))*grad_T_moy(i);
                              source_T_U(i) =g_t*beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);

                            }

                        }
                    }

                  eps0=2*d_visco*eq_k_U_W[compteur_faces_paroi].get_Unp1(2,1)/(eq_k_U_W[compteur_faces_paroi].get_y(1)*eq_k_U_W[compteur_faces_paroi].get_y(1));
                  eq_k_U_W[compteur_faces_paroi].set_F(2,0,-eps0 + source_T_k(0));
                  eq_k_U_W[compteur_faces_paroi].set_F(0,0, -gradient_de_pression0 + ts0 - source_T_U(0));

                  eq_k_U_W[compteur_faces_paroi].set_F(1,0, -gradient_de_pression1 + ts1 - source_T_U(0) );


                  for(int i=1 ; i<nb_pts ; i++)
                    {

                      //echelles de vitesse et de longueur (u ne sert pas actuellement, il est mis a 0)
                      double k=eq_k_U_W[compteur_faces_paroi].get_Unp1(2,i);
                      double y=eq_k_U_W[compteur_faces_paroi].get_y(i);


                      vv_bar=le_mod_ech.calculer_vv_bar(y, k, u, d_visco);
                      l_eps=le_mod_ech.calculer_l_eps(y, k, u, d_visco);

                      //terme source a la paroi


                      eps=eq_k_U_W[compteur_faces_paroi].get_Unp1(2,i)*sqrt(vv_bar)/l_eps;

                      eq_k_U_W[compteur_faces_paroi].set_F(2,i,(diffu.calculer_a_local(i)-d_visco)*(grad_vit_elem_moyx(i)*grad_vit_elem_moyx(i)+grad_vit_elem_moyz(i)*grad_vit_elem_moyz(i))-eps
                                                           + source_T_k(i));

                      // pour U et W

                      eq_k_U_W[compteur_faces_paroi].set_F(0,i, -gradient_de_pression0 + ts0  - source_T_U(i));
                      eq_k_U_W[compteur_faces_paroi].set_F(1,i, -gradient_de_pression1 + ts1  - source_T_U(i) );

                    }



                  //On resoud les equations aux limites simplifiees
                  //pendant un pas de temps du maillage grossier

                  eq_k_U_W[compteur_faces_paroi].set_u_y0(2,0.); //k a la paroi
                  eq_k_U_W[compteur_faces_paroi].set_u_yn(2,kcl);//k a yn

                  eq_k_U_W[compteur_faces_paroi].aller_au_temps(tps);

                  //Message lorsque k est negatif. Modification de la valeur a 1e-9
                  for(int i=0 ; i<nb_pts ; i++)
                    {
                      if (eq_k_U_W[compteur_faces_paroi].get_Unp1(2,i)<0.)
                        {
                          //        Cerr << "Warning dans TBLE_LRM : k <0 !!! k = " <<  eq_k_U_W[compteur_faces_paroi].get_Unp1(2,i) << finl;
                          eq_k_U_W[compteur_faces_paroi].set_Unp1(2,i,1.e-9);
                        }
                    }

                  if(itmax<eq_k_U_W[compteur_faces_paroi].get_it())
                    {
                      itmax = eq_k_U_W[compteur_faces_paroi].get_it();
                      if(itmax>max_it)
                        Cerr << "WARNING : TOO MANY ITERATIONS ARE NEEDED IN THE TBLE MESH"
                             << finl;
                    }


                  ///////////////////////////////////////////////
                  ///// Determination des deux composantes //////
                  /////     du cisaillement a la paroi     //////
                  ///////////////////////////////////////////////

                  if (ori == 0)
                    {
                      //Cerr << "ori=0" << finl;
                      Cisaillement_paroi_(num_face,0) = 0.;
                      Cisaillement_paroi_(num_face,1) =  eq_k_U_W[compteur_faces_paroi].get_cis(0)*signe;
                      Cisaillement_paroi_(num_face,2) =  eq_k_U_W[compteur_faces_paroi].get_cis(1)*signe;
                    }
                  else if (ori == 1)
                    {

                      Cisaillement_paroi_(num_face,0) = eq_k_U_W[compteur_faces_paroi].get_cis(1)*signe;
                      Cisaillement_paroi_(num_face,1) = 0.;
                      Cisaillement_paroi_(num_face,2) = eq_k_U_W[compteur_faces_paroi].get_cis(0)*signe;

                    }
                  else
                    {
                      //Cerr << "ori=2" << finl;
                      Cisaillement_paroi_(num_face,0) = eq_k_U_W[compteur_faces_paroi].get_cis(0)*signe;
                      Cisaillement_paroi_(num_face,1) = eq_k_U_W[compteur_faces_paroi].get_cis(1)*signe;
                      Cisaillement_paroi_(num_face,2) = 0.;
                    }

                  tab_u_star_(num_face) = pow((eq_k_U_W[compteur_faces_paroi].get_cis(0)*eq_k_U_W[compteur_faces_paroi].get_cis(0)
                                               +eq_k_U_W[compteur_faces_paroi].get_cis(1)*eq_k_U_W[compteur_faces_paroi].get_cis(1)),0.25);



                  compteur_faces_paroi++;


                  for(int j=0; j<nb_post_pts; j++)
                    {
                      Nom tmp2;
                      tmp2="nut_post_";
                      tmp2+=nom_pts[j];
                      tmp2+=".dat";

                      SFichier fic_nut(tmp2); //creation fichier de post-traitement des points TBLE

                      Diffu_totale_base& diffu1 = eq_k_U_W[num_faces_post(j)].get_diffu();
                      for(int i=0 ; i<nb_pts+1 ; i++)
                        {
                          fic_nut << eq_k_U_W[num_faces_post(j)].get_y(i)<< " " << diffu1.calculer_a_local(i)-d_visco << finl;
                        }
                      fic_nut << finl;

                    }

                }//fin boucle sur les faces de bords parietaux



            }//fin if(dim=3)
        }//fin if(sub_type(dirichlet))

    }//fin boucle sur les bords

  SFichier fic("iter.dat",ios::app); // ouverture du fichier iter.dat
  fic << tps << " " << itmax << finl;




  Cisaillement_paroi_.echange_espace_virtuel();


  return 1;
}

void ParoiVDF_TBLE_LRM::imprimer_ustar(Sortie& os) const
{
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const double& tps = eqn_hydr.inconnue().temps();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();


  //  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  //  int ndeb,nfin;
  //double upmoy,dpmoy,utaumoy;
  //upmoy=0.;dpmoy=0.;utaumoy=0.;
  //int compt=0;

  for(int j=0; j<nb_post_pts; j++)
    {
      Nom tmp;
      tmp="tble_post_";
      tmp+=nom_pts[j];
      tmp+=".dat";

      SFichier fic_post(tmp /*,ios::app*/);

      int ori=orientation(num_faces_post2(j));

      if (dimension == 2)
        {
          double x=zone_VDF.xv(num_faces_post2(j),0);
          double y=zone_VDF.xv(num_faces_post2(j),1);
          fic_post << "#" <<  "x= " << x << " " << "y= " << y << finl;
        }

      if(dimension == 3)
        {
          double x=zone_VDF.xv(num_faces_post2(j),0);
          double y=zone_VDF.xv(num_faces_post2(j),1);
          double z=zone_VDF.xv(num_faces_post2(j),2);
          fic_post << "#" << "x= " << x << " " << "y= " << y << " " << "z= " << z << finl;
        }


      fic_post << "#"<< tps << " " <<  "u*= " << tab_u_star_(num_faces_post2(j))  << finl;


      if(dimension == 2)
        {

          for(int i=0; i<nb_pts+1; i++)
            {
              fic_post << eq_k_U_W[num_faces_post(j)].get_y(i) << " " <<  eq_k_U_W[num_faces_post(j)].get_Unp1(0,i) <<" " <<
                       eq_k_U_W[num_faces_post(j)].get_Unp1(1,i) << finl;
            }
        }


      if(dimension == 3)
        {
          if(ori == 0)
            {
              for(int i=0; i<nb_pts+1; i++)
                {
                  fic_post <<  eq_k_U_W[num_faces_post(j)].get_y(i) << " " << eq_k_U_W[num_faces_post(j)].get_Unp1(0,i) <<
                           " " << eq_k_U_W[num_faces_post(j)].get_Unp1(1,i) << " " << eq_k_U_W[num_faces_post(j)].get_Unp1(2,i) <<  finl;
                }
            }
          else
            {
              for(int i=0; i<nb_pts+1; i++)
                {
                  fic_post <<  eq_k_U_W[num_faces_post(j)].get_y(i) << " " <<
                           eq_k_U_W[num_faces_post(j)].get_Unp1(1,i) <<
                           " " << eq_k_U_W[num_faces_post(j)].get_Unp1(0,i) << " " << eq_k_U_W[num_faces_post(j)].get_Unp1(2,i) <<  finl;
                }
            }
        }


    }

}













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
// File:        ParoiVEF_TBLE.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////


#include <ParoiVEF_TBLE.h>
#include <Paroi_std_hyd_VEF.h>
#include <Champ_Q1NC.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Fluide_Incompressible.h>
#include <EFichier.h>
#include <Diffu_lm.h>
#include <Schema_Temps.h>
#include <time.h>
#include <Modele_turbulence_scal_base.h>
#include <Probleme_base.h>
#include <ParoiVEF_TBLE_scal.h>
#include <Terme_Boussinesq_VEFPreP1B_Face.h>
#include <Terme_Source_Qdm_lambdaup_VEF_Face.h>
#include <SFichier.h>
#include <Mod_turb_hyd_base.h>
#include <Navier_Stokes_std.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(ParoiVEF_TBLE,"Paroi_TBLE_VEF",Paroi_hyd_base_VEF);

ParoiVEF_TBLE::ParoiVEF_TBLE()
{
  alpha=1.;
}

//     printOn()
Sortie& ParoiVEF_TBLE::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

//// readOn

Entree& ParoiVEF_TBLE::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" wall law"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void ParoiVEF_TBLE::set_param(Param& param)
{
  Paroi_hyd_base_VEF::set_param(param);
  Paroi_TBLE_QDM::set_param(param);
  Paroi_log_QDM::set_param(param);
  param.ajouter("alpha",&alpha);
  param.ajouter_condition("(value_of_alpha_ge_0)_and_(value_of_alpha_le_1)","The following condition must be satisfied : 0 <= alpha <= 1");
  //Ajout pour la lecture non standard
  //Sinon probleme dans Paroi_TBLE_QDM pour (this) qui est annonce const
  param.ajouter_non_std("nu_t_dyn",(this));
  param.ajouter_non_std("tps_start_stat_nu_t_dyn",(this));
  param.ajouter_non_std("tps_nu_t_dyn",(this));
  param.ajouter_non_std("sonde_tble",(this));
  param.ajouter_non_std("stationnaire",(this));
  param.ajouter_non_std("mu",(this));
  param.ajouter_non_std("lambda",(this));
  param.ajouter_non_std("sans_source_boussinesq",(this));
}

int ParoiVEF_TBLE::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Paroi_TBLE_QDM::lire_motcle_non_standard(mot,is);
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe ParoiVEF_TBLE
//
/////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////
// // Initialisation des tableaux
// ////////////////////////////////////////////////

int ParoiVEF_TBLE::init_lois_paroi()
{
  if(je_suis_maitre())
    {
      Cerr << "ParoiVEF_TBLE::init_lois_paroi()" << finl;
    }

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const int& nfac = zone.nb_faces_elem();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs();

  // OC: Je ne comprends pas pourquoi dans les autres LP VEF on resize a nb_faces_tot le cisaillement et le tab_u_star
  // De plus aucune loi de paroi VEF n appelle la methode init_lois_paroi_ de la classe mere
  // qui devrait elle se charger de dimensionner ce tableau
  // Pour l'instant, ici, on appele bien l'init de la classe mere qui resize bien a nb_faces_bord.

  Paroi_hyd_base_VEF::init_lois_paroi_();

  Paroi_TBLE_QDM::init_lois_paroi(la_zone_VEF, la_zone_Cl_VEF.valeur());

  int elem;

  int compteur_faces_paroi = 0;

  double surf2;
  double dist; //distance du centre de la maille a la paroi

  IntVect num(nfac);
  DoubleVect n(dimension),t1(dimension),t2(dimension); // vecteurs orthonomes du repere local associe a la face paroi
  DoubleVect v_tang(nb_comp);

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces();

          //Boucle sur les faces des bords parietaux
          for (int ind_face=0; ind_face<size; ind_face++)
            {
              Eq_couch_lim& equation_vitesse = eq_vit[compteur_faces_paroi];
              Diffu_totale_hyd_base& diffu_hyd = ref_cast_non_const(Diffu_totale_hyd_base, equation_vitesse.get_diffu()); //modele de viscosite turbulente
              diffu_hyd.setKappa(Kappa);

              int num_face = le_bord.num_face(ind_face);
              elem = face_voisins(num_face,0);

              surf2=0.;

              for(int i=0; i<dimension; i++)
                {
                  surf2 += face_normale(num_face,i)*face_normale(num_face,i);
                }

              if (dimension == 2)
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,2);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                  n[0] = face_normale(num_face,0)/sqrt(surf2);
                  n[1] = face_normale(num_face,1)/sqrt(surf2);

                  t1[0] = -n[1];
                  t1[1] =  n[0];

                  dist = distance_2D(num_face,elem,zone_VEF)*3./2.;

                  v_tang[0] = ((vit(num[0],0)+vit(num[1],0))*t1[0]
                               +(vit(num[0],1)+vit(num[1],1))*t1[1])/2.;
                }
              else
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);
                  num[2]=elem_faces(elem,2);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,3);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                  else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                  n[0]  = face_normale(num_face,0)/sqrt(surf2);
                  n[1]  = face_normale(num_face,1)/sqrt(surf2);
                  n[2]  = face_normale(num_face,2)/sqrt(surf2);

                  if( est_egal(n[0],0.) && est_egal(n[1],0.) )
                    {
                      t1[0] = 0.;
                      t1[1] = 1.;
                      t1[2] = 0.;;
                    }
                  else
                    {
                      t1[0] = -n[1]/sqrt(n[0]*n[0]+n[1]*n[1]);
                      t1[1] =  n[0]/sqrt(n[0]*n[0]+n[1]*n[1]);
                      t1[2] =  0.;
                    }

                  t2[0] =  n[1]*t1[2] - n[2]*t1[1];
                  t2[1] =  n[2]*t1[0] - n[0]*t1[2];
                  t2[2] =  n[0]*t1[1] - n[1]*t1[0];

                  dist = distance_3D(num_face,elem,zone_VEF)*4./3.;

                  v_tang[0] = ((vit(num[0],0)+vit(num[1],0)+vit(num[2],0))*t1[0]
                               +(vit(num[0],1)+vit(num[1],1)+vit(num[2],1))*t1[1]
                               +(vit(num[0],2)+vit(num[1],2)+vit(num[2],2))*t1[2])/3.;

                  v_tang[1] = ((vit(num[0],0)+vit(num[1],0)+vit(num[2],0))*t2[0]
                               +(vit(num[0],1)+vit(num[1],1)+vit(num[2],1))*t2[1]
                               +(vit(num[0],2)+vit(num[1],2)+vit(num[2],2))*t2[2])/3.;
                }


              equation_vitesse.set_y0(0.); //ordonnee de la paroi
              equation_vitesse.set_yn(dist); //ordonnee du 1er centre de maille

              equation_vitesse.initialiser(nb_comp, nb_pts, fac, epsilon, max_it, nu_t_dyn); //nbre de pts maillage fin



              for (int i=0; i<nb_comp; i++)
                {
                  equation_vitesse.set_u_y0(i,0.);          // vitesse a la paroi
                  equation_vitesse.set_u_yn(i,v_tang[i]); // vitesse en yn

                  //vitesse sur le maillage fin a l'instant initial

                  if (reprise_ok==0)
                    equation_vitesse.set_Uinit_lin(i, 0., v_tang[i]);
                  else
                    {
                      for (int itble=0; itble<nb_pts+1; itble++)
                        equation_vitesse.set_Uinit(i,itble,valeurs_reprises(compteur_faces_paroi, i, itble)) ;
                    }
                }
              compteur_faces_paroi++;

            }//Fin boucle sur num_face
        }//Fin if Paroi_fixe
    }//Fin boucle sur les bords parietaux

  f_tang_moy.resize(compteur_faces_paroi, nb_comp);
  f_tang_moy=0.;

  // Si on n'a pas interdit la presence du terme de Boussi dans TBLE (mot cle "sans_source_boussinesq",
  // Alors on verifie la presence ou non d'un terme source de Boussinesq dans le pb hydraulique :
  if (source_boussinesq==1)
    {
      source_boussinesq=0;
      const Sources& les_sources=eqn_hydr.sources();
      int nb_sources=les_sources.size();

      for (int j=0; j<nb_sources; j++)
        {
          const Source_base& ts = les_sources(j).valeur();
          if (sub_type(Terme_Boussinesq_base,ts))
            {
              const Terme_Boussinesq_base& terme_boussi = ref_cast(Terme_Boussinesq_base,ts);
              T0 = terme_boussi.Scalaire0(0);
              source_boussinesq=1;
              const Champ_Don& ch_beta_t = le_fluide.beta_t();
              const DoubleTab& tab_champ_beta_t = ch_beta_t->valeurs();
              if (sub_type(Champ_Uniforme,ch_beta_t.valeur()))
                {
                  beta_t = max(tab_champ_beta_t(0,0),DMINFLOAT);
                }
              else
                {
                  Cerr << " On ne sait pas traiter un beta_t non uniforme dans TBLE !!!"<< finl;
                  exit();
                }

              break;
            }
        }
    }
  Cerr << "fin init_lois_paroi()" << finl;
  return 1;
}


/////////////////////////////////////////////////
//Calcul du cisaillement a la paroi
////////////////////////////////////////////////

int ParoiVEF_TBLE::calculer_hyd(DoubleTab& nu_t, DoubleTab& tab_k)
{
  return calculer_hyd(nu_t,0,tab_k);
}

int ParoiVEF_TBLE::calculer_hyd(DoubleTab& tab_k_eps)
{

  DoubleTab bidon(0);
  // bidon ne servira pas
  return calculer_hyd(tab_k_eps,1,bidon);

}

int ParoiVEF_TBLE::calculer_hyd(DoubleTab& tab1,int isKeps,DoubleTab& tab2)
{
  // Certains modeles de turbulence appelle cette methode avant le debut du calcul
  // Ca coince ici alors car le tble_scal n est pas initialise.
  // pas propre mais on laisse pour l'instant :
  // si isKeps=1, on est dans le cas k-eps
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const int& nfac = zone.nb_faces_elem();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();

  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());

  if (source_boussinesq==1)
    {
      const Champ_Don_base& ch_gravite=le_fluide.gravite();

      if (!sub_type(Champ_Uniforme,ch_gravite))
        {
          Cerr << " On ne sait pas traiter la gravite non uniforme dans TBLE !!!"<< finl;
          exit();
        }
    }

  if (isKeps==1) // on est en K-eps
    {
      // on prend la methode par defaut
      int methode=-1;
      int is_champ_Q1NC=sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur());
      remplir_face_keps_imposee( flag_face_keps_imposee_, methode, face_keps_imposee_, zone_VEF,la_zone_Cl_VEF,!is_champ_Q1NC);
    }

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
  //tab_visco+=DMINFLOAT;

  int elem;

  int itmax=0,itmax_loc;

  const DoubleTab& vit = eqn_hydr.inconnue().valeurs(); //vitesse

  const Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std, eqn_hydr);

  // Calcul du gradient de pression
  DoubleTab grad_p(vit);
  const DoubleTab& p = eqnNS.pression().valeurs();
  const Operateur_Grad& gradient = eqnNS.operateur_gradient();
  gradient.calculer(p, grad_p);

  const DoubleTab& visco_turb = mon_modele_turb_hyd->viscosite_turbulente().valeurs();


  DoubleTab termes_sources(vit);
  termes_sources=0;
  // On calcule les termes sources, sauf celui de lambda_uprime et de Boussinesq (TBLE recalcule par lui meme ce terme s'il est demande)
  /*   const Sources& les_sources=eqn_hydr.sources();
       int nb_sources=les_sources.size();
       for (int j=0; j<nb_sources; j++)
       {
       const Source_base& ts = les_sources(j).valeur();
       if (!sub_type(Terme_Boussinesq_scalaire_VEFPreP1B_Face,ts))
       {
       ts.ajouter(termes_sources);
       }
       }
  */
  //  termes_sources.resize(zone_VEF.nb_faces(),dimension);
  //  eqn_hydr.sources().calculer(termes_sources); //les termes sources : commente : Pour ne pas prendre en compte lambda.uprime dans un premier temps



  const double& tps = eqnNS.schema_temps().temps_courant();
  const double& dt = eqnNS.schema_temps().pas_de_temps();
  const double& dt_min = eqnNS.schema_temps().pas_temps_min();

  int compteur_faces_paroi = 0;

  double surf2;
  double dist; //distance du centre de la maille a la paroi

  IntVect num(nfac);
  DoubleVect n(dimension), t1(dimension), t2(dimension); // vecteurs orthonomes du repere local associe a la face paroi
  DoubleVect F(dimension);
  DoubleVect v_tang(nb_comp),F_tang(nb_comp);
  DoubleTab ts_boussi(nb_comp, nb_pts+1);
  ts_boussi=0.;

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces();

          //Boucle sur les faces des bords parietaux
          for (int ind_face=0; ind_face<size; ind_face++)
            {
              Eq_couch_lim& equation_vitesse = eq_vit[compteur_faces_paroi];
              int num_face = le_bord.num_face(ind_face);
              elem = face_voisins(num_face,0);

              surf2=0.;

              for(int i=0; i<dimension; i++)
                {
                  surf2 += face_normale(num_face,i)*face_normale(num_face,i);
                }

              if (dimension == 2)
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,2);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                  n[0] = face_normale(num_face,0)/sqrt(surf2);
                  n[1] = face_normale(num_face,1)/sqrt(surf2);

                  t1[0] = -n[1];
                  t1[1] =  n[0];

                  dist = distance_2D(num_face,elem,zone_VEF)*3./2.;

                  v_tang[0] = ((vit(num[0],0)+vit(num[1],0))*t1[0]
                               +(vit(num[0],1)+vit(num[1],1))*t1[1])/2.;

                  // Terme source de Boussinesq si demande
                  if (source_boussinesq==1)
                    {
                      ts_boussi = 0.;
                      const Champ_Don_base& ch_gravite=le_fluide.gravite();
                      const DoubleVect& gravite = ch_gravite.valeurs();
                      const Equation_base& eqn_th = mon_modele_turb_hyd->equation().probleme().equation(1);
                      const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqn_th.get_modele(TURBULENCE).valeur());
                      const Turbulence_paroi_scal_base& loi = le_mod_turb_th.loi_paroi().valeur();
                      ParoiVEF_TBLE_scal& loi_tble_T = ref_cast_non_const(ParoiVEF_TBLE_scal,loi);
                      if (loi_tble_T.est_initialise())
                        {

                          double gt = 0.;
                          for (int idim=0; idim<dimension; idim++)
                            gt += gravite(idim)*t1[idim];
                          for(int i=0; i<nb_pts+1; i++)
                            ts_boussi(0, i) = -gt*beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);
                        }
                    }


                  F[0] =  ((termes_sources(num[0],0)-grad_p(num[0],0))/volumes_entrelaces(num[0])
                           +(termes_sources(num[1],0)-grad_p(num[1],0))/volumes_entrelaces(num[1]))/2. ;

                  F[1] =  ((termes_sources(num[0],1)-grad_p(num[0],1))/volumes_entrelaces(num[0])
                           +(termes_sources(num[1],1)-grad_p(num[1],1))/volumes_entrelaces(num[1]))/2. ;

                  F_tang[0]  = F[0]*t1[0] + F[1]*t1[1] ; // projection de F suivant l'axe tangentiel_1
                  f_tang_moy(compteur_faces_paroi,0) = alpha*F_tang[0] +(1-alpha)*f_tang_moy(compteur_faces_paroi,0);
                }
              else
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);
                  num[2]=elem_faces(elem,2);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,3);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                  else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                  n[0]  = face_normale(num_face,0)/sqrt(surf2);
                  n[1]  = face_normale(num_face,1)/sqrt(surf2);
                  n[2]  = face_normale(num_face,2)/sqrt(surf2);


                  if( est_egal(n[0],0.) && est_egal(n[1],0.) )
                    {
                      t1[0] = 0.;
                      t1[1] = 1;
                      t1[2] = 0;
                    }
                  else
                    {
                      t1[0] = -n[1]/sqrt(n[0]*n[0]+n[1]*n[1]);
                      t1[1] =  n[0]/sqrt(n[0]*n[0]+n[1]*n[1]);
                      t1[2] =  0.;
                    }

                  t2[0] =  n[1]*t1[2] - n[2]*t1[1];
                  t2[1] =  n[2]*t1[0] - n[0]*t1[2];
                  t2[2] =  n[0]*t1[1] - n[1]*t1[0];

                  dist = distance_3D(num_face,elem,zone_VEF)*4./3.;

                  v_tang[0] = ((vit(num[0],0)+vit(num[1],0)+vit(num[2],0))*t1[0]
                               +(vit(num[0],1)+vit(num[1],1)+vit(num[2],1))*t1[1]
                               +(vit(num[0],2)+vit(num[1],2)+vit(num[2],2))*t1[2])/3.;

                  v_tang[1] = ((vit(num[0],0)+vit(num[1],0)+vit(num[2],0))*t2[0]
                               +(vit(num[0],1)+vit(num[1],1)+vit(num[2],1))*t2[1]
                               +(vit(num[0],2)+vit(num[1],2)+vit(num[2],2))*t2[2])/3.;

                  F[0] =  ((termes_sources(num[0],0)-grad_p(num[0],0))/volumes_entrelaces(num[0])
                           +(termes_sources(num[1],0)-grad_p(num[1],0))/volumes_entrelaces(num[1])
                           +(termes_sources(num[2],0)-grad_p(num[2],0))/volumes_entrelaces(num[2]))/3.;

                  F[1] =  ((termes_sources(num[0],1)-grad_p(num[0],1))/volumes_entrelaces(num[0])
                           +(termes_sources(num[1],1)-grad_p(num[1],1))/volumes_entrelaces(num[1])
                           +(termes_sources(num[2],1)-grad_p(num[2],1))/volumes_entrelaces(num[2]))/3.;

                  F[2] =  ((termes_sources(num[0],2)-grad_p(num[0],2))/volumes_entrelaces(num[0])
                           +(termes_sources(num[1],2)-grad_p(num[1],2))/volumes_entrelaces(num[1])
                           +(termes_sources(num[2],2)-grad_p(num[2],2))/volumes_entrelaces(num[2]))/3.;


                  F_tang[0]  = F[0]*t1[0] + F[1]*t1[1] + F[2]*t1[2] ; // projection de F suivant l'axe tangentiel_1
                  F_tang[1] =  F[0]*t2[0] + F[1]*t2[1] + F[2]*t2[2] ; // projection de F suivant l'axe tangentiel_2
                  f_tang_moy(compteur_faces_paroi,0) = alpha*F_tang[0] +(1-alpha)*f_tang_moy(compteur_faces_paroi,0);
                  f_tang_moy(compteur_faces_paroi,1) = alpha*F_tang[1] +(1-alpha)*f_tang_moy(compteur_faces_paroi,1);

                  // Terme source de Boussinesq si demande
                  if (source_boussinesq==1)
                    {
                      ts_boussi = 0.;
                      const Champ_Don_base& ch_gravite=le_fluide.gravite();
                      const DoubleVect& gravite = ch_gravite.valeurs();
                      const Equation_base& eqn_th = mon_modele_turb_hyd->equation().probleme().equation(1);
                      const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqn_th.get_modele(TURBULENCE).valeur());
                      const Turbulence_paroi_scal_base& loi = le_mod_turb_th.loi_paroi().valeur();
                      ParoiVEF_TBLE_scal& loi_tble_T = ref_cast_non_const(ParoiVEF_TBLE_scal,loi);
                      if (loi_tble_T.est_initialise())
                        {

                          double gt1 = 0.;
                          double gt2 = 0.;
                          for (int idim=0; idim<dimension; idim++)
                            {
                              gt1 += gravite(idim)*t1[idim];
                              gt2 += gravite(idim)*t2[idim];
                            }

                          for(int i=0; i<nb_pts+1; i++)
                            {
                              double tmp = beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);
                              ts_boussi(0, i) = -gt1*tmp;
                              ts_boussi(1, i) = -gt2*tmp;
                            }
                          /*Cout << "gt1 " << gt1 << finl;
                            Cout << "gt2 " << gt2 << finl;
                            Cout << "T0 " << T0 << finl;
                            Cout << "beta_t " << beta_t << finl;*/
                        }
                    }
                }


              if(tps<=dt_min)
                {
                  visco_turb_moy(elem)=visco_turb(elem);
                }
              else if(tps>tps_start_stat_nu_t_dyn)
                {
                  visco_turb_moy(elem) = (dt/tps)*visco_turb(elem)+(1-(dt/tps))*visco_turb_moy(elem);
                }


              // au premier appel, dt ne vaut pas dt_min mais 0. car un appel a mettre_a_jour dans le preparer_calcul des modeles de turb
              // precede l'initialisation du schema en temps.
              if(dt<dt_min) equation_vitesse.set_dt(1.e12);
              else            equation_vitesse.set_dt(dt);


              for (int i=0; i<nb_comp; i++)
                {
                  equation_vitesse.set_u_y0(i,0.);          // vitesse a la paroi
                  equation_vitesse.set_u_yn(i,v_tang[i]); // vitesse en yn

                  //equation_vitesse.set_F(i, F_tang[i]);
                  for(int ipts=0 ; ipts<nb_pts+1; ipts++)
                    equation_vitesse.set_F(i, ipts, f_tang_moy(compteur_faces_paroi,i)+ts_boussi(i,ipts));
                }


              /////////////////////////////////////////////////////
              // On resout les equations aux limites simplifiees //
              //   pendant un pas de temps du maillage grossier  //
              /////////////////////////////////////////////////////


              if (statio==0)
                equation_vitesse.aller_au_temps(tps);
              else
                equation_vitesse.aller_jusqu_a_convergence(max_it_statio, eps_statio);


              itmax_loc = equation_vitesse.get_it();

              if(itmax_loc>max_it) Cerr << "WARNING : TOO MANY ITERATIONS ARE NEEDED IN THE TBLE MESH" << finl;

              if(itmax_loc>itmax) itmax=itmax_loc;


              ///////////////////////////////////////////////
              /////   Determination des  composantes   //////
              /////     du cisaillement a la paroi     //////
              ///////////////////////////////////////////////


              if (dimension == 2)
                {
                  double C0 = equation_vitesse.get_cis(0);

                  double Cx = t1[0]*C0  ;
                  double Cy = t1[1]*C0  ;

                  Cisaillement_paroi_(num_face,0) =  Cx;
                  Cisaillement_paroi_(num_face,1) =  Cy;

                  tab_u_star_(num_face) = pow(Cx*Cx+Cy*Cy,0.25);
                }
              else
                {
                  double C0 = equation_vitesse.get_cis(0);
                  double C1 = equation_vitesse.get_cis(1);

                  double Cx = t1[0]*C0 + t2[0]*C1 ;
                  double Cy = t1[1]*C0 + t2[1]*C1 ;
                  double Cz = t1[2]*C0 + t2[2]*C1 ;

                  Cisaillement_paroi_(num_face,0) =  Cx;
                  Cisaillement_paroi_(num_face,1) =  Cy;
                  Cisaillement_paroi_(num_face,2) =  Cz;

                  tab_u_star_(num_face) = pow(Cx*Cx+Cy*Cy+Cz*Cz,0.25);
                }


              //////////////////////////////////////////////////
              ///// Mise a jour des grandeurs turbulentes //////
              /////           nu_t - k - eps              //////
              //////////////////////////////////////////////////


              if (isKeps==0) // on est en LES
                {
                  if((nu_t_dyn==0)||(tps<tps_nu_t_dyn))
                    {
                      tab1(elem)=equation_vitesse.get_nu_t_yn();
                    }
                  else
                    equation_vitesse.set_nu_t_yn(visco_turb_moy(elem));
                }
              else if (isKeps==1) // on est en K-eps
                {
                  double d_visco;

                  if (l_unif==1)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  double y_plus = dist*tab_u_star_(num_face)/d_visco;

                  traitement_keps(tab1, num_face, face_voisins, elem_faces, nfac, dist, y_plus, d_visco, tab_u_star_(num_face));
                }

              compteur_faces_paroi++;

            }//Fin boucle sur num_face
        }//Fin if Paroi_fixe
    }//Fin boucle sur les bords parietaux


  if (Process::je_suis_maitre())
    {
      SFichier fic("iter.dat",ios::app); // ouverture du fichier iter.dat
      fic << tps << " " << itmax << finl;
    }

  if(oui_stats==1)
    calculer_stats();

  return 1;

}

void ParoiVEF_TBLE::traitement_keps(DoubleTab& tab_k_eps, int num_face, const IntTab& face_voisins,
                                    const IntTab& elem_faces, int nfac, double dist, double d_plus, double d_visco, double u_star)
{
  IntVect num(nfac);
  int elem=face_voisins(num_face,0);
  int nf2;

  // on determine les face de l'elements qui sont autres que le num_face traite
  for (nf2=0; nf2<nfac; nf2++)
    {
      num[nf2] = elem_faces(elem,nf2);
    }
  // Maintenant on place le num_face en fin de tableau
  for (nf2=0; nf2<nfac-1; nf2++)
    {
      num[nf2] = elem_faces(elem,nf2);
      if (num[nf2] == num_face)
        {
          num[nf2] = num[nfac-1];
          num[nfac-1] = num_face;
        }
    }

  // Boucle sur les faces :
  for (int nf=0; nf<nfac; nf++)
    {
      if (num[nf]==num_face)
        {


          //      if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
          // Strategie pour les tetras :
          // On impose k et eps a la paroi :
          // approximation: d(k)/d(n) = 0 a la paroi
          // c'est faux mais ca marche
          tab_k_eps(num[nf],0)=0.;
          tab_k_eps(num[nf],1)=0.;
          int nk=0;

          for (int k=0; k<nfac; k++)
            //if ( (num[k] >= ndebint) && (k != nf))
            if ( (face_keps_imposee_[num[k]]>-1) && (k != nf))
              {

                tab_k_eps(num[nf],0)+= tab_k_eps(num[k],0);
                tab_k_eps(num[nf],1)+= tab_k_eps(num[k],1);
                nk++;
              }

          if (nk != 0 )
            {
              tab_k_eps(num[nf],0)/=nk;
              tab_k_eps(num[nf],1)/=nk;
            }

        }


      // On verifie si num[nf] n'est pas une face de bord :

      else  if ( (face_keps_imposee_[num[nf]]>-1))//if (num[nf]>=ndebint)
        {
          calculer_k_eps(tab_k_eps(num[nf],0),tab_k_eps(num[nf],1),d_plus,u_star,d_visco,dist);


          /*  double y_plus = d_plus;

          if (y_plus<=8)
          {
          tab_k_eps(elem,0)=0.;
          tab_k_eps(elem,1)=0.;
          }
          else if (y_plus>8 && y_plus<30)
          {
          double u_star = tab_u_star_(num_face);
          double lm_plus = calcul_lm_plus(y_plus);
          double  deriv = Fdypar_direct(lm_plus);
          double x = lm_plus*u_star*deriv;
          tab_k_eps(elem,0) = x*x/sqrt(CMU_DEF);
          tab_k_eps(elem,1) = (tab_k_eps(elem,0)*u_star*u_star*deriv)*sqrt(CMU_DEF)/d_visco;
          }
          else if (y_plus>=30)
          {
          double us2 = tab_u_star_(num_face)*tab_u_star_(num_face);
          tab_k_eps(elem,0)=us2/sqrt(CMU_DEF);
          tab_k_eps(elem,1)=us2*u_star/K_DEF/dist;
          }
          */

        }
      else
        {
          //Cerr<<"Attention, on n'a rien fait dans Paroi_std_hyd_VEF::calculer_hyd"<<finl;
          //Cerr<<"pour le numero de face num[nf] = "<<num[nf]<<" et num_face = "<<num_face<<finl;
          //Cerr<<"ndebint = "<<ndebint<<finl;
        }
    }
}

int ParoiVEF_TBLE::calculer_k_eps(double& k, double& eps , double yp, double u_star,
                                  double d_visco, double dist)
{
  // PQ : 05/04/07 : formulation continue de k et epsilon
  //  assurant le bon comportement asymptotique
  k    = 0.07*yp*yp*(exp(-yp/9.))+(1./(sqrt(Cmu)))*pow((1.-exp(-yp/20.)),2);  // k_plus
  k   *= u_star*u_star;
  // PL: 50625=15^4 on evite d'utiliser pow car lent
  eps  = (1./(Kappa*pow(yp*yp*yp*yp+50625,0.25)));  // eps_plus
  eps *= pow(u_star,4)/d_visco;

  return 1;
}

int ParoiVEF_TBLE::calculer_stats()
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const int& nfac = zone.nb_faces_elem();

  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const double& tps = eqn_hydr.inconnue().temps();
  const double& dt = eqn_hydr.schema_temps().pas_de_temps();

  int num_face, num_face_global;
  double surf;
  double nx,ny,nz=0.; // vecteur normal a la paroi
  double t1x,t1y,t1z=0.; // vecteur tangentiel_1 a la paroi
  double t2x,t2y,t2z=0.; // vecteur tangentiel_2 a la paroi (n^t1)
  double Unp1_t1,Unp1_t2=0.;
  double Fx, Fy, Fz;
  double u,v,w=0.;
  IntVect num(nfac);


  //////////////////////////////////////
  //Moyennes Temporelles
  //////////////////////////////////////
  if(((tps>tps_deb_stats) && (tps<tps_fin_stats)) && (oui_stats!=0))
    {
      if (dimension==2)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              num_face=num_faces_post(j);
              num_face_global=num_global_faces_post(j);

              surf=sqrt(face_normale(num_face_global,0)*face_normale(num_face_global,0)+
                        face_normale(num_face_global,1)*face_normale(num_face_global,1));

              nx = face_normale(num_face_global,0)/surf;
              ny = face_normale(num_face_global,1)/surf;

              t1x = -ny;
              t1y =  nx;

              for(int i=0 ; i<nb_pts+1 ; i++)
                {
                  Unp1_t1 = eq_vit[num_face].get_Unp1(0,i);
                  Fx = t1x*eq_vit[num_face].get_F(0,i);
                  Fy = t1y*eq_vit[num_face].get_F(0,i);
                  u = t1x*Unp1_t1;
                  v = t1y*Unp1_t1;

                  calculer_stat(j,i,Fx, Fy, 0, u,v,0,dt);
                }
            }
        }
      else if (dimension==3)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              num_face=num_faces_post(j);
              num_face_global=num_global_faces_post(j);

              surf=sqrt(face_normale(num_face_global,0)*face_normale(num_face_global,0)+
                        face_normale(num_face_global,1)*face_normale(num_face_global,1)+
                        face_normale(num_face_global,2)*face_normale(num_face_global,2));

              nx = face_normale(num_face_global,0)/surf;
              ny = face_normale(num_face_global,1)/surf;
              nz = face_normale(num_face_global,2)/surf;

              if( est_egal(nx,0.) && est_egal(ny,0.) )
                {
                  t1x = 0.;
                  t1y = 1;
                  t1z = 0;
                }
              else
                {
                  t1x = -ny/sqrt(nx*nx+ny*ny);
                  t1y =  nx/sqrt(nx*nx+ny*ny);
                  t1z =  0.;
                }

              t2x =  ny*t1z - nz*t1y;
              t2y =  nz*t1x - nx*t1z;
              t2z =  nx*t1y - ny*t1x;

              for(int i=0 ; i<nb_pts+1 ; i++)
                {

                  Unp1_t1 = eq_vit[num_face].get_Unp1(0,i);
                  Unp1_t2 = eq_vit[num_face].get_Unp1(1,i);
                  Fx = t1x*eq_vit[num_face].get_F(0,i)+t2x*eq_vit[num_face].get_F(1,i);
                  Fy = t1y*eq_vit[num_face].get_F(0,i)+t2y*eq_vit[num_face].get_F(1,i);
                  Fz = t1z*eq_vit[num_face].get_F(0,i)+t2z*eq_vit[num_face].get_F(1,i);

                  u = t1x*Unp1_t1 + t2x*Unp1_t2 ;
                  v = t1y*Unp1_t1 + t2y*Unp1_t2 ;
                  w = t1z*Unp1_t1 + t2z*Unp1_t2 ;

                  calculer_stat(j,i,Fx, Fy, Fz, u,v,w,dt);
                }
            }
        }
    }
  return 1;
}



void ParoiVEF_TBLE::imprimer_ustar(Sortie& os) const
{
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const double& tps = eqn_hydr.inconnue().temps();

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const int& nfac = zone.nb_faces_elem();

  int num_face, num_face_global;
  double surf;
  double nx,ny,nz=0.; // vecteur normal a la paroi
  double t1x,t1y,t1z=0.; // vecteur tangentiel_1 a la paroi
  double t2x,t2y,t2z=0.; // vecteur tangentiel_2 a la paroi (n^t1)
  double Unp1_t1,Unp1_t2=0.;
  double u,v,w=0.;
  double norm_v=0.;
  double nut=0.;
  IntVect num(nfac);

  for(int j=0; j<nb_post_pts; j++)
    {
      num_face=num_faces_post(j);
      num_face_global=num_global_faces_post(j);

      Nom tmp;
      tmp="tble_post_";
      tmp+=nom_pts[j];
      tmp+=".dat";

      SFichier fic_post(tmp, ios::app);

      fic_post << "# t="<< tps << " " ;
      fic_post << "x= " << zone_VEF.xv(num_face_global,0) << " " << "y= " << zone_VEF.xv(num_face_global,1) ;
      if (dimension==3) fic_post << " "<< "z= " << zone_VEF.xv(num_face_global,2);
      fic_post << " " << "u*= " << tab_u_star_(num_face_global)  ;
      //fic_post << " " << "dp/dt1= " << eq_vit[num_face].get_F0(0)  ;
      //if (dimension==3) fic_post << " " << "dp/dt2= " << eq_vit[num_face].get_F0(1)  ;

      if (dimension==2)
        {
          surf=sqrt(face_normale(num_face_global,0)*face_normale(num_face_global,0)+
                    face_normale(num_face_global,1)*face_normale(num_face_global,1));

          nx = face_normale(num_face_global,0)/surf;
          ny = face_normale(num_face_global,1)/surf;

          t1x = -ny;
          t1y =  nx;

          fic_post << " " << "F_x= " << eq_vit[num_face].get_F0(0)*t1x << " F_y= " << eq_vit[num_face].get_F0(0)*t1y << finl;
          fic_post << "# d_paroi u v  norm_v nu_t" << finl;

          for(int i=0; i<nb_pts+1; i++)
            {
              Unp1_t1 = eq_vit[num_face].get_Unp1(0,i);
              nut = (i<nb_pts)? eq_vit[num_face].get_visco_tot(i) : 0;

              u = t1x*Unp1_t1;
              v = t1y*Unp1_t1;
              norm_v=sqrt(u*u+v*v);

              fic_post <<  eq_vit[num_face].get_y(i) << " " << u << " " << v << " " << norm_v << " " << nut <<  finl;
            }
        }
      else if (dimension==3)
        {
          surf=sqrt(face_normale(num_face_global,0)*face_normale(num_face_global,0)+
                    face_normale(num_face_global,1)*face_normale(num_face_global,1)+
                    face_normale(num_face_global,2)*face_normale(num_face_global,2));

          nx = face_normale(num_face_global,0)/surf;
          ny = face_normale(num_face_global,1)/surf;
          nz = face_normale(num_face_global,2)/surf;

          if( est_egal(nx,0.) && est_egal(ny,0.) )
            {
              t1x =  0.;
              t1y =  1.;
              t1z =  0.;
            }
          else
            {
              t1x = -ny/sqrt(nx*nx+ny*ny);
              t1y =  nx/sqrt(nx*nx+ny*ny);
              t1z =  0.;
            }


          t2x =  ny*t1z - nz*t1y;
          t2y =  nz*t1x - nx*t1z;
          t2z =  nx*t1y - ny*t1x;

          fic_post << " " << "F_x= " << eq_vit[num_face].get_F0(0)*t1x+eq_vit[num_face].get_F0(1)*t2x
                   << " F_y= " << eq_vit[num_face].get_F0(0)*t1y+eq_vit[num_face].get_F0(1)*t2y
                   << " F_z= " << eq_vit[num_face].get_F0(0)*t1z+eq_vit[num_face].get_F0(1)*t2z
                   << finl;
          fic_post << "# dp/dt1= " << eq_vit[num_face].get_F0(0) << " dp/dt2= " << eq_vit[num_face].get_F0(1) << finl;
          fic_post << "# d_paroi u v w norm_v nu_t" << finl;

          for(int i=0 ; i<nb_pts+1 ; i++)
            {
              Unp1_t1 = eq_vit[num_face].get_Unp1(0,i);
              Unp1_t2 = eq_vit[num_face].get_Unp1(1,i);

              u = t1x*Unp1_t1 + t2x*Unp1_t2 ;
              v = t1y*Unp1_t1 + t2y*Unp1_t2 ;
              w = t1z*Unp1_t1 + t2z*Unp1_t2 ;
              norm_v=sqrt(u*u+v*v+w*w);
              nut = (i<nb_pts)? eq_vit[num_face].get_visco_tot(i) : 0;

              fic_post <<  eq_vit[num_face].get_y(i) << " " << u << " " << v << " " << w << " " << norm_v << " " << nut << finl;
            }
        }
    }
  Paroi_TBLE_QDM::imprimer_stat(os, tps);


}

int ParoiVEF_TBLE::sauvegarder(Sortie& os) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  double tps =  mon_modele_turb_hyd->equation().inconnue().temps();
  return Paroi_TBLE_QDM::sauvegarder(os, zone_VEF, la_zone_Cl_VEF.valeur(), tps);
}


int ParoiVEF_TBLE::reprendre(Entree& is)
{
  if (la_zone_VEF.non_nul()) // test pour ne pas planter dans "avancer_fichier(...)"
    {
      const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
      double tps_reprise = mon_modele_turb_hyd->equation().schema_temps().temps_courant();
      return Paroi_TBLE_QDM::reprendre(is, zone_VEF, la_zone_Cl_VEF.valeur(), tps_reprise);
    }
  return 1;
}


const Probleme_base& ParoiVEF_TBLE::getPbBase() const
{
  const Probleme_base& pb_base  = mon_modele_turb_hyd->equation().probleme();
  return pb_base;
}

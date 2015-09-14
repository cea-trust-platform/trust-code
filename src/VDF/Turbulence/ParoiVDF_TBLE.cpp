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
// File:        ParoiVDF_TBLE.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////


#include <ParoiVDF_TBLE.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Fluide_Incompressible.h>
#include <EFichier.h>
#include <Diffu_lm.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <Operateur_Grad.h>
#include <Probleme_base.h>
#include <Modele_turbulence_scal_base.h>
#include <Paroi_TBLE_scal_VDF.h>
#include <Terme_Boussinesq_VDF_Face.h>
#include <time.h>
#include <SFichier.h>
#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd_base.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(ParoiVDF_TBLE,"Paroi_TBLE_VDF",Paroi_hyd_base_VDF);

ParoiVDF_TBLE::ParoiVDF_TBLE()
{
  alpha_cv=0;
}

//     printOn()
/////

Sortie& ParoiVDF_TBLE::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& ParoiVDF_TBLE::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" wall law"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void ParoiVDF_TBLE::set_param(Param& param)
{
  Paroi_hyd_base_VDF::set_param(param);
  Paroi_TBLE_QDM::set_param(param);
  Paroi_log_QDM::set_param(param);
  param.ajouter_flag("alpha_cv",&alpha_cv);
  param.ajouter_non_std("nu_t_dyn",(this));
  param.ajouter_non_std("tps_start_stat_nu_t_dyn",(this));
  param.ajouter_non_std("tps_nu_t_dyn",(this));
  param.ajouter_non_std("sonde_tble",(this));
  param.ajouter_non_std("stationnaire",(this));
  param.ajouter_non_std("mu",(this));
  param.ajouter_non_std("lambda",(this));
  param.ajouter_non_std("sans_source_boussinesq",(this));
}

int ParoiVDF_TBLE::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Paroi_TBLE_QDM::lire_motcle_non_standard(mot,is);
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe ParoiVDF_TBLE
//
/////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////
// // Initialisation des tableaux
// ////////////////////////////////////////////////

int ParoiVDF_TBLE::init_lois_paroi()
{
  Cerr << "debut init_lois_paroi" << finl;

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  int compteur_faces_paroi = 0;
  int elem;
  double vmoy = 0.;
  double dist; //distance du premier centre de maille a la paroi


  init_lois_paroi_();
  Paroi_TBLE_QDM::init_lois_paroi(zone_VDF, la_zone_Cl_VDF.valeur());



  corresp.resize(la_zone_VDF->nb_faces_bord()); // Ce tableau ete introduit a l origine pour les termes convectifs
  // On le garde ?
  compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi



  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              corresp[num_face]=compteur_faces_paroi;
              compteur_faces_paroi++;
            }
        }
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
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();

          ////////////
          /// 2D  ////
          ////////////

          if (dimension == 2 )
            {
              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  //ici ori=direction perpendiculaire a la paroi
                  int ori = orientation(num_face);

                  if ((elem = face_voisins(num_face,0)) == -1)
                    elem = face_voisins(num_face,1);


                  //Distance a la paroi du 1er centre de maille
                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);

                  Diffu_totale_hyd_base& diffu_hyd = ref_cast_non_const(Diffu_totale_hyd_base, eq_vit[corresp[num_face]].get_diffu()); //modele de viscosite turbulente
                  diffu_hyd.setKappa(Kappa);

                  eq_vit[corresp[num_face]].set_y0(0.); //ordonnee de la paroi
                  eq_vit[corresp[num_face]].set_yn(dist); //ordonnee du 1er centre de maille

                  eq_vit[corresp[num_face]].initialiser(nb_comp, nb_pts,
                                                        fac, epsilon, max_it, nu_t_dyn); //nbre de pts maillage fin

                  eq_vit[corresp[num_face]].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi


                  vmoy = 0.5*(vit(elem_faces(elem,(ori+1)%4)) + vit(elem_faces(elem,(ori+3)%4)));

                  eq_vit[corresp[num_face]].set_u_yn(0,vmoy); //1ere composante de la vitesse en yn

                  //vitesse sur le maillage fin a l'instant initial
                  if (reprise_ok==0)
                    eq_vit[corresp[num_face]].set_Uinit_lin(0, 0., vmoy);
                  else
                    {
                      for (int itble=0; itble<nb_pts+1; itble++)
                        eq_vit[corresp[num_face]].set_Uinit(0,itble,valeurs_reprises(corresp[num_face], 0, itble)) ;
                    }
                  compteur_faces_paroi++;

                }//Fin boucle sur les faces de bord

            }



          ////////////
          /// 3D  ////
          ////////////

          else if (dimension == 3)
            {
              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {

                  //ici ori=direction perpendiculaire a la paroi
                  int ori = orientation(num_face);

                  if ((elem = face_voisins(num_face,0)) == -1)
                    {
                      elem = face_voisins(num_face,1);
                    }


                  //Distance a la paroi du 1er centre de maille
                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);

                  eq_vit[corresp[num_face]].set_y0(0.); //ordonnee de la paroi
                  eq_vit[corresp[num_face]].set_yn(dist); //ordonnee du 1er centre de maille

                  eq_vit[corresp[num_face]].initialiser(nb_comp, nb_pts,
                                                        fac, epsilon, max_it,
                                                        nu_t_dyn);

                  eq_vit[corresp[num_face]].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi
                  eq_vit[corresp[num_face]].set_u_y0(1,0.); //2e composante de la vitesse a la paroi

                  vmoy = 0.5*(vit(elem_faces(elem,(ori+1)%6)) + vit(elem_faces(elem,(ori+4)%6)));

                  eq_vit[corresp[num_face]].set_u_yn(0,vmoy); //1ere composante de la vitesse en yn

                  //vitesse sur le maillage fin a l'instant initial
                  if (reprise_ok==0)
                    eq_vit[corresp[num_face]].set_Uinit_lin(0, 0., vmoy);
                  else
                    {
                      for (int itble=0; itble<nb_pts+1; itble++)
                        eq_vit[corresp[num_face]].set_Uinit(0,itble,valeurs_reprises(corresp[num_face], 0, itble)) ;
                    }

                  vmoy = 0.5*(vit(elem_faces(elem,(ori+2)%6)) + vit(elem_faces(elem,(ori+5)%6)));
                  eq_vit[corresp[num_face]].set_u_yn(1,vmoy); //2e composante de la vitesse en yn
                  if (reprise_ok==0)
                    eq_vit[corresp[num_face]].set_Uinit_lin(1, 0., vmoy);
                  else
                    {
                      for (int itble=0; itble<nb_pts+1; itble++)
                        eq_vit[corresp[num_face]].set_Uinit(1,itble,valeurs_reprises(corresp[num_face], 1, itble)) ;
                    }

                  compteur_faces_paroi++;

                }//Fin boucle sur les faces de bord
            }// Fin if dim 3
        }//Fin if Paroi_fixe
    }//Fin boucle sur les bords parietaux


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
          if (sub_type(Terme_Boussinesq_VDF_Face,ts))
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



  Cerr << "fin init_lois_paroi" << finl;
  return 1;
}


/////////////////////////////////////////////////
//Calcul du cisaillement a la paroi
////////////////////////////////////////////////

int ParoiVDF_TBLE::calculer_hyd(DoubleTab& nu_t, DoubleTab& tab_k)
{
  return calculer_hyd(nu_t,0,tab_k);
}

int ParoiVDF_TBLE::calculer_hyd(DoubleTab& tab_k_eps)
{

  DoubleTab bidon(0);
  // bidon ne servira pas
  return calculer_hyd(tab_k_eps,1,bidon);

}

int ParoiVDF_TBLE::calculer_hyd(DoubleTab& tab1,int isKeps,DoubleTab& tab2)
{
  // si isKeps=1, on est dans le cas k-eps

  //  clock_t clock0,clock1;

  //  clock0=clock();

  /*  if (Process::nproc()>1)
      {
      Cerr << "ParoiVDF_TBLE::calculer_hyd n'est pas parallelise." << finl;
      exit();
      }
  */

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
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
  int ori;
  int elem;

  int signe;
  int itmax=0;

  double gradient_de_pression0 = 0., gradient_de_pression1 = 0., vmoy = 0., ts0 =0., ts1=0.;
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs(); //vitesse
  Navier_Stokes_std& eqnNS = ref_cast_non_const(Navier_Stokes_std, eqn_hydr);

  // Calcul du gradient de pression
  DoubleTab grad_p(vit);
  const DoubleTab& p = eqnNS.pression().valeurs();
  const Operateur_Grad& gradient = eqnNS.operateur_gradient();
  gradient.calculer(p, grad_p);
  const DoubleTab& visco_turb = mon_modele_turb_hyd->viscosite_turbulente().valeurs();


  DoubleTab termes_sources;
  termes_sources.resize(zone_VDF.nb_faces());
  termes_sources = 0.;
  // On calcule les termes sources, sauf celui de Boussinesq (TBLE recalcule par lui meme ce terme s'il est demande)
  const Sources& les_sources=eqn_hydr.sources();
  int nb_sources=les_sources.size();
  for (int j=0; j<nb_sources; j++)
    {
      const Source_base& ts = les_sources(j).valeur();
      if (!sub_type(Terme_Boussinesq_base,ts))
        {
          ts.ajouter(termes_sources);
        }
    }


  const double& tps = eqnNS.schema_temps().temps_courant();
  const double& dt = eqnNS.schema_temps().pas_de_temps();
  const double& dt_min = eqnNS.schema_temps().pas_temps_min();

  int compteur_faces_paroi = 0;




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

          ///////////////////////
          //   Si probleme 2D  //
          ///////////////////////


          if(dimension == 2)
            {
              //eq_vit.dimensionner(2*la_zone_VDF->nb_faces_bord());

              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  ori = orientation(num_face);

                  //Selection de l'element associe a une face parietale
                  //et initialisation de l'entier signe

                  if ((elem = face_voisins(num_face,0)) == -1)
                    {
                      elem = face_voisins(num_face,1);
                      signe = 1;
                    }
                  else signe = -1 ;


                  if(dt<dt_min)
                    eq_vit[corresp[num_face]].set_dt(1.e12); // Ca ne devrait pas servir ???
                  else
                    eq_vit[corresp[num_face]].set_dt(dt);



                  // 1er couple de faces perpendiculaires a la paroi
                  int face1 = elem_faces(elem,(ori+1));
                  int face2 = elem_faces(elem,(ori+3)%4);





                  // Terme source de Boussinesq si demande
                  DoubleVect ts_boussi(nb_pts+1);
                  ts_boussi = 0.;
                  if (source_boussinesq==1)
                    {
                      const Champ_Don_base& ch_gravite=le_fluide.gravite();
                      const DoubleVect& gravite = ch_gravite.valeurs();
                      const Equation_base& eqn_th = mon_modele_turb_hyd->equation().probleme().equation(1);
                      const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqn_th.get_modele(TURBULENCE).valeur());
                      const Turbulence_paroi_scal_base& loi = le_mod_turb_th.loi_paroi().valeur();
                      Paroi_TBLE_scal_VDF& loi_tble_T = ref_cast_non_const(Paroi_TBLE_scal_VDF,loi);

                      if (loi_tble_T.est_initialise())
                        {

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

                          for(int i=0; i<nb_pts+1; i++)
                            {
                              ts_boussi(i) = -g_t*beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);
                            }
                          /*Cout << "gt " << g_t << finl;
                            Cout << "T0 " << T0 << finl;
                            Cout << "beta_t " << beta_t << finl;*/
                        }
                    }

                  ////////////////////////////////////////////////////////////
                  ////// couple  de faces perpendiculaires a la paroi /////
                  ////////////////////////////////////////////////////////////

                  vmoy = 0.5*(vit(face1) + vit(face2));

                  //vitesse sur le maillage fin a l'instant initial

                  ts0 = 0.5*(termes_sources(face1)/volumes_entrelaces(face1)
                             + termes_sources(face2)/volumes_entrelaces(face2));

                  eq_vit[corresp[num_face]].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi

                  eq_vit[corresp[num_face]].set_u_yn(0,vmoy); //vitesse en yn

                  //*** Gradient de pression du 1er couple de faces
                  // perpendiculaires a la paroi ***

                  //gradient de pression (constant dans le maillage fin)
                  gradient_de_pression0 = 0.5*(grad_p(face1)/volumes_entrelaces(face1)+grad_p(face2)/volumes_entrelaces(face2));

                  for(int i=0 ; i<nb_pts+1; i++)
                    eq_vit[compteur_faces_paroi].set_F(0, i, -gradient_de_pression0+ts0+ts_boussi(i));


                  //On resoud les equations aux limites simplifiees
                  //pendant un pas de temps du maillage grossier

                  if (statio==0)
                    eq_vit[compteur_faces_paroi].aller_au_temps(tps);
                  else
                    eq_vit[compteur_faces_paroi].aller_jusqu_a_convergence(max_it_statio, eps_statio);


                  if(itmax<eq_vit[corresp[num_face]].get_it())
                    {
                      itmax = eq_vit[compteur_faces_paroi].get_it();
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
                      Cisaillement_paroi_(num_face,1) =  eq_vit[corresp[num_face]].get_cis(0)*signe;
                    }
                  else if (ori == 1)
                    {

                      Cisaillement_paroi_(num_face,0) = eq_vit[corresp[num_face]].get_cis(0)*signe;
                      Cisaillement_paroi_(num_face,1) = 0.;
                    }

                  tab_u_star_(num_face) = pow(eq_vit[corresp[num_face]].get_cis(0)*eq_vit[corresp[num_face]].get_cis(0),0.25);

                  if (isKeps==0) // on est en LES
                    {

                      ////////////////////////////////////////////////////////////////////
                      //nu_t longueur de melange en premiere maille calcule dans Diffu_lm
                      ////////////////////////////////////////////////////////////////////

                      tab1(elem)=eq_vit[corresp[num_face]].get_nu_t_yn();
                    }
                  else if (isKeps==1) // on est en K-eps
                    {
                      double dist;

                      if (axi)
                        dist = zone_VDF.dist_norm_bord_axi(num_face);
                      else
                        dist = zone_VDF.dist_norm_bord(num_face);

                      double d_visco;

                      if (l_unif==1)
                        d_visco = visco;
                      else
                        d_visco = tab_visco[elem];


                      double y_plus = dist*tab_u_star_(num_face)/d_visco;

                      if (y_plus<8)
                        {
                          tab1(elem,0)=0.;
                          tab1(elem,1)=0.;
                        }
                      else if (y_plus>8 && y_plus<30)
                        {
                          double u_star = tab_u_star_(num_face);
                          double lm_plus = calcul_lm_plus(y_plus);
                          double  deriv = Fdypar_direct(lm_plus);
                          double x = lm_plus*u_star*deriv;
                          tab1(elem,0) = x*x/sqrt(Cmu) ;
                          tab1(elem,1) = (tab1(elem,0)*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;
                        }
                      else if (y_plus>30)
                        {
                          double us2 = tab_u_star_(num_face)*tab_u_star_(num_face);
                          tab1(elem,0)=us2/sqrt(Cmu);
                          tab1(elem,1)=us2*tab_u_star_(num_face)/Kappa/dist;
                        }
                    }

                  compteur_faces_paroi++;

                }//fin boucle sur les faces de bords parietaux


            }//fin if(dim2)


          ///////////////////////
          //   Si probleme 3D  //
          ///////////////////////

          else if(dimension == 3)
            {
              //Boucle sur les faces des bords parietaux

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  //ici ori=direction perpendiculaire a la paroi
                  ori = orientation(num_face);


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
                    eq_vit[corresp[num_face]].set_dt(1.e12); // Ca ne devrait pas servir ???
                  else
                    eq_vit[corresp[num_face]].set_dt(dt);


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

                  eq_vit[corresp[num_face]].set_u_y0(0,0.); //1ere composante de la vitesse a la paroi

                  eq_vit[corresp[num_face]].set_u_yn(0,vmoy); //vitesse en yn

                  //*** Gradient de pression du 1er couple de faces
                  // perpendiculaires a la paroi ***

                  //gradient de pression (constant dans le maillage fin)
                  gradient_de_pression0 = 0.5*(grad_p(face1)/volumes_entrelaces(face1)+grad_p(face2)/volumes_entrelaces(face2));

                  ////////////////////////////////////////////////////////////
                  ////// 2e couple  de faces perpendiculaires a la paroi /////
                  ////////////////////////////////////////////////////////////

                  vmoy = 0.5*(vit(face3) + vit(face4));

                  ts1 = 0.5*(termes_sources(face3)/volumes_entrelaces(face3)
                             + termes_sources(face4)/volumes_entrelaces(face4));

                  eq_vit[corresp[num_face]].set_u_y0(1,0.); //2e composante de la vitesse a la paroi

                  eq_vit[corresp[num_face]].set_u_yn(1,vmoy); //vitesse en yn

                  //*** Gradient de pression du 2e couple de faces
                  // perpendiculaires a la paroi ***


                  //gradient de pression (constant dans le maillage fin)

                  gradient_de_pression1 = 0.5*(grad_p(face3)/volumes_entrelaces(face3)+grad_p(face4)/volumes_entrelaces(face4));

                  // Terme source de Boussinesq si demande
                  DoubleTab ts_boussi(nb_pts+1,2);
                  ts_boussi = 0.;
                  if (source_boussinesq==1)
                    {
                      const Champ_Don_base& ch_gravite=le_fluide.gravite();
                      const DoubleVect& gravite = ch_gravite.valeurs();
                      const Equation_base& eqn_th = mon_modele_turb_hyd->equation().probleme().equation(1);
                      const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqn_th.get_modele(TURBULENCE).valeur());
                      const Turbulence_paroi_scal_base& loi = le_mod_turb_th.loi_paroi().valeur();
                      Paroi_TBLE_scal_VDF& loi_tble_T = ref_cast_non_const(Paroi_TBLE_scal_VDF,loi);
                      if (loi_tble_T.est_initialise())
                        {
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

                          for(int i=0; i<nb_pts+1; i++)
                            {
                              ts_boussi(i,0) = -gt_vect((ori+1)%dimension)*beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);
                              ts_boussi(i,1) = -gt_vect((ori+2)%dimension)*beta_t*(loi_tble_T.get_eq_couche_lim_T(compteur_faces_paroi).get_Unp1(0,i)-T0);
                              /*Cout << "gt_vect((ori+1)%dimension) " << gt_vect((ori+1)%dimension) << finl;
                                Cout << "gt_vect((ori+2)%dimension) " << gt_vect((ori+2)%dimension) << finl;
                                Cout << "vit(face1) " << vit(face1) << finl;
                                Cout << "vit(face3) " << vit(face3) << finl;
                                Cout << "T0 " << T0 << finl;
                                Cout << "beta_t " << beta_t << finl;*/
                            }
                        }
                    }



                  if(alpha_cv == 1) //Si TBLE + termes convectifs
                    calculer_convection(num_face,face1, face2, face3, face4, elem, ndeb, nfin, ori, gradient_de_pression0, ts0, gradient_de_pression1, ts1);
                  else
                    {
                      for(int i=0 ; i<nb_pts+1 ; i++)
                        {
                          eq_vit[corresp[num_face]].set_F(0, i, - gradient_de_pression0 + ts0 + ts_boussi(i,0));
                          eq_vit[corresp[num_face]].set_F(1, i, - gradient_de_pression1 + ts1 + ts_boussi(i,1));
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

                  //On resoud les equations aux limites simplifiees
                  //pendant un pas de temps du maillage grossier
                  if (statio==0)
                    eq_vit[corresp[num_face]].aller_au_temps(tps);
                  else
                    eq_vit[corresp[num_face]].aller_jusqu_a_convergence(max_it_statio, eps_statio);

                  if(itmax<eq_vit[corresp[num_face]].get_it())
                    {
                      itmax = eq_vit[corresp[num_face]].get_it();
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
                      Cisaillement_paroi_(num_face,0) = 0.;
                      Cisaillement_paroi_(num_face,1) =  eq_vit[corresp[num_face]].get_cis(0)*signe;
                      Cisaillement_paroi_(num_face,2) =  eq_vit[corresp[num_face]].get_cis(1)*signe;
                    }
                  else if (ori == 1)
                    {
                      Cisaillement_paroi_(num_face,0) = eq_vit[corresp[num_face]].get_cis(1)*signe;
                      Cisaillement_paroi_(num_face,1) = 0.;
                      Cisaillement_paroi_(num_face,2) = eq_vit[corresp[num_face]].get_cis(0)*signe;
                    }
                  else
                    {
                      Cisaillement_paroi_(num_face,0) = eq_vit[corresp[num_face]].get_cis(0)*signe;
                      Cisaillement_paroi_(num_face,1) = eq_vit[corresp[num_face]].get_cis(1)*signe;
                      Cisaillement_paroi_(num_face,2) = 0.;
                    }

                  tab_u_star_(num_face) = sqrt(sqrt(eq_vit[corresp[num_face]].get_cis(0)*eq_vit[corresp[num_face]].get_cis(0)
                                                    +eq_vit[corresp[num_face]].get_cis(1)*eq_vit[corresp[num_face]].get_cis(1)));


                  if (isKeps==0) // on est en LES
                    {
                      if((nu_t_dyn==0)||(tps<tps_nu_t_dyn))
                        {
                          tab1(elem)=eq_vit[corresp[num_face]].get_nu_t_yn();
                        }
                      else
                        eq_vit[corresp[num_face]].set_nu_t_yn(visco_turb_moy(elem));
                    }
                  else if (isKeps==1) // on est en K-eps
                    {
                      double dist;

                      if (axi)
                        dist = zone_VDF.dist_norm_bord_axi(num_face);
                      else
                        dist = zone_VDF.dist_norm_bord(num_face);
                      double d_visco;

                      if (l_unif==1)
                        d_visco = visco;
                      else
                        d_visco = tab_visco[elem];

                      double y_plus = dist*tab_u_star_(num_face)/d_visco;

                      if (y_plus<8)
                        {
                          tab1(elem,0)=0.;
                          tab1(elem,1)=0.;
                        }
                      else if (y_plus>8 && y_plus<30)
                        {
                          double u_star = tab_u_star_(num_face);
                          double lm_plus = calcul_lm_plus(y_plus);
                          double  deriv = Fdypar_direct(lm_plus);
                          double x = lm_plus*u_star*deriv;
                          tab1(elem,0) = x*x/sqrt(Cmu);
                          tab1(elem,1) = (tab1(elem,0)*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;
                        }
                      else if (y_plus>30)
                        {
                          double us2 = tab_u_star_(num_face)*tab_u_star_(num_face);
                          tab1(elem,0)=us2/sqrt(Cmu);
                          tab1(elem,1)=us2*tab_u_star_(num_face)/Kappa/dist;
                        }
                    }

                  compteur_faces_paroi++;

                }//fin boucle sur les faces de bords parietaux
            }//fin if(dim=3)
        }//fin if(sub_type(dirichlet))

    }//fin boucle sur les bords

  SFichier fic("iter.dat",ios::app); // ouverture du fichier iter.dat
  fic << tps << " " << itmax << finl;

  if(oui_stats==1)
    calculer_stats();


  //  clock1=clock();
  //  double time = (clock1-clock0)*1.e-6;
  //  if(je_suis_maitre()) {  Cerr << " CPU time TBLE : " << Process::mp_max(time) << finl; }

  return 1;
}


int ParoiVDF_TBLE::calculer_stats()
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();

  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const double& tps = eqn_hydr.inconnue().temps();
  const double& dt = eqn_hydr.schema_temps().pas_de_temps();


  //////////////////////////////////////
  //Moyennes Temporelles
  //////////////////////////////////////
  if(((tps>tps_deb_stats) && (tps<tps_fin_stats)) && (oui_stats!=0))
    {
      if (dimension==2)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              for(int i=0 ; i<nb_pts+1 ; i++)
                {
                  double u = eq_vit[num_faces_post(j)].get_Unp1(0,i);
                  double F=eq_vit[num_faces_post(j)].get_F(0,i);
                  calculer_stat(j,i,F,0,0,u,0.,0.,dt);
                }
            }
        }
      else if (dimension==3)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              int ori=orientation(num_global_faces_post(j));
              if (ori == 0)
                {
                  for(int i=0 ; i<nb_pts+1 ; i++)
                    {
                      double u=eq_vit[num_faces_post(j)].get_v(i);
                      double v=eq_vit[num_faces_post(j)].get_Unp1(0,i);
                      double w=eq_vit[num_faces_post(j)].get_Unp1(1,i);
                      double Fy=eq_vit[num_faces_post(j)].get_F(0,i);
                      double Fz=eq_vit[num_faces_post(j)].get_F(1,i);
                      calculer_stat(j,i,0, Fy, Fz, u,v,w,dt);
                    }
                }
              else if (ori == 1)
                {
                  for(int i=0 ; i<nb_pts+1 ; i++)
                    {
                      double u=eq_vit[num_faces_post(j)].get_Unp1(1,i);
                      double v=eq_vit[num_faces_post(j)].get_v(i);
                      double w=eq_vit[num_faces_post(j)].get_Unp1(0,i);
                      double Fx=eq_vit[num_faces_post(j)].get_F(1,i);
                      double Fz=eq_vit[num_faces_post(j)].get_F(0,i);
                      calculer_stat(j,i,Fx, 0, Fz, u,v,w,dt);
                    }
                }
              else if (ori == 2)
                {
                  for(int i=0 ; i<nb_pts+1 ; i++)
                    {
                      double u=eq_vit[num_faces_post(j)].get_Unp1(0,i);
                      double v=eq_vit[num_faces_post(j)].get_Unp1(1,i);
                      double w=eq_vit[num_faces_post(j)].get_v(i);
                      double Fx=eq_vit[num_faces_post(j)].get_F(0,i);
                      double Fy=eq_vit[num_faces_post(j)].get_F(1,i);
                      calculer_stat(j,i,Fx,Fy,0,u,v,w,dt);
                    }
                }
            }
        }
    }

  return 1;
}

void ParoiVDF_TBLE::imprimer_ustar(Sortie& os) const
{
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const double& tps = eqn_hydr.inconnue().temps();

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();

  for(int j=0; j<nb_post_pts; j++)
    {
      Nom tmp;
      tmp="tble_post_";
      tmp+=nom_pts[j];
      tmp+=".dat";

      int ori=orientation(num_global_faces_post(j));

      SFichier fic_post(tmp, ios::app);

      fic_post << "# "<< tps << " " <<  "u*= " << tab_u_star_(num_global_faces_post(j))  << finl;
      if (dimension==2)
        {
          for(int i=0; i<nb_pts+1; i++)
            fic_post <<  eq_vit[num_faces_post(j)].get_y(i) << " " << eq_vit[num_faces_post(j)].get_Unp1(0,i) <<  finl;
        }
      else if (dimension==3)
        {
          if (ori == 0)
            for(int i=0; i<nb_pts+1; i++)
              fic_post <<  eq_vit[num_faces_post(j)].get_y(i) << " 0. " << eq_vit[num_faces_post(j)].get_Unp1(0,i) <<
                       " " << eq_vit[num_faces_post(j)].get_Unp1(1,i) <<  finl;
          else if (ori == 1)
            for(int i=0; i<nb_pts+1; i++)
              fic_post <<  eq_vit[num_faces_post(j)].get_y(i) << " " << eq_vit[num_faces_post(j)].get_Unp1(1,i) <<
                       " 0. " << eq_vit[num_faces_post(j)].get_Unp1(0,i) <<  finl;
          else if (ori == 2)
            for(int i=0; i<nb_pts+1; i++)
              fic_post <<  eq_vit[num_faces_post(j)].get_y(i) << " " << eq_vit[num_faces_post(j)].get_Unp1(0,i) <<
                       " " << eq_vit[num_faces_post(j)].get_Unp1(1,i) <<  " 0." << finl;
        }
    }

  Paroi_TBLE_QDM::imprimer_stat(os, tps);

}

int ParoiVDF_TBLE::sauvegarder(Sortie& os) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  double tps =  mon_modele_turb_hyd->equation().inconnue().temps();
  return Paroi_TBLE_QDM::sauvegarder(os, zone_VDF, la_zone_Cl_VDF.valeur(), tps);
}


int ParoiVDF_TBLE::reprendre(Entree& is)
{
  if (la_zone_VDF.non_nul()) // test pour ne pas planter dans "avancer_fichier(...)"
    {
      const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
      double tps_reprise = mon_modele_turb_hyd->equation().schema_temps().temps_courant();
      return Paroi_TBLE_QDM::reprendre(is, zone_VDF, la_zone_Cl_VDF.valeur(), tps_reprise);
    }
  else return 1;
}

const Probleme_base& ParoiVDF_TBLE::getPbBase() const
{
  const Probleme_base& pb_base  = mon_modele_turb_hyd->equation().probleme();
  return pb_base;
}

void ParoiVDF_TBLE::calculer_convection(int num_face, int face1, int face2, int face3, int face4, int elem, int ndeb, int nfin, int ori, double gradient_de_pression0, double ts0, double gradient_de_pression1, double ts1)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double d0d0_1=0;
  double d1d1_1=0;
  double d00d0 = 0., d0vdy= 0., d01d1= 0., d10d0= 0., d1vdy= 0., d11d1=0.;
  int elem_gauche0, elem_gauche1, elem_droite0, elem_droite1;
  int face_gauche0, face_gauche1, face_droite0, face_droite1;

  ////////////////////////////////////////////////////////////////////////////////////
  //Calcul de la vitesse normale a la paroi a partir de la conservation de la masse
  ////////////////////////////////////////////////////////////////////////////////////
  double v = 0.;

  //On cherche les elements voisins de l'element considere

  elem_gauche0 = face_voisins(face1,0);
  elem_droite0 = face_voisins(face2,1);
  if (elem_gauche0 == elem)
    {
      elem_droite0 = face_voisins(face1,1);
      elem_gauche0 = face_voisins(face2,0);
    }

  elem_gauche1 = face_voisins(face3,0);
  elem_droite1 = face_voisins(face4,1);
  if (elem_gauche1 == elem)
    {
      elem_droite1 = face_voisins(face3,1);
      elem_gauche1 = face_voisins(face4,0);
    }

  //On evite les coins interieurs
  if((elem_droite1!=-1)&&(elem_droite0!=-1)&&(elem_gauche1!=-1)&&(elem_gauche0!=-1))
    {
      //On cherche les faces parietales voisines de la face
      face_gauche0 = elem_faces(elem_gauche0, ori);
      if ((face_voisins(face_gauche0,0) != -1)&&(face_voisins(face_gauche0,1) != -1))
        {
          face_gauche0 = elem_faces(elem_gauche0, ori+3);
        }
      face_droite0 = elem_faces(elem_droite0, ori);
      if ((face_voisins(face_droite0,0) != -1)&&(face_voisins(face_droite0,1) != -1))
        {
          face_droite0 = elem_faces(elem_droite0, ori+3);
        }

      face_gauche1 = elem_faces(elem_gauche1, ori);
      if ((face_voisins(face_gauche1,0) != -1)&&(face_voisins(face_gauche1,1) != -1))
        {
          face_gauche1 = elem_faces(elem_gauche1, ori+3);
        }
      face_droite1 = elem_faces(elem_droite1, ori);
      if ((face_voisins(face_droite1,0) != -1)&&(face_voisins(face_droite1,1) != -1))
        {
          face_droite1 = elem_faces(elem_droite1, ori+3);
        }

      //On evite les coins exterieurs (on evite de calculer des gradients sur des faces non parietales)
      if((face_droite1<nfin)&&(face_droite0<nfin)&&(face_gauche1<nfin)&&(face_gauche0<nfin)&&
          (ndeb<=face_droite1)&&(ndeb<=face_droite0)&&(ndeb<=face_gauche1)&&(ndeb<=face_gauche0))
        {
          int i=1;

          ////////////////
          //Calvul de v(1)
          ////////////////


          if(eq_vit[corresp[num_face]].get_Unp1(0,1)>0)
            d0d0_1 = (eq_vit[corresp[face_droite0]].get_Unp1(0,1)-eq_vit[corresp[num_face]].get_Unp1(0,1))
                     /zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3);
          else
            d0d0_1 = (eq_vit[corresp[num_face]].get_Unp1(0,1)-eq_vit[corresp[face_gauche0]].get_Unp1(0,1))
                     /zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3);

          if(eq_vit[corresp[num_face]].get_Unp1(0,1)>0)
            d1d1_1 = (eq_vit[corresp[face_droite1]].get_Unp1(1,1)-eq_vit[corresp[num_face]].get_Unp1(1,1))
                     /zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3);
          else
            d1d1_1 = (eq_vit[corresp[num_face]].get_Unp1(1,1)-eq_vit[corresp[face_gauche1]].get_Unp1(1,1))
                     /zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3);

          v += -(d0d0_1+d1d1_1)
               *0.5*(eq_vit[corresp[num_face]].get_yc(1)-eq_vit[corresp[num_face]].get_yc(0));

          eq_vit[corresp[num_face]].set_v(i,v);
          //fic_v << eq_vit[corresp[num_face]].get_y(i) << " " << -eq_vit[corresp[num_face]].get_v(i) << finl;

          ////////////////
          //Calvul de v(i)
          ////////////////
          for(i=2 ; i<nb_pts ; i++)
            {
              v += -(
                     (eq_vit[corresp[face_droite0]].get_Unp1(0,i)-eq_vit[corresp[face_gauche0]].get_Unp1(0,i))
                     /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)+zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3))
                     +
                     (eq_vit[corresp[face_droite1]].get_Unp1(1,i)-eq_vit[corresp[face_gauche1]].get_Unp1(1,i))
                     /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)+zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3))
                   )
                   *0.5*(eq_vit[corresp[num_face]].get_yc(i)-eq_vit[corresp[num_face]].get_yc(i-1))
                   -(
                     (eq_vit[corresp[face_droite0]].get_Unp1(0,i-1)-eq_vit[corresp[face_gauche0]].get_Unp1(0,i-1))
                     /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)+zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3))
                     +
                     (eq_vit[corresp[face_droite1]].get_Unp1(1,i-1)-eq_vit[corresp[face_gauche1]].get_Unp1(1,i-1))
                     /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)+zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3))
                   )
                   *0.5*(eq_vit[corresp[num_face]].get_yc(i-1)-eq_vit[corresp[num_face]].get_yc(i-2));

              eq_vit[corresp[num_face]].set_v(i,v);
              //fic_v << eq_vit[corresp[num_face]].get_y(i) << " " << -eq_vit[corresp[num_face]].get_v(i) << finl;
            }

          ////////////////
          //Calvul de v(N)
          ////////////////
          //Cerr << "nb_pts = " << nb_pts << finl;
          i=nb_pts;

          v += -(
                 (eq_vit[corresp[face_droite0]].get_Unp1(0,i)-eq_vit[corresp[face_gauche0]].get_Unp1(0,i))
                 /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)+zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3))
                 +
                 (eq_vit[corresp[face_droite1]].get_Unp1(1,i)-eq_vit[corresp[face_gauche1]].get_Unp1(1,i))
                 /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)+zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3))
               )
               *0.5*(eq_vit[corresp[num_face]].get_yc(nb_pts-1)-eq_vit[corresp[num_face]].get_yc(nb_pts-2));

          eq_vit[corresp[num_face]].set_v(nb_pts,v);


          //                  //////////////////////////////////////////////////////////////////////////////////////
          //                  //Fin calcul de la vitesse normale a la paroi a partir de la conservation de la masse
          //                  //////////////////////////////////////////////////////////////////////////////////////

          //                  ///////////////////////////////
          //                  //Calcul des termes convectifs
          //                  ///////////////////////////////

          eq_vit[corresp[num_face]].set_F(0, 0, - gradient_de_pression0 + ts0);
          eq_vit[corresp[num_face]].set_F(1, 0, - gradient_de_pression1 + ts1);

          for(i=1 ; i<nb_pts ; i++)
            {
              d00d0 = (eq_vit[corresp[face_droite0]].get_Unp1(0,i)*eq_vit[corresp[face_droite0]].get_Unp1(0,i)
                       -eq_vit[corresp[face_gauche0]].get_Unp1(0,i)*eq_vit[corresp[face_gauche0]].get_Unp1(0,i))
                      /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)
                        +zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3));

              d01d1 = (eq_vit[corresp[face_droite1]].get_Unp1(0,i)*eq_vit[corresp[face_droite1]].get_Unp1(1,i)
                       -eq_vit[corresp[face_gauche1]].get_Unp1(0,i)*eq_vit[corresp[face_gauche1]].get_Unp1(1,i))
                      /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)
                        +zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3));

              d10d0 = (eq_vit[corresp[face_droite0]].get_Unp1(0,i)*eq_vit[corresp[face_droite0]].get_Unp1(1,i)
                       -eq_vit[corresp[face_gauche0]].get_Unp1(0,i)*eq_vit[corresp[face_gauche0]].get_Unp1(1,i))
                      /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)
                        +zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3));

              d11d1 = (eq_vit[corresp[face_droite1]].get_Unp1(1,i)*eq_vit[corresp[face_droite1]].get_Unp1(1,i)
                       -eq_vit[corresp[face_gauche1]].get_Unp1(1,i)*eq_vit[corresp[face_gauche1]].get_Unp1(1,i))
                      /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)
                        +zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3));

              d0vdy = (eq_vit[corresp[num_face]].get_Unp1(0,i+1)*eq_vit[corresp[num_face]].get_v(i+1)
                       -eq_vit[corresp[num_face]].get_Unp1(0,i-1)*eq_vit[corresp[num_face]].get_v(i-1))
                      /(eq_vit[corresp[num_face]].get_y(i+1)-eq_vit[corresp[num_face]].get_y(i-1));

              d1vdy = (eq_vit[corresp[num_face]].get_Unp1(1,i+1)*eq_vit[corresp[num_face]].get_v(i+1)
                       -eq_vit[corresp[num_face]].get_Unp1(1,i-1)*eq_vit[corresp[num_face]].get_v(i-1))
                      /(eq_vit[corresp[num_face]].get_y(i+1)-eq_vit[corresp[num_face]].get_y(i-1));

              eq_vit[corresp[num_face]].set_F(0, i, - gradient_de_pression0 + ts0 - (d00d0+d01d1+d0vdy));
              eq_vit[corresp[num_face]].set_F(1, i, - gradient_de_pression1 + ts1 - (d11d1+d10d0+d1vdy));

            }

          i=nb_pts;

          d00d0 = (eq_vit[corresp[face_droite0]].get_Unp1(0,i)*eq_vit[corresp[face_droite0]].get_Unp1(0,i)
                   -eq_vit[corresp[face_gauche0]].get_Unp1(0,i)*eq_vit[corresp[face_gauche0]].get_Unp1(0,i))
                  /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)
                    +zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3));

          d01d1 = (eq_vit[corresp[face_droite1]].get_Unp1(0,i)*eq_vit[corresp[face_droite1]].get_Unp1(1,i)
                   -eq_vit[corresp[face_gauche1]].get_Unp1(0,i)*eq_vit[corresp[face_gauche1]].get_Unp1(1,i))
                  /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)
                    +zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3));

          d10d0 = (eq_vit[corresp[face_droite0]].get_Unp1(0,i)*eq_vit[corresp[face_droite0]].get_Unp1(1,i)
                   -eq_vit[corresp[face_gauche0]].get_Unp1(0,i)*eq_vit[corresp[face_gauche0]].get_Unp1(1,i))
                  /(zone_VDF.dist_elem_period(elem_gauche0,elem,(ori+1)%3)
                    +zone_VDF.dist_elem_period(elem,elem_droite0,(ori+1)%3));

          d11d1 = (eq_vit[corresp[face_droite1]].get_Unp1(1,i)*eq_vit[corresp[face_droite1]].get_Unp1(1,i)
                   -eq_vit[corresp[face_gauche1]].get_Unp1(1,i)*eq_vit[corresp[face_gauche1]].get_Unp1(1,i))
                  /(zone_VDF.dist_elem_period(elem_gauche1,elem,(ori+2)%3)
                    +zone_VDF.dist_elem_period(elem,elem_droite1,(ori+2)%3));

          d0vdy = (eq_vit[corresp[num_face]].get_Unp1(0,i)*eq_vit[corresp[num_face]].get_v(i)
                   -eq_vit[corresp[num_face]].get_Unp1(0,i-1)*eq_vit[corresp[num_face]].get_v(i-1))
                  /(eq_vit[corresp[num_face]].get_y(i)-eq_vit[corresp[num_face]].get_y(i-1));

          d1vdy = (eq_vit[corresp[num_face]].get_Unp1(1,i)*eq_vit[corresp[num_face]].get_v(i)
                   -eq_vit[corresp[num_face]].get_Unp1(1,i-1)*eq_vit[corresp[num_face]].get_v(i-1))
                  /(eq_vit[corresp[num_face]].get_y(i)-eq_vit[corresp[num_face]].get_y(i-1));

          //                  //////////////////////////////////////
          //                  //Fin du calcul des termes convectifs
          //                  //////////////////////////////////////

          eq_vit[corresp[num_face]].set_F(0, i, - gradient_de_pression0 + ts0 - (d00d0+d01d1+d0vdy));
          eq_vit[corresp[num_face]].set_F(1, i, - gradient_de_pression1 + ts1 - (d11d1+d10d0+d1vdy));

          //Cerr << "je suis dans les termes convectifs vitesse" << finl;
        }//fin if pas de coins exterieurs
    }//fin if pas de coins interieurs

  else
    {
      for(int i=0 ; i<nb_pts+1 ; i++)
        {
          eq_vit[corresp[num_face]].set_F(0, i, - gradient_de_pression0 + ts0);
          eq_vit[corresp[num_face]].set_F(1, i, - gradient_de_pression1 + ts1);
        }
    }
}

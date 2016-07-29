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
// File:        Paroi_std_hyd_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/42
//
//////////////////////////////////////////////////////////////////////////////


#include <Paroi_std_hyd_VDF.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Fluide_Quasi_Compressible.h>
#include <SFichier.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <Equation_base.h>
#include <Mod_turb_hyd_base.h>
#include <Param.h>
#include <communications.h>
#include <DoubleTrav.h>

Implemente_instanciable(Paroi_std_hyd_VDF,"loi_standard_hydr_VDF",Paroi_hyd_base_VDF);
Implemente_instanciable(Loi_expert_hydr_VDF,"Loi_expert_hydr_VDF",Paroi_std_hyd_VDF);

Sortie& Paroi_std_hyd_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Sortie& Loi_expert_hydr_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Paroi_std_hyd_VDF::readOn(Entree& s)
{
  Paroi_hyd_base_VDF::readOn(s);
  return s ;
}

Entree& Loi_expert_hydr_VDF::readOn(Entree& s)
{
  Param param(que_suis_je());
  Paroi_std_hyd_VDF::set_param(param);

  param.lire_avec_accolades_depuis(s);

  return s ;
}

void Paroi_std_hyd_VDF::set_param(Param& param)
{
  Paroi_hyd_base_VDF::set_param(param);
  Paroi_log_QDM::set_param(param);
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_std_hyd_VDF
//
/////////////////////////////////////////////////////////////////////


int Paroi_std_hyd_VDF::init_lois_paroi()
{
  dplus_.resize(la_zone_VDF->nb_faces_bord());
  uplus_.resize(la_zone_VDF->nb_faces_bord());
  init_lois_paroi_();

  return init_lois_paroi_hydraulique();
}

// Remplissage de la table

int Paroi_std_hyd_VDF::init_lois_paroi_hydraulique()
{
  Cmu = mon_modele_turb_hyd->get_Cmu();
  init_lois_paroi_hydraulique_();
  return 1;
}

// On annule les valeurs des grandeurs turbulentes qui
// correspondent aux mailles de paroi

int Paroi_std_hyd_VDF::preparer_calcul_hyd(DoubleTab& tab)
{
  int nb_dim = tab.nb_dim();
  const IntTab& face_voisins = la_zone_VDF->face_voisins();
  //  const IntVect& orientation = la_zone_VDF->orientation();
  // Boucle sur les bords

  int ndeb,nfin,elem;

  for (int n_bord=0; n_bord<la_zone_VDF->nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))  ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (nb_dim == 1)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              if ( (elem = face_voisins(num_face,0)) != -1)
                tab(elem) = 0;
              else
                {
                  elem = face_voisins(num_face,1);
                  tab(elem) = 0;
                }
          else if (nb_dim == 2)
            {
              int nb_comp= tab.dimension(1);
              for (int num_face=ndeb; num_face<nfin; num_face++)
                if ( (elem = face_voisins(num_face,0)) != -1)
                  for (int k=0; k<nb_comp; k++)
                    tab(elem,k) = 0;
                else
                  {
                    elem = face_voisins(num_face,1);
                    for (int k=0; k<nb_comp; k++)
                      tab(elem,k) = 0;
                  }
            }
          else
            {
              Cerr << "Erreur TRUST dans Paroi_std_hyd_VDF::preparer_calculer_hyd" << finl;
              Cerr << "Le DoubleTab tab ne peut pas avoir plus de 2 entrees" << finl;
              exit();
            }
        }
    }
  return 1;
}

int Paroi_std_hyd_VDF::calculer_hyd(DoubleTab& tab_k_eps)
{
  DoubleTab bidon(0);
  // bidon ne servira pas
  return calculer_hyd(tab_k_eps,1,bidon);
}
int Paroi_std_hyd_VDF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  return calculer_hyd(tab_nu_t,0,tab_k);
}


int Paroi_std_hyd_VDF::calculer_hyd(DoubleTab& tab1,int isKeps,DoubleTab& tab2)
{
  // si isKeps =1 tab1=tab_keps
  //                tab2 bidon
  // si isKeps=0 tab1=tab_nu_t
  //             tab2=tab_k
  //  Cerr<<" Paroi_std_hyd_VDF::calculer_hyd"<<finl;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
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
  int ndeb,nfin;
  int elem,ori;
  double norm_v;
  double dist;
  double u_plus_d_plus,d_visco;
  //double val,val1,val2;

  //****************************************************************
  // Modifs du 19/10 pour etre coherent avec la diffusion turbulente
  double signe;
  //****************************************************************

  // Boucle sur les bords

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = le_bord.num_premiere_face();
      nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl.valeur() ))
        {
          int isdiri=0;
          DoubleTab vitesse_imposee_face_bord(le_bord.nb_faces(),dimension);
          if (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
            {
              isdiri=1;
              const Dirichlet_paroi_defilante& cl_diri = ref_cast(Dirichlet_paroi_defilante,la_cl.valeur());
              for (int face=ndeb; face<nfin; face++)
                for (int k=0; k<dimension; k++)
                  vitesse_imposee_face_bord(face-ndeb,k) = cl_diri.val_imp(face-ndeb,k);
            }
          ArrOfDouble vit_paroi(dimension);
          ArrOfDouble val(dimension-1);


          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              if (isdiri)
                {
                  int rang = num_face-ndeb;
                  for (int k=0; k<dimension; k++)
                    vit_paroi[k]=vitesse_imposee_face_bord(rang,k);
                }
              ori = orientation(num_face);
              if ( (elem =face_voisins(num_face,0)) != -1)
                {
                  //norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                  norm_v=norm_vit(vit,elem,ori,zone_VDF,vit_paroi,val);
                  signe = -1.;
                }
              else
                {
                  elem = face_voisins(num_face,1);
                  //norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                  norm_v=norm_vit(vit,elem,ori,zone_VDF,vit_paroi,val);
                  signe = 1.;
                }
              if (axi)
                dist=zone_VDF.dist_norm_bord_axi(num_face);
              else
                dist=zone_VDF.dist_norm_bord(num_face);
              if (l_unif)
                d_visco = visco;
              else
                d_visco = (tab_visco.nb_dim()==1 ? tab_visco(elem) : tab_visco(elem,0));

              u_plus_d_plus = norm_v*dist/d_visco;

              // Calcul de u* et des grandeurs turbulentes:

              if (isKeps)
                {
                  //tab1=tab_k_eps

                  //  calculer_local(u_plus_d_plus,d_visco,tab_k_eps,norm_v,dist,elem,num_face);
                  calculer_local(u_plus_d_plus,d_visco,tab1,norm_v,dist,elem,num_face);
                }
              else
                {
                  // tab1=tab_nu_t
                  // tab2=tab_k
                  //calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);
                  calculer_local(u_plus_d_plus,d_visco,tab1,tab2,norm_v,dist,elem,num_face);
                }
              // Calcul de la contrainte tangentielle

              double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

              if (ori == 0)
                {
                  Cisaillement_paroi_(num_face,1) = vit_frot*val[0]*signe;
                  if (dimension==3)
                    Cisaillement_paroi_(num_face,2) = vit_frot*val[1]*signe;
                  Cisaillement_paroi_(num_face,0) = 0.;
                }
              else if (ori == 1)
                {
                  Cisaillement_paroi_(num_face,0) = vit_frot*val[0]*signe;
                  if (dimension==3)
                    Cisaillement_paroi_(num_face,2) = vit_frot*val[1]*signe;
                  Cisaillement_paroi_(num_face,1) = 0.;
                }
              else
                {
                  Cisaillement_paroi_(num_face,0) = vit_frot*val[0]*signe;
                  Cisaillement_paroi_(num_face,1) = vit_frot*val[1]*signe;
                  Cisaillement_paroi_(num_face,2) = 0.;
                }

              // Calcul de u+ d+
              calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;
            }


        }
    }
  Cisaillement_paroi_.echange_espace_virtuel();
  tab1.echange_espace_virtuel();
  if (! isKeps) tab2.echange_espace_virtuel();
  //////////////////////////////////////////////////////


  /////////////////////////////////////////
  //YB:24/11/04
  ////////////////////////
  //Postraitement angles
  ////////////////////////
  /*  if (dimension==3)
      {
      const double& tps = eqn_hydr.schema_temps().temps_courant();
      const IntTab& elem_faces = zone_VDF.elem_faces();
      SFichier fic_angle("angles.dat",ios::app); // impression des faces parietales et leur coordonnees
      double angle_u;
      double angle_cis;
      double cos_angle_u;
      double cos_angle_cis;
      double sin_angle_u;
      double sin_angle_cis;
      double vit0, vit1;
      int elem_ndeb = 0;
      ndeb = 0;
      //La face ndeb correspond ici a la premiere face parietale du bas
      const DoubleTab& xp = zone_VDF.xp();

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
      {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
      {
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      ndeb = le_bord.num_premiere_face();

      if ((elem_ndeb = face_voisins(ndeb,0)) == -1)
      {
      elem_ndeb = face_voisins(ndeb,1);
      }

      int ori_ndeb=orientation(ndeb);

      if(xp(elem_ndeb,1)<1.) //Channel height=2
      {
      // 1er couple de faces perpendiculaires a la paroi
      int face1 = elem_faces(elem_ndeb ,(ori_ndeb+1));
      int face2 = elem_faces(elem_ndeb ,(ori_ndeb+4)%6);
      vit0 = 0.5*(vit(face1)+vit(face2));

      // 2e couple de faces perpendiculaires a la paroi
      int face3 = elem_faces(elem_ndeb ,(ori_ndeb+2));
      int face4 = elem_faces(elem_ndeb ,(ori_ndeb+5)%6);
      vit1 = 0.5*(vit(face3)+vit(face4));

      cos_angle_u = vit1/(sqrt(vit0*vit0+vit1*vit1));

      sin_angle_u = vit0/(sqrt(vit0*vit0+vit1*vit1));

      cos_angle_cis = Cisaillement_paroi_(ndeb,0)
      /sqrt(Cisaillement_paroi_(ndeb,0)*Cisaillement_paroi_(ndeb,0)
      +Cisaillement_paroi_(ndeb,2)*Cisaillement_paroi_(ndeb,2));

      sin_angle_cis = Cisaillement_paroi_(ndeb,2)
      /sqrt(Cisaillement_paroi_(ndeb,0)*Cisaillement_paroi_(ndeb,0)
      +Cisaillement_paroi_(ndeb,2)*Cisaillement_paroi_(ndeb,2));

      if(sin_angle_u > 0)
      angle_u = -acos(cos_angle_u);
      else
      angle_u = acos(cos_angle_u);

      if(sin_angle_cis > 0)
      angle_cis = -acos(cos_angle_cis);
      else
      angle_cis = acos(cos_angle_cis);

      fic_angle << tps << " " << angle_u << " " << angle_cis << finl;
      }
      }
      }
      }

      ////////////////////////////////////////////////////////////////////////
      */
  return 1;

}

// Calcul de u+ d+
void Paroi_std_hyd_VDF::calculer_uplus_dplus(DoubleVect& uplus, DoubleVect& dplus, DoubleVect& tab_ustar,
                                             int num_face, double dist, double d_visco, double norm_v)
{
  double ustar=tab_ustar(num_face);
  dplus(num_face)=ustar*dist/d_visco;
  if (ustar != 0)
    uplus(num_face)=norm_v/ustar;
}

int Paroi_std_hyd_VDF::calculer_local(double u_plus_d_plus,double d_visco,
                                      DoubleTab& k_eps,double norm_vit,
                                      double dist,int elem,int num_face)
{
  double valmin = table_hyd.val(5);
  double valmax = table_hyd.val(30);

  if (u_plus_d_plus <= valmin)
    {
      calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);

      calculer_sous_couche_visq(k_eps,dist,elem,d_visco,num_face);
    }

  else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
    {
      double d_plus;
      calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
      calculer_sous_couche_tampon(k_eps,d_visco,d_plus,elem,num_face);
    }

  else  // if (u_plus_d_plus >= valmax)
    {
      calculer_u_star_sous_couche_log(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_log(k_eps,dist,elem,num_face);
    }
  return 1;
}



int Paroi_std_hyd_VDF::calculer_local(double u_plus_d_plus,double d_visco,
                                      DoubleTab& tab_nu_t,DoubleTab& tab_k,double norm_vit,
                                      double dist,int elem,int num_face)
{
  double valmin = table_hyd.val(5);
  double valmax = table_hyd.val(30);

  if (u_plus_d_plus <= valmin)
    {
      calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_visq(tab_nu_t,tab_k,elem);
    }

  else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
    {
      double d_plus;
      calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
      calculer_sous_couche_tampon(tab_nu_t,tab_k,d_visco,d_plus,elem,num_face);
    }

  else if (u_plus_d_plus >= valmax)
    {
      calculer_u_star_sous_couche_log(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_log(tab_nu_t,tab_k,dist,elem,num_face);
    }
  return 1;
}

double Paroi_std_hyd_VDF::calculer_local(double u_plus_d_plus,double d_visco,double norm_vit,
                                         double dist, int& type_cou, double& d_plus)
{
  double valmin = table_hyd.val(5);
  double valmax = table_hyd.val(30);  // sous-couche tampon de 5 a 40
  double u_star;

  if (u_plus_d_plus <= valmin)
    {
      u_star = calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist);
      //Cerr << "Premier point dans le sous-couche visqueuse!! : y+=" << u_star*dist/d_visco << finl;
      type_cou = 0;
    }

  else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
    {
      u_star = calculer_u_star_sous_couche_tampon(u_plus_d_plus,d_visco,dist,d_plus);
      //Cerr << "Premier point dans le sous-couche tampon!! : y+=" << u_star*dist/d_visco  << finl;
      type_cou = 1;
    }

  else if (u_plus_d_plus >= valmax)
    {
      u_star = calculer_u_star(norm_vit,d_visco , dist);
      // Cerr << "Premier point dans le sous-couche log!! : y+=" << u_star*dist/d_visco << finl;
      type_cou = 2;

    }
  else
    {
      //bizarre
      u_star=0;
      assert(0);
      exit();
    }
  return u_star;
}


int Paroi_std_hyd_VDF::calculer_u_star_sous_couche_visq(double norm_vit,
                                                        double d_visco,double dist,
                                                        int face)
{
  // Dans la sous couche visqueuse:  u* = sqrt(u*nu/d)

  tab_u_star_(face) = sqrt(norm_vit*d_visco/dist);
  //  Cerr<<" USTA "<<face<<" "<< tab_u_star_(face)<<finl;
  return 1;
}


int Paroi_std_hyd_VDF::calculer_sous_couche_visq(DoubleTab& K_eps, double dist, int elem,double d_visco, int face )
{
  // Dans la sous couche visqueuse: k = eps = 0

  K_eps(elem,0) = 0.;
  K_eps(elem,1) = 0.;

  return 1;
  double u_star= tab_u_star(face);
  double d_plus=dist*u_star/d_visco;
  double u_star_carre = u_star*u_star;
  double C=0.08;
  double alpha=0.06;
  alpha=0.06550;
  //  f1=f2=fmu=1
  alpha=0.3;
  // f1=f2+1 fmu de LS
  alpha=0.05480;

  K_eps(elem,0) = u_star_carre*d_plus*d_plus*C;
  //K_eps(elem,1) = u_star_carre*u_star_carre/d_visco*(d_plus*0.005);
  K_eps(elem,1) = u_star_carre*u_star_carre/d_visco*(d_plus*d_plus*C)*alpha;
  Cerr<<" ici "<<  elem <<" " << K_eps(elem,0) <<" "<<K_eps(elem,1)<<finl;
  //assert(0);
  //exit();
  return 1;
}


int Paroi_std_hyd_VDF::calculer_sous_couche_visq(DoubleTab& nu_t,DoubleTab& k,int elem)
{
  // Dans la sous couche visqueuse: nu_t = k = 0
  nu_t(elem) = 0.;
  k(elem) = 0.;
  return 1;
}

double Paroi_std_hyd_VDF::calculer_u_star_sous_couche_visq(double norm_vit,
                                                           double d_visco,double dist)
{
  // Dans la sous couche visqueuse:  u* = sqrt(u*nu/d)
  return sqrt(norm_vit*d_visco/dist);
}

int Paroi_std_hyd_VDF::calculer_u_star_sous_couche_tampon(double& d_plus,double u_plus_d_plus,
                                                          double d_visco,double dist,int face)
{

  // Calcul de d_plus solution de table_hyd(inconnue) = u_plus_d_plus
  // puis calcul de u* dans la sous-couche tampon : u* = nu*d+/dist
  double d_plus_min = 5;
  double d_plus_max = 30;
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


int Paroi_std_hyd_VDF::calculer_sous_couche_tampon(DoubleTab& K_eps,double d_visco,
                                                   double d_plus,int elem,int face)
{

  // Calcul des grandeurs turbulentes a partir de d_plus et de u_star
  double u_star = tab_u_star(face);
  double lm_plus = calcul_lm_plus(d_plus);
  double  deriv = Fdypar_direct(lm_plus);
  double x= lm_plus*u_star*deriv;

  // Dans la sous couche tampon :
  //
  //  k = lm+ * u* * derivee(u+d+(d+))/sqrt(Cmu)
  //
  //              2
  //  eps = k * u* * derivee(u+d+(d+))*sqrt(Cmu)/nu
  //

  //   K_eps(elem,0) = max( K_eps(elem,0) , x*x/sqrt(Cmu) );

  //K_eps(elem,1) = max( K_eps(elem,1) , (K_eps(elem,0)*u_star*u_star*deriv)*sqrt(Cmu)/d_visco );


  K_eps(elem,0) = x*x/sqrt(Cmu) ;
  K_eps(elem,1) = (K_eps(elem,0)*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;

  return 1;
}


int Paroi_std_hyd_VDF::calculer_sous_couche_tampon(DoubleTab& nu_t,DoubleTab& tab_k,
                                                   double d_visco,double d_plus,
                                                   int elem,int face)
{
  // Calcul des grandeurs turbulentes a partir de d_plus et de u_star

  double u_star = tab_u_star(face);
  double lm_plus = calcul_lm_plus(d_plus);
  double deriv = Fdypar_direct(lm_plus);
  double x= lm_plus*u_star*deriv;

  // Dans la sous couche tampon :
  //
  //  k = lm+ * u* * derivee(u+d+(d+))/sqrt(Cmu)
  //
  //              2
  //  eps = k * u* * derivee(u+d+(d+))*sqrt(Cmu)/nu
  //
  //  nu_t = Cmu*k*k/eps
  //

  double k = x*x/sqrt(Cmu);
  double eps = (k*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;
  tab_k(elem) = k;
  nu_t(elem) = Cmu*k*k/eps;
  return 1;
}

double Paroi_std_hyd_VDF::calculer_u_star_sous_couche_tampon(double u_plus_d_plus,
                                                             double d_visco,double dist, double& d_plus)
{

  // Calcul de d_plus solution de table_hyd(inconnue) = u_plus_d_plus
  // puis calcul de u* dans la sous-couche tampon : u* = nu*d+/dist
  double d_plus_min = 5;
  double d_plus_max = 30;
  double valeur,u_star=0.;
  double epsilon = 1.e-12;
  double gauche = table_hyd.val(d_plus_min);
  double droite = table_hyd.val(d_plus_max);
  double deriv;
  int atteint = 2;

  if (gauche == droite)
    d_plus = d_plus_min;
  else
    {
      while(atteint!=1)
        {
          deriv = (droite-gauche)/(d_plus_max-d_plus_min);
          d_plus = d_plus_min + (u_plus_d_plus - gauche)/deriv;
          valeur = table_hyd.val(d_plus);
          if(dabs(valeur-u_plus_d_plus) < epsilon)
            {
              u_star = (d_visco*d_plus)/dist;
              atteint = 1;
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
  return u_star;

}

int Paroi_std_hyd_VDF::calculer_u_star_sous_couche_log(double norm_vit,double d_visco,
                                                       double dist, int face)
{
  // Dans la sous couche inertielle u* est solution d'une equation
  // differentielle resolue de maniere iterative

  const double Xpini = 30.;
  //  const double Xpini = 200.;
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


int Paroi_std_hyd_VDF::calculer_sous_couche_log(DoubleTab& K_eps,double dist,
                                                int elem,int face)
{


  // K et Eps sont donnes par les formules suivantes:
  //
  //          2                      3
  //    k = u*/sqrt(Cmu)  et eps = u* / Kd
  //

  double u_star= tab_u_star(face);
  double u_star_carre = u_star*u_star;

  K_eps(elem,0) = u_star_carre/sqrt(Cmu);
  K_eps(elem,1) = u_star_carre*u_star/(Kappa*dist);

  return 1;
}

int Paroi_std_hyd_VDF::calculer_sous_couche_log(DoubleTab& nu_t,DoubleTab& tab_k,
                                                double dist,int elem,int face)
{
  //  nu_t = Cmu*k*k/eps
  //
  //                      2                      3
  //  En utilisant  k = u*/sqrt(Cmu)  et eps = u* / Kd
  //
  //  on calcule nu_t en fonction de u*

  double u_star = tab_u_star(face);

  tab_k(elem) = u_star*u_star/sqrt(Cmu);
  nu_t(elem) = u_star*Kappa*dist;

  return 1;
}




void Paroi_std_hyd_VDF::imprimer_ustar(Sortie& os) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int ndeb,nfin;
  DoubleVect moy(4);
  moy=0.;
  double norme_L2=0.;
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
                  Ustar << "-------------------------------------------------------------------------------------------";
                  Ustar << "--------------------------------------------------------------------------------------------" << finl;
                  Ustar << "\tFace a\t\t\t\t|\t\t\t\t\t\t\t\t\t| TAU=Nu.Grad(Ut) [m2/s2]" << finl;
                  Ustar << "----------------------------------------|--------------------------------------------------";
                  Ustar << "---------------------|----------------------------------------------------------------------" << finl;
                  Ustar << "X\t\t| Y\t\t\t| u+\t\t\t| d+\t\t\t| u*\t\t\t| ||TAU||_2\t\t| |TAUx|\t\t| |TAUy|" << finl;
                  Ustar << "----------------|-----------------------|-----------------------|-----------------------|--";
                  Ustar << "---------------------|-----------------------|-----------------------|----------------------" << finl;
                }
              if (dimension == 3)
                {
                  Ustar << "-----------------------------------------------------------------------------------------------------------------";
                  Ustar << "-----------------------------------------------------------------------------------------------------------------" << finl;
                  Ustar << "\tFace a\t\t\t\t\t\t\t|\t\t\t\t\t\t\t\t\t| TAU=Nu.Grad(Ut) [m2/s2]" << finl;
                  Ustar << "----------------------------------------------------------------|------------------------------------------------";
                  Ustar << "-----------------------|-----------------------------------------------------------------------------------------" << finl;
                  Ustar << "X\t\t| Y\t\t\t| Z\t\t\t| u+\t\t\t| d+\t\t\t| u*\t\t\t| ||TAU||_2\t\t| |TAUx|\t\t| |TAUy|\t\t| |TAUz|" << finl;
                  Ustar << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|";
                  Ustar << "-----------------------|-----------------------|-----------------------|-----------------------|-----------------" << finl;
                }
            }
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              double x=zone_VDF.xv(num_face,0);
              double y=zone_VDF.xv(num_face,1);
              norme_L2= Cisaillement_paroi_(num_face,0)*Cisaillement_paroi_(num_face,0) + Cisaillement_paroi_(num_face,1)*Cisaillement_paroi_(num_face,1);
              if (dimension == 2)
                Ustar << x << "\t| " << y;
              if (dimension == 3)
                {
                  double z=zone_VDF.xv(num_face,2);
                  Ustar << x << "\t| " << y << "\t| " << z;
                  norme_L2+= Cisaillement_paroi_(num_face,2)*Cisaillement_paroi_(num_face,2);
                }
              norme_L2=sqrt(norme_L2);
              Ustar << "\t| " << uplus_(num_face) << "\t| " << dplus_(num_face) << "\t| " << tab_u_star(num_face);
              Ustar << "\t| " << norme_L2 << "\t| " << Cisaillement_paroi_(num_face,0) << "\t| " << Cisaillement_paroi_(num_face,1) ;
              if (dimension == 3)
                Ustar << "\t| " << Cisaillement_paroi_(num_face,2) << finl;
              else
                Ustar << finl;

              // PQ : 03/03 : Calcul des valeurs moyennes (en supposant maillage regulier)
              moy(0) +=uplus_(num_face);
              moy(1) +=dplus_(num_face);
              moy(2) +=tab_u_star(num_face);
              moy(3) +=1;
            }
          Ustar.syncfile();
        }
    }
  mp_sum_for_each_item(moy);
  if (moy(3) && je_suis_maitre())
    {
      Ustar << finl;
      Ustar << "-------------------------------------------------------------" << finl;
      Ustar << "Calcul des valeurs moyennes (en supposant maillage regulier):" << finl;
      Ustar << "<u+>= " << moy(0)/moy(3) << " <d+>= " << moy(1)/moy(3) << " <u*>= " << moy(2)/moy(3) << finl;
    }
  if(je_suis_maitre())
    Ustar << finl << finl;
  Ustar.syncfile();
}

//
// Calcule les vitesses moyennes aux premieres mailles le long des parois
// (norme des vitesses)
// LIMITATION au cas du CANAL, avec parois en y=0 et y=2
//
void Paroi_std_hyd_VDF::calculer_moyennes_parois(double& U_moy_1,
                                                 double& U_moy_2,
                                                 double& dist_1,
                                                 double& dist_2,
                                                 double& visco_1,
                                                 double& visco_2)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const DoubleTab& vitesse = eqn_hydr.inconnue().valeurs();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  const IntVect& orientation = zone_VDF.orientation();

//  double u_m1 = 0.; // moyenne des vitesses selon x, a la paroi 1 (y=0)
//  double u_m2 = 0.; // moyenne des vitesses selon x, a la paroi 2 (y=2)
//  double w_m1 = 0.; // moyenne des vitesses selon z, a la paroi 1 (y=0)
//  double w_m2 = 0.; // moyenne des vitesses selon z, a la paroi 2 (y=2)
//  int nb_1 = 0;
//  int nb_2 = 0;

  DoubleTrav values(5,2);
  values=0.;
  values(0,0) = visco_1;
  values(0,1) = visco_2;
  values(1,0) = dist_1;
  values(1,1) = dist_2;
// u_m1 -> values(2,0)  et   u_m2 -> values(2,1)
// w_m1 -> values(3,0)  et   w_m2 -> values(3,1)
// nb_1 -> values(4,0)  et   nb_2 -> values(4,1)

  int dir_par,dir1,dir2;
  int nb_front_Cl=zone_VDF.nb_front_Cl();
  double visco = -1;
  int l_unif = 0;

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    {
      if (tab_visco.local_min_vect()<DMINFLOAT)
        //   on ne doit pas changer tab_visco ici !
        {
          Cerr<<" visco <=0 ?"<<finl;
          exit();
        }
      //        tab_visco += DMINFLOAT;
    }

  for (int n_bord=0; n_bord<nb_front_Cl; n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois
      // Dans un premier temps on ne traite que les paroi_fixe,
      // qui correspondent a une des 2 parois du canal

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          int paroi;
          int num_elem;
          if (nfin==ndeb)
            {
              // cas ou le bord est nul
              dir_par=-1;
              dir1 =-1;
              dir2=-1;
            }
          else
            {
              dir_par = orientation(ndeb);
              // Recherche des deux autres orientations (pas dir_par)
              switch(dir_par)
                {
                case 0 :
                  {
                    dir1 = 1;
                    dir2 = 2;
                    break;
                  }
                case 1 :
                  {
                    dir1 = 0;
                    dir2 = 2;
                    break;
                  }
                case 2 :
                  {
                    dir1 = 0;
                    dir2 = 1;
                    break;
                  }
                default :
                  {
                    dir1=dir2=-1;
                    Cerr << que_suis_je() <<" Aborting during detection of wall orientation."<< finl;
                    Cerr<<"Please contact TRUST support."<<finl;
                    exit();
                  }
                }
            }

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              // Recherche de l'element associe a cette face de la bordure
              // Distinction entre la paroi basse et la paroi haute (specifique au CANAL)
              num_elem = face_voisins(num_face,0);
              paroi = 2;
              if (num_elem == -1)
                {
                  num_elem = face_voisins(num_face,1);  // Alors on est a la paroi basse!!!
                  paroi = 1;
                }

              // Calcul des composantes de la vitesse moyenne
              if (paroi == 1)
                {
                  values(2,0) += vitesse(elem_faces(num_elem, dir1));
                  values(3,0) += vitesse(elem_faces(num_elem, dir2));
                  values(1,0) += zone_VDF.dim_elem(num_elem, dir_par);
                  if (l_unif)
                    values(0,0) += visco;
                  else
                    values(0,0) += tab_visco(num_elem);
                  values(4,0)++;
                }
              else
                {
                  values(2,1) += vitesse(elem_faces(num_elem, dir1));
                  values(3,1) += vitesse(elem_faces(num_elem, dir2));
                  values(1,1) += zone_VDF.dim_elem(num_elem, dir_par);
                  if (l_unif)
                    values(0,1) += visco;
                  else
                    values(0,1) += tab_visco(num_elem);
                  values(4,1)++;
                }
            }
        }
    }

  values(1,0) *= 0.5;
  values(1,1)*= 0.5;
  // Somme les contributions de chaque processeur:
  mp_sum_for_each_item(values);
  U_moy_1 = sqrt( values(2,0) * values(2,0) + values(3,0) * values(3,0) ) / values(4,0);
  U_moy_2 = sqrt( values(2,1) * values(2,1) + values(3,1) * values(3,1) ) / values(4,1);
  dist_1 = values(1,0)/values(4,0);
  dist_2 = values(1,1)/values(4,1);
  visco_1 = values(0,0)/values(4,0);
  visco_2 = values(0,1)/values(4,1);

}

void Paroi_std_hyd_VDF::modifs_valeurs_turb(int num_elem,int type_cou, double u_star, double dist, double d_visco, double d_plus,DoubleTab& tab_nu_t,DoubleTab& tab_k)
{

  if (type_cou==0)
    {
      tab_nu_t(num_elem) = 0.;
      tab_k(num_elem) = 0.;
    }

  else if (type_cou==1)
    {
      double lm_plus = calcul_lm_plus(d_plus);
      double deriv = Fdypar_direct(lm_plus);
      double x= lm_plus*u_star*deriv;

      // Dans la sous couche tampon :
      //
      //  k = lm+ * u* * derivee(u+d+(d+))/sqrt(Cmu)
      //
      //              2
      //  eps = k * u* * derivee(u+d+(d+))*sqrt(Cmu)/nu
      //
      //  nu_t = Cmu*k*k/eps
      //

      double k = x*x/sqrt(Cmu);
      double eps = (k*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;
      tab_k(num_elem) = k;
      tab_nu_t(num_elem) = Cmu*k*k/eps;
    }
  else if (type_cou==2)
    {
      tab_k(num_elem) = u_star*u_star/sqrt(Cmu);
      tab_nu_t(num_elem) = u_star*Kappa*dist ;

    }
  else
    {
      Cerr << "Il y a un pbl : type_cou doit etre egal a 0, 1, ou 2!!" << finl;
      Cerr << "type_cou=" << type_cou << finl;
      exit();
    }
}

double Paroi_std_hyd_VDF::calculer_u_star(
  double& norm_vit,
  double& visco,
  double& dist
)
{
  Cerr << "On ne doit pas passer dans Paroi_std_hyd_VDF::calculer_u_star..." << finl;
  exit();
  return 1;
}

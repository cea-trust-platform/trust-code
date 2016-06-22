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
// File:        Paroi_loi_Ciofalo_hyd_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_loi_Ciofalo_hyd_VDF.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Turbulence_paroi_base.h>
#include <Equation_base.h>
#include <Mod_turb_hyd_base.h>

Implemente_instanciable_sans_constructeur(Paroi_loi_Ciofalo_hyd_VDF,"loi_Ciofalo_hydr_VDF",Paroi_std_hyd_VDF);

// PrintOn
Sortie& Paroi_loi_Ciofalo_hyd_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

// ReadOn
Entree& Paroi_loi_Ciofalo_hyd_VDF::readOn(Entree& s )
{
  Paroi_std_hyd_VDF::readOn(s);
  return s;
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_loi_Ciofalo_hyd_VDF
//
/////////////////////////////////////////////////////////////////////


// Remplissage de la table
int Paroi_loi_Ciofalo_hyd_VDF::init_lois_paroi_hydraulique()
{
  // Initialisations des constantes du modele :
  Y0=11.81 ;
  Cmu = mon_modele_turb_hyd->get_Cmu();
  Erugu =  ( exp ( Kappa * Y0) ) / Y0 ;
  return 1;
}

// On annule les valeurs des grandeurs turbulentes qui
// correspondent aux mailles de paroi
int Paroi_loi_Ciofalo_hyd_VDF::preparer_calcul_hyd(DoubleTab& tab)
{
  return 1;
}

// calculer_hyd pour le k-epsilon
int Paroi_loi_Ciofalo_hyd_VDF::calculer_hyd(DoubleTab& tab_k_eps)
{
  Cerr << " Paroi_loi_Ciofalo_hyd_VDF::calculer_hyd(DoubleTab& tab_k_eps) " << finl;
  Cerr <<  "on ne doit pas entrer dans cette methode" << finl;
  Cerr << " car elle est definie uniquement pour la LES " << finl ;
  return 1 ;


}

int Paroi_loi_Ciofalo_hyd_VDF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();

  // la vitesse resolue par l'equation hydr
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  double visco=-1.;

  // l_unif est le parametre de controle
  int l_unif;

  // test si la viscosite est un champ uniforme
  // l_unif=0 la viscosite n'est pas uniforme
  // l_unif=1 la viscosite est uniforme
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;


  // preparer_calcul_hyd(tab);

  // Cas ou la viscosite est nulle !!!!
  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //    tab_visco+=DMINFLOAT;

  int ndeb,nfin;
  int elem,ori;
  double norm_v;
  double dist;
  double u_plus_d_plus,d_visco;

  // valeurs qui vallent 0 si la norme de la vitesse est nulle
  double val,val1,val2;


  // Boucle sur les bords
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);


      //**********************
      // Condition Paroi_fixe
      //**********************

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          // Cas dimension 2
          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {

                //ori vaut 0,1 ou 2 (0=> x=Cte, 1=> y=Cte ...)
                ori = orientation(num_face);

                // face_voisin contient les deux elements voisins d'une face
                // 1er cas le bord est 'a droite' de la face de bord
                // 2e cas le bord est 'a gauche'
                if ( (elem =face_voisins(num_face,0)) != -1)
                  norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                  }

                // dist correspondt a la distance entre le centre de la cellule et le bord
                if (axi)
                  dist=zone_VDF.dist_norm_bord_axi(num_face);
                else
                  dist=zone_VDF.dist_norm_bord(num_face);

                // evaluation de la viscosite
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                //u+ d+ = (v d / nu)
                u_plus_d_plus = norm_v*dist/d_visco;

                // Calcul de u* et des grandeurs turbulentes
                // a partir de u+d+
                calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);
                // RQ : Pbls : signe negatif pour le cisaillement pour la paroi sup du canal
                Cerr << "ATTENTION!!! MODIFS FAITE POUR 3D ETENDUE A 2D MAIS NON VERIFIEE POUR 2D!!!!" << finl;
                int num_elem = zone_VDF.elem_faces(num_face,0);
                if (num_elem == -1)
                  num_elem = zone_VDF.elem_faces(num_face,1);
                //            double diff =  zone_VDF.xp(num_elem,ori)- zone_VDF.xv(num_face,ori);
                //              Cerr << "diff=" << diff << finl;
                //            double signe = diff / dabs(diff);
                //              Cerr << "signe=" << signe << finl;

                // Calcul de la contrainte tangentielle
                Cisaillement_paroi_(num_face,1-ori) = tab_u_star(num_face)*tab_u_star(num_face)*val;
              }

          // Cas dimension 3
          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {

                //ori vaut 0,1 ou 2 (0=> x=Cte, 1=> y=Cte ...)
                ori = orientation(num_face);

                // face_voisin contient les deux elements voisins d'une face
                if ( (elem = face_voisins(num_face,0)) != -1)
                  norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                  }

                // dist correspondt a la distance entre le centre de la cellule et le bord
                if (axi)
                  dist = zone_VDF.dist_norm_bord_axi(num_face);
                else
                  dist = zone_VDF.dist_norm_bord(num_face);

                // evaluation de la viscosite
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                //u+ d+ = (v d / nu)
                u_plus_d_plus = norm_v*dist/d_visco;

                // Calcul de u* et des grandeurs turbulentes:
                // a partir de u+d+
                calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                // Calcul des deux composantes de la contrainte tangentielle:
                double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);
                // RQ : Pbls : signe negatif pour le cisaillement pour la paroi sup du canal
                int num_elem = zone_VDF.elem_faces(num_face,0);
                if (num_elem == -1)
                  num_elem = zone_VDF.elem_faces(num_face,1);
                //              double diff =  zone_VDF.xp(num_elem,ori)- zone_VDF.xv(num_face,ori);
                //              Cerr << "diff=" << diff << finl;
                //              double signe = diff / dabs(diff);
                //              Cerr << "signe=" << signe << finl;

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
              }
        }


      //***************************
      // Condition Paroi_defilante
      //***************************

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

          // Cas dimension 2
          if (dimension == 2 )

            // On calcule au centre des mailles de bord
            // la vitesse parallele a l'axe de la face de bord
            //
            // On calcule la norme de cette vitesse

            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                //ori vaut 0,1 ou 2 (0=> x=Cte, 1=> y=Cte ...)
                ori = orientation(num_face);

                int rang = num_face-ndeb;

                // face_voisin contient les deux elements voisins d'une face
                if ( (elem = face_voisins(num_face,0)) != -1)
                  norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                     vitesse_imposee_face_bord(rang,1),val);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),val);
                  }

                // dist correspondt a la distance entre le centre de la cellule et le bord
                if (axi)
                  dist = zone_VDF.dist_norm_bord_axi(num_face);
                else
                  dist = zone_VDF.dist_norm_bord(num_face);

                // evaluation de la viscosite
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                //u+ d+ = (v d / nu)
                // a partir de u+d+
                u_plus_d_plus = norm_v*dist/d_visco;

                // Calcul de u* et des grandeurs turbulentes:
                calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                // Calcul de la contrainte tangentielle a la paroi:
                Cisaillement_paroi_(num_face,1-ori) = tab_u_star(num_face)*tab_u_star(num_face)*val;
              }

          // Cas dimension 3
          else if (dimension == 3)

            // On calcule au centre des mailles de bord
            // la vitesse dans le plan parallele a celui de la face de bord
            // On calcule la norme de cette vitesse

            for (int num_face=ndeb; num_face<nfin; num_face++)
              {

                //ori vaut 0,1 ou 2 (0=> x=Cte, 1=> y=Cte ...)
                ori = orientation(num_face);

                int rang = num_face-ndeb;

                // face_voisin contient les deux elements voisins d'une face
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

                // dist correspondt a la distance entre le centre de la cellule et le bord
                if (axi)
                  dist = zone_VDF.dist_norm_bord_axi(num_face);
                else
                  dist = zone_VDF.dist_norm_bord(num_face);

                // evaluation de la viscosite
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                //u+ d+ = (v d / nu)
                u_plus_d_plus = norm_v*dist/d_visco;

                // Calcul de u* et des grandeurs turbulentes:
                // a partir de u+d+
                calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);


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
              }

        }
    }
  Cisaillement_paroi_.echange_espace_virtuel();
  tab_nu_t.echange_espace_virtuel();
  tab_k.echange_espace_virtuel();
  return 1;
}

int Paroi_loi_Ciofalo_hyd_VDF::calculer_local(double u_plus_d_plus,double d_visco,
                                              DoubleTab& tab_nu_t,DoubleTab& tab_k,double norm_vit,
                                              double dist,int elem,int num_face)
{


  double valmin= Y0*Y0 ;
  // valeur de l'intensite turbulente
  double psi_E, psi_P ;
  // Epaisseur de la sous-couche visqueuse proportionnelle a l'intensite turbulente
  double Y_nu ;
  // constante recalculee pour la continuite de la vitesse
  double E_ ;
  double c=1./3. ;

  if ( tab_k(elem) <= 1.E-10 )
    {
      //  Cerr << "calcul normal:" << tab_k(elem) <<  finl ;

      if (u_plus_d_plus <= valmin)
        {
          calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);
          calculer_sous_couche_visq(tab_nu_t,tab_k,elem);
        }


      else
        {
          calculer_u_star_couche_log(norm_vit,d_visco,dist,num_face,Erugu);
          calculer_sous_couche_log(tab_nu_t,tab_k,dist,elem,num_face);
        }
    }


  else
    {

      //      Cerr << "calcul ciofalo" << tab_k(elem) << finl ;

      psi_P= sqrt( tab_k(elem) )/ norm_vit ;

      if (u_plus_d_plus <= valmin )
        psi_E=1/(Y0*pow (Cmu, 1./4.) ) ;

      else
        // on va calculer la valeur de psi_E pour une couche limite a l'equilibre
        // dans la zone log
        {

          const int itmax  = 25;
          const double seuil = 0.001;
          const double c1 = Kappa*norm_vit;
          const double c2 = log(Erugu*dist/d_visco);  // log = logarithme neperien
          double u_star,u_star1;
          double F,F_;

          // initialisation des variables
          int iter = 0;
          u_star1 = 1.;
          u_star= 2. ;

          // processus iteratif de calcul de u*
          while (( fabs((u_star-u_star1)/u_star1) > seuil ) && (iter < itmax ) )
            {
              u_star=u_star1 ;
              iter++ ;
              F= u_star*(log(u_star)+c2) -c1 ;
              F_= log(u_star)+1+c2 ;
              u_star1=u_star- F/F_ ;
            }

          if (iter >= itmax) erreur_non_convergence();

          // valeur de u_star
          u_star=u_star1;

          psi_E= Kappa / (log (Erugu*dist*u_star/d_visco)* pow (Cmu, 1./4.) ) ;

        }

      // calcul de Y_nu
      Y_nu = Y0*pow( (psi_P/psi_E) , -c );
      // calcul de la nouvelle constante pour la loi log
      E_ = ( exp ( Kappa * Y_nu) ) / Y_nu ;

      if (u_plus_d_plus <= Y_nu*Y_nu)
        {
          calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);
          calculer_sous_couche_visq(tab_nu_t,tab_k,elem);
        }

      else
        {

          calculer_u_star_couche_log(norm_vit,d_visco,dist,num_face,E_);
          calculer_sous_couche_log(tab_nu_t,tab_k,dist,elem,num_face);
        }


      // fin du else de test de la valeur de k
    }

  // test si on sort des limites de validite de la loi log
  if ( (dist*tab_u_star_(num_face)/d_visco) > 500. )
    {
      //          Cerr << "Probleme de TRUST dans Paroi_std_hyd_VDF::calculer_local() : " << finl;
      //          Cerr << "on sort du domaine de validite de la loi log" << finl;
      Cerr << "Y+=" << (dist*tab_u_star_(num_face)/d_visco) << finl;
    }

  return 1;
}


int Paroi_loi_Ciofalo_hyd_VDF::calculer_u_star_sous_couche_visq(double norm_vit,
                                                                double d_visco,double dist,
                                                                int face)
{
  // Dans la sous couche visqueuse:  u* = sqrt(u*nu/d)

  ////tab_u_star_(face) = sqrt(2*norm_vit*d_visco/dist);
  tab_u_star_(face) = sqrt(norm_vit*d_visco/dist);

  return 1;
}

int Paroi_loi_Ciofalo_hyd_VDF::calculer_u_star_couche_log(double norm_vit,double d_visco,
                                                          double dist, int face, double E_)
{
  // Dans la sous couche inertielle u* est solution d'une equation
  // differentielle resolue de maniere iterative

  const int itmax  = 25;
  const double seuil = 0.001;
  const double c1 = Kappa*norm_vit;
  const double c2 = log(E_*dist/d_visco);  // log = logarithme neperien
  double u_star,u_star1;
  double F,F_;

  int iter = 0;
  u_star1 = 1.;
  u_star= 2. ;

  while (( fabs((u_star-u_star1)/u_star1) > seuil ) && (iter < itmax ) )
    {
      u_star=u_star1 ;
      iter++ ;
      F= u_star*(log(u_star)+c2) -c1 ;
      F_= log(u_star)+1+c2 ;
      u_star1=u_star- F/F_ ;

    }

  if (iter >= itmax) erreur_non_convergence();

  tab_u_star_(face)= u_star1;



  return 1;
}




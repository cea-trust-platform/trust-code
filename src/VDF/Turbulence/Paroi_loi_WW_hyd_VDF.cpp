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
// File:        Paroi_loi_WW_hyd_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_loi_WW_hyd_VDF.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Equation_base.h>
#include <Mod_turb_hyd_base.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Paroi_loi_WW_hyd_VDF,"loi_WW_hydr_VDF",Paroi_std_hyd_VDF);

// PrintOn
Sortie& Paroi_loi_WW_hyd_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

// ReadOn
Entree& Paroi_loi_WW_hyd_VDF::readOn(Entree& is )
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" wall law"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is ;
}

void Paroi_loi_WW_hyd_VDF::set_param(Param& param)
{
  Paroi_std_hyd_VDF::set_param(param);
  param.ajouter_flag("impr",&impr);
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_loi_WW_hyd_VDF
//
/////////////////////////////////////////////////////////////////////

// Remplissage de la table
int Paroi_loi_WW_hyd_VDF::init_lois_paroi_hydraulique()
{
  Cmu = mon_modele_turb_hyd->get_Cmu();
  A= 8.3 ;
  B= 1./7. ;
  Y0= 11.81 ;
  return 1;
}

// On annule les valeurs des grandeurs turbulentes qui
// correspondent aux mailles de paroi
int Paroi_loi_WW_hyd_VDF::preparer_calcul_hyd(DoubleTab& tab)
{

  return 1;
}

// calculer_hyd pour le k-epsilon
int Paroi_loi_WW_hyd_VDF::calculer_hyd(DoubleTab& tab_k_eps)
{
  Cerr << " Paroi_loi_WW_hyd_VDF::calculer_hyd(DoubleTab& tab_k_eps) " << finl;
  Cerr <<  "on ne doit pas entrer dans cette methode" << finl;
  Cerr << " car elle est definie uniquement pour la LES " << finl ;
  return 1 ;
}

int Paroi_loi_WW_hyd_VDF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
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
  double visco=-1;

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


  // *************************
  // preparer_calcul_hyd(tab);
  // *************************


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
  double d_visco;

  double val,val1,val2;
  double signe;


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
                  {
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                    signe = -1.;
                  }
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                    signe = 1.;
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

                // Calcul de u* et des grandeurs turbulentes
                // a partir de norm_v
                calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);
                //Cerr << "ATTENTION!!! MODIFS FAITE POUR 3D ETENDUE A 2D MAIS NON VERIFIEE POUR 2D!!!!" << finl;

                int num_elem = zone_VDF.face_voisins(num_face,0);
                if (num_elem == -1)
                  num_elem = zone_VDF.face_voisins(num_face,1);


                // Calcul de la contrainte tangentielle
                Cisaillement_paroi_(num_face,1-ori) = signe*tab_u_star(num_face)*tab_u_star(num_face)*val;
                Cisaillement_paroi_(num_face,ori) = 0.;
              }

          // Cas dimension 3
          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                //ori vaut 0,1 ou 2 (0=> x=Cte, 1=> y=Cte ...)
                ori = orientation(num_face);

                // face_voisin contient les deux elements voisins d'une face
                if ( (elem = face_voisins(num_face,0)) != -1)
                  {
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                    signe = -1.; // on est a la paroi sup
                  }
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                    signe = 1.; // on est a la paroi inf
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


                // Calcul de u* et des grandeurs turbulentes:
                // a partir de de norm_v
                calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                // Calcul des deux composantes de la contrainte tangentielle:
                double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                if (ori == 0)
                  {
                    Cisaillement_paroi_(num_face,1) = signe*vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = signe*vit_frot*val2;
                    Cisaillement_paroi_(num_face,0) = 0.;
                  }
                else if (ori == 1)
                  {
                    Cisaillement_paroi_(num_face,0) = signe*vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = signe*vit_frot*val2;
                    Cisaillement_paroi_(num_face,1) = 0.;
                  }
                else
                  {
                    Cisaillement_paroi_(num_face,0) = signe*vit_frot*val1;
                    Cisaillement_paroi_(num_face,1) = signe*vit_frot*val2;
                    Cisaillement_paroi_(num_face,2) = 0.;
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
                  {
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),val);
                    signe = -1.; // on est a la paroi sup
                  }
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),val);
                    signe = 1.; // on est a la paroi inf
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


                // Calcul de u* et des grandeurs turbulentes:
                calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                // Calcul de la contrainte tangentielle a la paroi:
                Cisaillement_paroi_(num_face,1-ori) = signe*tab_u_star(num_face)*tab_u_star(num_face)*val;
                Cisaillement_paroi_(num_face,ori) = 0.;
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
                  {
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),
                                       vitesse_imposee_face_bord(rang,2),val1,val2);
                    signe = -1.; // on est a la paroi sup
                  }
                else
                  {
                    elem =  face_voisins(num_face,1);
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,vitesse_imposee_face_bord(rang,0),
                                       vitesse_imposee_face_bord(rang,1),
                                       vitesse_imposee_face_bord(rang,2),val1,val2);
                    signe = 1.; // on est a la paroi inf
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

                // Calcul de u* et des grandeurs turbulentes:
                // a partir de u+d+
                calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                // Calcul des deux composantes de la contrainte tangentielle:
                double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                if (ori == 0)
                  {
                    Cisaillement_paroi_(num_face,1) = signe*vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = signe*vit_frot*val2;
                    Cisaillement_paroi_(num_face,0) = 0.;
                  }
                else if (ori == 1)
                  {
                    Cisaillement_paroi_(num_face,0) = signe*vit_frot*val1;
                    Cisaillement_paroi_(num_face,2) = signe*vit_frot*val2;
                    Cisaillement_paroi_(num_face,1) = 0.;
                  }
                else
                  {
                    Cisaillement_paroi_(num_face,0) = signe*vit_frot*val1;
                    Cisaillement_paroi_(num_face,1) = signe*vit_frot*val2;
                    Cisaillement_paroi_(num_face,2) = 0.;
                  }
              }
        }
    }
  Cisaillement_paroi_.echange_espace_virtuel();
  tab_nu_t.echange_espace_virtuel();
  tab_k.echange_espace_virtuel();
  return 1;
}

int Paroi_loi_WW_hyd_VDF::calculer_local(double d_visco,
                                         DoubleTab& tab_nu_t,DoubleTab& tab_k,double norm_vit,
                                         double dist,int elem,int num_face)
{
  // C est la hauteur de la premiere maille en int qui doit etre testee ->> 2.*dist
  double up_lim = d_visco*Y0*Y0/(4.*dist);

  if (norm_vit <= up_lim)
    {
      calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_visq(tab_nu_t,tab_k,elem);
      if (impr==1) Cerr << "Zone lineaire" << finl;
    }
  else
    {
      calculer_u_star_couche_puissance(norm_vit,d_visco,dist,num_face);
      calculer_couche_puissance(tab_nu_t,tab_k,dist,elem,num_face);
      if (impr==1) Cerr << "Zone en puissance" << finl;
    }
  return 1;
}

int Paroi_loi_WW_hyd_VDF::calculer_u_star_couche_puissance(double norm_vit,double d_visco,
                                                           double dist, int face)
{
  double part1, part2 ;
  static double Apuiss = pow(A,(1+B)/(1-B));

  part1= ( (B+1) * pow(d_visco,B) * norm_vit ) / (A *pow(2.*dist,B) );
  part2= ( (1-B) * pow(d_visco,B+1) * Apuiss ) / (2 * pow(2.*dist,B+1) ) ;

  tab_u_star_(face)= pow(part1+part2, 1/(B+1) );

  return 1;
}


int Paroi_loi_WW_hyd_VDF::calculer_couche_puissance(DoubleTab& nu_t,DoubleTab& tab_k,
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
  nu_t(elem) = u_star*Kappa*dist ;

  return 1;
}



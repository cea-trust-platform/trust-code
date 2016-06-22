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
// File:        Paroi_loi_WW_hyd_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_loi_WW_hyd_VEF.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Domaine.h>
#include <Mod_turb_hyd_base.h>
#include <Equation_base.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Paroi_loi_WW_hyd_VEF,"loi_WW_hydr_VEF",Paroi_std_hyd_VEF);

// PrintOn
Sortie& Paroi_loi_WW_hyd_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

// ReadOn
Entree& Paroi_loi_WW_hyd_VEF::readOn(Entree& is )
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" wall law"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is ;
}

void Paroi_loi_WW_hyd_VEF::set_param(Param& param)
{
  Paroi_std_hyd_VEF::set_param(param);
  param.ajouter_flag("impr",&impr);
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_loi_WW_hyd_VEF
//
/////////////////////////////////////////////////////////////////////

// Remplissage de la table
int Paroi_loi_WW_hyd_VEF::init_lois_paroi_hydraulique()
{
  Cmu = mon_modele_turb_hyd->get_Cmu();
  A= 8.3 ;
  B= 1./7. ;
  Y0= 11.81 ;
  return 1;
}

// On annule les valeurs des grandeurs turbulentes qui
// correspondent aux mailles de paroi
int Paroi_loi_WW_hyd_VEF::preparer_calcul_hyd(DoubleTab& tab)
{
  return 1;
}

// calculer_hyd pour le k-epsilon
int Paroi_loi_WW_hyd_VEF::calculer_hyd(DoubleTab& tab_k_eps)
{
  Cerr << " Paroi_loi_WW_hyd_VEF::calculer_hyd(DoubleTab& tab_k_eps) " << finl;
  Cerr <<  "on ne doit pas entrer dans cette methode" << finl;
  Cerr << " car elle est definie uniquement pour la LES " << finl ;
  return 1 ;
}



int Paroi_loi_WW_hyd_VEF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs();
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
  //tab_visco+=DMINFLOAT;

  int ndeb,nfin;
  double norm_v,signe;
  double dist_G,dist_som;
  double d_visco;
  double val,val1,val2,val3;
  IntVect num(dimension);

  // Boucle sur les bords

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const IntTab& elem_faces = zone_VEF.elem_faces();
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                int elem = face_voisins(num_face,0);
                num[0]=elem_faces(elem,0);
                num[1]=elem_faces(elem,1);

                if (num[0]==num_face) num[0]=elem_faces(elem,2);
                else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                norm_v=norm_2D_vit(vit,num[0],num[1],num_face,zone_VEF,val,val1,val2);

                dist_G = distance_2D(num_face,elem,zone_VEF,signe);
                dist_som = (dimension+1)*dist_G;
                //                dist_som = distance_2D_som(num_face,elem,zone_VEF);

                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist_G,dist_som,elem,num_face);

                Cerr << "ATTENTION!!! MODIFS FAITE POUR 3D ETENDUE A 2D MAIS NON VERIFIEE POUR 2D!!!!" << finl;

                // Calcul de la contrainte tangentielle
                Cisaillement_paroi_(num_face,0) = tab_u_star(num_face)*tab_u_star(num_face)*val1;
                Cisaillement_paroi_(num_face,1) = tab_u_star(num_face)*tab_u_star(num_face)*val2;

                // Strategie de calcul de la loi de paroi sur tous
                // les elements ayant un sommet pres d'une paroi
                //                 for (int i=0;i<2;i++) {
                //                   int elem_voisin;
                //                   int num0=num[i];
                //                   if (face_voisins(num0,0)!=elem) elem_voisin=face_voisins(num0,0);
                //                   else elem_voisin=face_voisins(num0,1);
                //                   if (elem_voisin!=-1) {
                //                     int num1=elem_faces(elem_voisin,0);
                //                     int num2=elem_faces(elem_voisin,1);
                //                     if (num1==num0) num1=elem_faces(elem_voisin,2);
                //                     else if (num2==num0) num2=elem_faces(elem_voisin,2);
                //                     if (rang_elem_non_std(elem_voisin)==-1) {
                //                       // elem_voisin n'est pas un element de bord

                //                       norm_v=norm_2D_vit3(vit,num0,num1,num2,num_face,zone_VEF,val,val1,val2);

                //                       dist_G = distance_2D(num_face,elem,zone_VEF,signe);
                //                       dist_som = (dimension+1)*dist_G/dimension;  // cf 3D
                // //                      dist_som = distance_2D_som(num_face,elem,zone_VEF);

                //                       if (l_unif)
                //                         d_visco = visco;
                //                       else
                //                         d_visco = tab_visco[elem];

                //                       calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist_G,dist_som,elem,num_face);

                //                     }
                //                   }
                //                 }
                //                 // Fin de la strategie du calcul generalise de la loi de paroi

              }  // fin du for faces (2D)

          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                int elem = face_voisins(num_face,0);
                num[0] = elem_faces(elem,0);
                num[1] = elem_faces(elem,1);
                num[2] = elem_faces(elem,2);
                if (num[0]==num_face) num[0]=elem_faces(elem,3);
                else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                norm_v=norm_3D_vit(vit, num_face, num[0], num[1], num[2], zone_VEF, val1, val2, val3);

                dist_G = distance_3D(num_face,elem,zone_VEF,signe);
                dist_som = (dimension+1)*dist_G;
                //                dist_som = distance_3D_som(num_face,elem,zone_VEF);

                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                // Calcul de u* et des grandeurs turbulentes:
                // a partir de de norm_v
                calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist_G,dist_som,elem,num_face);

                // Calcul des deux composantes de la contrainte tangentielle:
                double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                // Signe du cisaillement : on le donne dans l OpDift


                Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                Cisaillement_paroi_(num_face,1) = vit_frot*val2;
                Cisaillement_paroi_(num_face,2) = vit_frot*val3;

                //                 // Strategie de calcul de la loi de paroi sur tous
                //                 // les elements ayant un sommet pres d'une paroi
                //                 for (int i=0;i<3;i++) {
                //                   int elem_voisin;
                //                   int num0=num[i];
                //                   if (face_voisins(num0,0)!=elem) elem_voisin=face_voisins(num0,0);
                //                   else elem_voisin=face_voisins(num0,1);
                //                   if (elem_voisin!=-1) {
                //                     int num1=elem_faces(elem_voisin,0);
                //                     int num2=elem_faces(elem_voisin,1);
                //                     int num3=elem_faces(elem_voisin,2);
                //                     if (num1==num0) num1=elem_faces(elem_voisin,3);
                //                     else if (num2==num0) num2=elem_faces(elem_voisin,3);
                //                     else if (num3==num0) num3=elem_faces(elem_voisin,3);
                //                     if (rang_elem_non_std(elem_voisin)==-1) {
                //                       // elem_voisin n'est pas un element de bord
                //                       norm_v=norm_3D_vit4(vit, num_face, num0, num1, num2, num3, zone_VEF, val1, val2, val3);
                //                       dist_G = distance_3D(num_face,elem_voisin,zone_VEF,signe);
                //                       dist_som = (dimension+1)*dist_G/dimension;
                //                       // Il faudrait chercher la distance entre la paroi et le centre de gravite de la face ne touchant pas la pario
                //                       // Premiere approx : dist_som_3.*dist_G
                // //                      dist_som = distance_3D_som(num_face,elem_voisin,zone_VEF);

                //                       if (l_unif)
                //                         d_visco = visco;
                //                       else
                //                         d_visco = tab_visco[elem_voisin];

                //                       calculer_local(d_visco,tab_nu_t,tab_k,norm_v,dist_G,dist_som,elem,num_face);

                //                     }
                //                   }
                //                 }
                // Fin de la strategie du calcul generalise de la loi de paroi
              }  // fin du for faces (3D)
        }  //  fin de Dirichlet_paroi_fixe


      // ATTENTION : CODER PAROI DEFILANTE
      else if (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
        {
          Cerr << "pour l instant Werner et Wengle n'est pas code pour la condition de paroi defilante!!" << finl;
        } // fin de Dirichlet_paroi_defilante
    }  // fin du for bord CL

  Cisaillement_paroi_.echange_espace_virtuel();
  tab_nu_t.echange_espace_virtuel();
  tab_k.echange_espace_virtuel();
  return 1;
}  // fin du calcul_hyd (nu-t)


int Paroi_loi_WW_hyd_VEF::calculer_local(double d_visco,
                                         DoubleTab& tab_nu_t,DoubleTab& tab_k,double norm_vit,
                                         double dist_G,double dist_som,int elem,int num_face)
{
  // C est la hauteur de la premiere maille en int qui doit etre testee ->> 2.*dist
  double up_lim = d_visco*Y0*Y0/(2.*dist_som);

  if (norm_vit <= up_lim)
    {
      calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist_G,num_face);
      //calculer_sous_couche_visq(tab_nu_t,tab_k,elem);
      tab_nu_t(elem) = 0.;
      tab_k(elem) = 0.;

      if (impr==1)  Cerr << "Zone lineaire" << finl;
    }
  else
    {
      calculer_u_star_couche_puissance(norm_vit,d_visco,dist_som,num_face);
      calculer_couche_puissance(tab_nu_t,tab_k,dist_som,elem,num_face);
      if (impr==1)  Cerr << "Zone en puissance" << finl;
    }
  return 1;
}


int Paroi_loi_WW_hyd_VEF::calculer_u_star_sous_couche_visq(double norm_vit,
                                                           double d_visco,double dist,
                                                           int face)
{
  // Dans la sous couche visqueuse:  u* = sqrt(u*nu/d)

  tab_u_star_(face) = sqrt(norm_vit*d_visco/dist);

  return 1;
}


int Paroi_loi_WW_hyd_VEF::calculer_u_star_couche_puissance(double norm_vit,double d_visco,
                                                           double dist, int face)
{
  double part1, part2 ;
  static double Apuiss = pow(A,(1+B)/(1-B));

  part1= ( (B+1) * pow(d_visco,B) * norm_vit ) / (A *pow(2.*dist,B) );
  part2= ( (1-B) * pow(d_visco,B+1) * Apuiss ) / (2 * pow(2.*dist,B+1) ) ;

  tab_u_star_(face)= pow(part1+part2, 1/(B+1) );

  return 1;
}


int Paroi_loi_WW_hyd_VEF::calculer_couche_puissance(DoubleTab& nu_t,DoubleTab& tab_k,
                                                    double dist,int elem,int face)
{
  //  nu_t = Cmu*k*k/eps
  //
  //                          2                       3
  //  En utilisant  k =     u*/sqrt(Cmu)  et eps = u* / Kd
  //
  //  on calcule nu_t en fonction de u*

  double u_star = tab_u_star(face);

  tab_k(elem) = u_star*u_star/sqrt(Cmu);
  nu_t(elem) =  u_star*Kappa*dist ;

  return 1;
}



////////////////////////////////////////////////////
//
// Fonctions utiles au calcul des lois de paroi:
//
////////////////////////////////////////////////////

double norm_2D_vit(const DoubleTab& vit,int num1,int num2,int fac,const Zone_VEF& zone,double& u,double& val1, double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  DoubleVect r(2);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  r/=norme_array(r);
  double v1=(vit(num1,0)+vit(num2,0))/3.;
  double v2=(vit(num1,1)+vit(num2,1))/3.;
  double v = sqrt( carre(v1) + carre(v2)
                   - carre(v1*r[0]+v2*r[1]) );

  val1 = v1/(v+DMINFLOAT);
  val2 = v2/(v+DMINFLOAT);

  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit3(const DoubleTab& vit,int num1,int num2,int num3,int fac,const Zone_VEF& zone,double& u,double& val1, double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  DoubleVect r(2);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  r/=norme_array(r);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0))/3.;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1))/3.;
  double v = sqrt( carre(v1) + carre(v2)
                   - carre(v1*r[0]+v2*r[1]) );

  val1 = v1/(v+DMINFLOAT);
  val2 = v2/(v+DMINFLOAT);

  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_3D_vit(const DoubleTab& vit,int fac,int num1,int num2,int num3,
                   const Zone_VEF& zone,
                   double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  DoubleVect r(3);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  r[2]=face_normale(fac,2);
  r/=norme_array(r);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0))/4.;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1))/4.;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2))/4.;
  double norm_vit = sqrt( carre(v1) + carre(v2) + carre(v3)
                          - carre(v1*r[0]+v2*r[1]+v3*r[2]) );
  val1 = v1/(norm_vit+DMINFLOAT);
  val2 = v2/(norm_vit+DMINFLOAT);
  val3 = v3/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_3D_vit4(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  DoubleVect r(3);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  r[2]=face_normale(fac,2);
  r/=norme_array(r);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0))/4.;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1))/4.;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2)+vit(num4,2))/4.;
  double norm_vit = sqrt( carre(v1) + carre(v2) + carre(v3)
                          - carre(v1*r[0]+v2*r[1]+v3*r[2]) );
  val1 = v1/(norm_vit+DMINFLOAT);
  val2 = v2/(norm_vit+DMINFLOAT);
  val3 = v3/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double distance_2D(int fac,int elem,const Zone_VEF& zone, double& signe) //distance centre de gravite -> face 2D
{
  const DoubleTab& xp = zone.xp();    // centre de gravite des elements
  const DoubleTab& xv = zone.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = zone.face_normales();

  DoubleVect r(2);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double x1=xp(elem,0);
  double y1=xp(elem,1);
  double x2=x0-x1;
  double y2=y0-y1;

  signe = x2*r[0]+y2*r[1];
  signe /= dabs(signe);

  return dabs(r[0]*(x1-x0)+r[1]*(y1-y0))/norme_array(r);
}

double distance_3D(int fac,int elem,const Zone_VEF& zone, double& signe) //distance centre de gravite -> face 2D
{
  const DoubleTab& xp = zone.xp();    // centre de gravite des elements
  const DoubleTab& xv = zone.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = zone.face_normales();

  DoubleVect r(3);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  r[2]=face_normale(fac,2);
  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double z0=xv(fac,2);
  double x1=xp(elem,0);
  double y1=xp(elem,1);
  double z1=xp(elem,2);

  return dabs(r[0]*(x1-x0)+r[1]*(y1-y0)+r[2]*(z1-z0))/norme_array(r);
}

double distance_2D_som(int fac,int elem,const Zone_VEF& zone ) //distance sommet -> face 2D
{
  const Zone& zone_geom = zone.zone();
  //  const IntTab& face_sommets = zone.face_sommets();
  const IntTab& elem_faces = zone.elem_faces();
  const Domaine& dom = zone_geom.domaine();
  const DoubleTab& coord = dom.coord_sommets();
  const Zone& zoneg = zone.zone();
  int nfac = zoneg.nb_faces_elem();
  const IntTab& som_elem=zone_geom.les_elems();


  const DoubleTab& xv = zone.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = zone.face_normales();

  int i,num0,sommet;
  IntVect num(2),som(2);
  // On cherche les faces autres que fac
  for (i=0; i<nfac; i++)
    {
      num0 = elem_faces(elem,i);
      if (num0 == fac)
        break;
    }
  sommet = som_elem(elem,i);   // On recupere le numero local de la face qui est egalement celui du sommet

  DoubleVect r(2);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double x1=coord(sommet,0);
  double y1=coord(sommet,1);

  return dabs(r[0]*(x1-x0)+r[1]*(y1-y0))/norme_array(r);
}

double distance_3D_som(int fac,int elem,const Zone_VEF& zone) //distance sommet -> face en 3D
{
  const Zone& zone_geom = zone.zone();
  const IntTab& face_sommets = zone.face_sommets();
  const IntTab& elem_faces = zone.elem_faces();
  const Domaine& dom = zone_geom.domaine();
  const DoubleTab& coord = dom.coord_sommets();
  const Zone& zoneg = zone.zone();
  int nfac = zoneg.nb_faces_elem();

  const DoubleTab& xv = zone.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = zone.face_normales();

  int i,j=0,num0,sommet=-1,k;
  IntVect num(3),som0(3),som1(3);
  // On cherche les faces autres que fac
  for (i=0; i<nfac; i++)
    {
      num0 = elem_faces(elem,i);
      if (num0 != fac)
        {
          num[j] = num0;
          j++;
        }
    }

  // On cherche le sommet commun aux faces autres que fac
  som0[0] = face_sommets(num[0],0);
  som0[1] = face_sommets(num[0],1);
  som0[2] = face_sommets(num[0],2);

  som1[0] = face_sommets(num[1],0);
  som1[1] = face_sommets(num[1],1);
  som1[2] = face_sommets(num[1],2);

  while(i<3)
    {
      for (j=0; j<3; j++)
        {
          if (face_sommets(num[1],i) == som0[j])
            {
              for (k=0; k<3; k++)
                {
                  if (face_sommets(num[2],k) == som0[j])
                    {
                      sommet = som0[j];
                      i=4;
                      k=3;
                      j=3;
                    }
                  else
                    k=3;
                }
            }
        }
    }

  DoubleVect r(3);
  r[0]=face_normale(fac,0);
  r[1]=face_normale(fac,1);
  r[2]=face_normale(fac,2);
  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double z0=xv(fac,2);
  double x1=coord(sommet,0);
  double y1=coord(sommet,1);
  double z1=coord(sommet,2);

  return dabs(r[0]*(x1-x0)+r[1]*(y1-y0)+r[2]*(z1-z0))/norme_array(r);
}

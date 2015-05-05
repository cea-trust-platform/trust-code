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
// File:        Modele_Jones_Launder_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_Jones_Launder_VEF.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Les_Cl.h>

Implemente_instanciable(Modele_Jones_Launder_VEF,"Modele_Jones_Launder_VEF",Modele_Fonc_Bas_Reynolds_Base);

// printOn et readOn

Sortie& Modele_Jones_Launder_VEF::printOn(Sortie& s ) const
{
  return s;
}

Entree& Modele_Jones_Launder_VEF::readOn(Entree& is )
{
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  if (motlu==accolade_ouverte)
    {
      is >> motlu;
      if (motlu != accolade_fermee)
        {
          Cerr << "Erreur a la lecture du Modele fonc bas reynolds Jones et Launder" << finl;
          Cerr << "On attendait } a la place de " << motlu << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Erreur a la lecture du Modele fonc bas reynolds Jones et Launder" << finl;
      Cerr << "On attendait { a la place de " << motlu << finl;
      exit();
    }
  return is;
}

Entree& Modele_Jones_Launder_VEF::lire(const Motcle& , Entree& is)
{
  return is;
}
///////////////////////////////////////////////////////////////
//   Implementation des fonctions de la classe
///////////////////////////////////////////////////////////////

void  Modele_Jones_Launder_VEF::associer(const Zone_dis& zone_dis,
                                         const Zone_Cl_dis& zone_Cl_dis)
{
  //  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  //  const Zone_Cl_VEF& la_zone_Cl = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
}

DoubleTab& Modele_Jones_Launder_VEF::Calcul_D(DoubleTab& D,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                              const DoubleTab& vitesse,const DoubleTab& K_eps_Bas_Re, double visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  //  const Zone_Cl_VEF& la_zone_Cl = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
  const DoubleVect& volumes = la_zone.volumes();
  //  int nb_faces = la_zone.nb_faces();
  int nb_faces_tot = la_zone.nb_faces();
  //  int nb_elem = la_zone.nb_elem();
  int nb_elem_tot = la_zone.nb_elem_tot();
  //  int nb_elem_tot = la_zone.nb_elem_tot();
  const Zone& zone=la_zone.zone();
  const IntTab& elem_faces = la_zone.elem_faces();
  const int nb_faces_elem = zone.nb_faces_elem();
  const DoubleTab& face_normales = la_zone.face_normales();
  const DoubleVect& vol_ent = la_zone.volumes_entrelaces();

  D = 0;
  int num_elem,i,face_loc,face_glob,num_face;
  double deriv,Vol;

  // Algo :
  //   * Boucle sur les elements
  //      * boucle locale dans l'element sur les faces -> calcul du gradient de racine de k
  //      * distribution de la valeur de l'integrale sur les faces de l'element
  //  Cela doit etre OK!!!
  // Rq : k et epsilon sont definis comme la vitesse P1NC.

  // ET le // ????? nb_elem ou nb_elem_tot
  // Pbls des C.L. en paroi???? non car on impose epsilon-D = 0!!
  for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
    {
      Vol = volumes(num_elem);
      for(i=0; i<dimension; i++)
        {
          deriv = 0;
          for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
            {
              face_glob = elem_faces(num_elem,face_loc);
              deriv += sqrt(K_eps_Bas_Re(face_glob,0))*face_normales(face_glob,i)*la_zone.oriente_normale(face_glob,num_elem);
            }
          deriv /= Vol;
          for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
            {
              face_glob = elem_faces(num_elem,face_loc);
              /*              Cerr<<"face_glob = "<<face_glob<<finl;
                              Cerr<<"K_eps_Bas_Re(face_glob,0) = "<<K_eps_Bas_Re(face_glob,0)<<finl;
                              Cerr<<"deriv = "<<deriv<<", Vol = "<<Vol<<", nb_faces_elem = "<<nb_faces_elem<<finl;*/
              D(face_glob) += deriv*deriv*Vol/nb_faces_elem;
            }
        }
    }

  for (num_face=0; num_face<nb_faces_tot; num_face++)
    D(num_face) *= -2.*visco/vol_ent(num_face);

  // RQ : il faut diviser par le volume entrelace car ce que nous calculons c est l integrale en V
  // Par contre dans les termes sources on multiplie par le volume entrelace daonc pour rester coherent avec le reste....
  return D;
}


// Fonction utile visc
// Pour le calcul de la derivee seconde
// 1/|v|*(Si,elem)ind_der*(Sj,elem)ind_der
/* pas appele
   static double viscA(const Zone_VEF& la_zone,int num_face,int num2,int dimension,
   int num_elem, int ind_der)
   {
   double pscal;

   pscal = la_zone.face_normales(num_face,ind_der)*la_zone.face_normales(num2,ind_der);

   if ( (la_zone.face_voisins(num_face,0) == la_zone.face_voisins(num2,0)) ||
   (la_zone.face_voisins(num_face,1) == la_zone.face_voisins(num2,1)))
   return -pscal/la_zone.volumes(num_elem);
   else
   return pscal/la_zone.volumes(num_elem);
   }
*/

DoubleTab& Modele_Jones_Launder_VEF::Calcul_E(DoubleTab& E,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& transporte,const DoubleTab& K_eps_Bas_Re,double visco, const DoubleTab& visco_turb ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
  const Zone_VEF& zone_VEF =  ref_cast(Zone_VEF,zone_dis.valeur());

  //  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleVect& volumes = zone_VEF.volumes();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  //  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  //  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  //  const Zone& zone = zone_VEF.zone();
  const int nb_faces = zone_VEF.nb_faces();
  int nb_faces_ = zone_VEF.nb_faces();
  //  int nb_faces_tot = zone_VEF.nb_faces_tot();
  //  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_elem = zone_VEF.nb_elem();
  //  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  //  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  int premiere_face_int = zone_VEF.premiere_face_int();

  int i,num_face;
  int elem,elem0;//alpha,beta,elem0;
  //  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  //  double val;

  int ncomp_ch_transporte;
  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);
  //  int ncomp_ch_transporte;
  //   if (vit.nb_dim() == 1)
  //     ncomp_ch_transporte=1;
  //   else
  //     ncomp_ch_transporte= vit.dimension(1);

  // Calcul de la derivee premiere de U
  // stokage d'un gradient_elem par element.
  DoubleTab gradient_elem(0, ncomp_ch_transporte, dimension);
  zone_VEF.zone().creer_tableau_elements(gradient_elem);

  // (du/dx du/dy dv/dx dv/dy) pour un poly
  DoubleTab gradient(0, ncomp_ch_transporte, dimension);
  zone_VEF.creer_tableau_faces(gradient);

  // (du/dx du/dy dv/dx dv/dy) pour une face
  gradient_elem=0.;

  //  DoubleTab deriv_seconde_elem(nb_elem,ncomp_ch_transporte,dimension);
  DoubleTab deriv_seconde_elem(gradient_elem);

  // (du/dx du/dy dv/dx dv/dy) pour un poly
  //  DoubleTab deriv_seconde(nb_faces_,ncomp_ch_transporte,dimension);
  DoubleTab deriv_seconde(gradient);
  // (du/dx du/dy dv/dx dv/dy) pour une face
  deriv_seconde_elem=0.;

  int fac=-1,elem1,elem2,comp;
  int n_bord;
  // On va d'abord calculer la derivee seconde de la vitesse du type : d^2(u_alpha)/d(x_beta)^2
  // DoubleTab deriv_seconde(nb_faces,dimension,dimension);
  // deriv_seconde =0.;
  //  Cerr << "debut E " << finl;
  // On traite les faces bord :

  //   for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
  //     {
  //       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
  //       const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  //       int ndeb = le_bord.num_premiere_face();
  //       int nfin = ndeb + le_bord.nb_faces();

  //       if (sub_type(Periodique,la_cl.valeur()))
  //         {
  //           const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
  //           int fac_asso;
  //           for (num_face=ndeb; num_face<nfin; num_face++)
  //             {
  //               fac_asso = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
  //               for (int k=0; k<2; k++)
  //                 {
  //                   elem = face_voisins(num_face,k);
  //                   for (i=0; i<nb_faces_elem; i++)
  //                     {
  //                       if ( ( (j= elem_faces(elem,i)) > num_face ) && (j != fac_asso) )
  //                         {

  //                           for (alpha=0; alpha<dimension; alpha++)
  //                             {
  //                               for (beta=0; beta<dimension; beta++)
  //                                 {
  //                                   if (alpha!=beta)
  //                                     {
  //                                       val = viscA(zone_VEF,num_face,j,dimension,elem,beta);

  //                                       deriv_seconde(num_face,alpha,beta)+=val*vit(j,alpha)/zone_VEF.volumes_entrelaces(num_face);
  //                                       deriv_seconde(num_face,alpha,beta)-=val*vit(num_face,alpha)/zone_VEF.volumes_entrelaces(num_face);

  //                                       deriv_seconde(j,alpha,beta)+=0.5*val*vit(num_face,alpha)/zone_VEF.volumes_entrelaces(j);
  //                                       deriv_seconde(j,alpha,beta)-=0.5*val*vit(j,alpha)/zone_VEF.volumes_entrelaces(j);
  //                                     }
  //                                 }
  //                             }
  //                         }
  //                     }
  //                 }
  //             }
  //         }
  //       else   //  conditions aux limites autres que periodicite
  //         {
  //           for (num_face=ndeb; num_face<nfin; num_face++)
  //             {
  //               elem = face_voisins(num_face,0);
  //               for (i=0; i<nb_faces_elem; i++)
  //                 {
  //                   if ( (j= elem_faces(elem,i)) > num_face )
  //                     {
  //                       for (alpha=0; alpha<dimension; alpha++)
  //                         {
  //                           for (beta=0; beta<dimension; beta++)
  //                             {
  //                               if (alpha!=beta)
  //                                 {
  //                                   val = viscA(zone_VEF,num_face,j,dimension,elem,beta);

  //                                   deriv_seconde(num_face,alpha,beta)+=val*vit(j,alpha)/zone_VEF.volumes_entrelaces(num_face);
  //                                   deriv_seconde(num_face,alpha,beta)-=val*vit(num_face,alpha)/zone_VEF.volumes_entrelaces(num_face);

  //                                   deriv_seconde(j,alpha,beta)+=val*vit(num_face,alpha)/zone_VEF.volumes_entrelaces(j);
  //                                   deriv_seconde(j,alpha,beta)-=val*vit(j,alpha)/zone_VEF.volumes_entrelaces(j);

  //                                   if ( deriv_seconde(num_face,0,1) > 1e-4 || deriv_seconde(j,0,1) > 1e-4  ) {
  //                                     Cerr << "FACE BORD" << finl;
  //                                     Cerr << "deriv_seconde(num_face,0,1) " << deriv_seconde(num_face,0,1) << finl;
  //                                     Cerr << "deriv_seconde(j,0,1) " << deriv_seconde(j,0,1) << finl;
  //                                     Cerr << " la vitesse avec num_face= " << num_face << " = " <<  vit(num_face,0) << finl;
  //                                     Cerr << " la vitesse avec j= " << j << " = " <<  vit(j,0)  << finl;
  //                                     Cerr << "zone_VEF.volumes_entrelaces(num_face) " << zone_VEF.volumes_entrelaces(num_face) << finl;
  //                                     Cerr << "zone_VEF.volumes_entrelaces(j) " << zone_VEF.volumes_entrelaces(j) << finl;
  //                                     Cerr << " val " << viscA(zone_VEF,num_face,j,dimension,elem,beta) << finl;
  //                                     Cerr << "face_normales(num_face,beta) " <<zone_VEF.face_normales(num_face,beta) << finl;
  //                                     Cerr << "face_normales(j,beta) " << zone_VEF.face_normales(j,beta) << finl;
  //                                     Cerr << "volumes(num_elem) " << zone_VEF.volumes(elem) << finl;
  //                                   }

  //                                 }
  //                             }
  //                         }
  //                     }
  //                 }
  //             }
  //         }
  //     }

  //   // On traite les faces internes
  //   // Calcul des derivees secondes pour alpha!=beta
  //   for (num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
  //     {
  //       for (int k=0; k<2; k++)
  //         {
  //           elem = face_voisins(num_face,k);
  //           for (i=0; i<nb_faces_elem; i++)
  //             {
  //               if ( (j= elem_faces(elem,i)) > num_face )
  //                 {
  //                   for (alpha=0; alpha<dimension; alpha++)
  //                     {
  //                       for (beta=0; beta<dimension; beta++)
  //                         {
  //                           if (alpha!=beta)
  //                             {
  //                               val = viscA(zone_VEF,num_face,j,dimension,elem,beta);

  //                               deriv_seconde(num_face,alpha,beta)+=val*vit(j,alpha)/zone_VEF.volumes_entrelaces(num_face);
  //                               deriv_seconde(num_face,alpha,beta)-=val*vit(num_face,alpha)/zone_VEF.volumes_entrelaces(num_face);

  //                               deriv_seconde(j,alpha,beta)+=val*vit(num_face,alpha)/zone_VEF.volumes_entrelaces(j);
  //                               deriv_seconde(j,alpha,beta)-=val*vit(j,alpha)/zone_VEF.volumes_entrelaces(j);

  //                               if ( deriv_seconde(num_face,0,1) > 1e-4 || deriv_seconde(j,0,1) > 1e-4 ) {
  //                                 Cerr << "FACE INTERNE" << finl;
  //                                 Cerr << "deriv_seconde(num_face,0,1) " << deriv_seconde(num_face,0,1) << finl;
  //                                 Cerr << "deriv_seconde(j,0,1) " << deriv_seconde(j,0,1) << finl;
  //                                 Cerr << " la vitesse avec num_face= " << num_face << " = " <<  vit(num_face,0) << " et " <<  vit(num_face,1) << finl;
  //                                 Cerr << " la vitesse avec j= " << j << " = " <<  vit(j,0) << " et " <<  vit(j,1) << finl;
  //                                 Cerr << "zone_VEF.volumes_entrelaces(num_face) " << zone_VEF.volumes_entrelaces(num_face) << finl;
  //                                 Cerr << "zone_VEF.volumes_entrelaces(j) " << zone_VEF.volumes_entrelaces(j) << finl;
  //                                 Cerr << " val " << viscA(zone_VEF,num_face,j,dimension,elem,beta) << finl;
  //                                     Cerr << "face_normales(num_face,beta) " <<zone_VEF.face_normales(num_face,beta) << finl;
  //                                     Cerr << "face_normales(j,beta) " << zone_VEF.face_normales(j,beta) << finl;
  //                                     Cerr << "volumes(num_elem) " << zone_VEF.volumes(elem) << finl;
  //                               }
  //                             }
  //                         }
  //                     }
  //                 }
  //             }
  //         }
  //     }

  // Prise en compte des conditions aux limites de Neumnan a la paroi
  //  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
  //     {
  //       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

  //       if (sub_type(Neumann_paroi,la_cl.valeur()))
  //         {
  //           const Neumann_paroi& la_cl_paroi = (Neumann_paroi&) la_cl.valeur();
  //           const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  //           int ndeb = le_bord.num_premiere_face();
  //           int nfin = ndeb + le_bord.nb_faces();
  //           for (int face=ndeb; face<nfin; face++)
  //             {
  //               resu(face,0) += la_cl_paroi.flux_impose(face-ndeb,0)*zone_VEF.surface(face);
  //               resu(face,1) += la_cl_paroi.flux_impose(face-ndeb,1)*zone_VEF.surface(face);
  //             }
  //         }
  //     }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////
  // Boucle sur les faces

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          //          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          // *0.5 car les faces perio sont parcourues deux fois
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1=face_voisins(num_face,0);
              elem2=face_voisins(num_face,1);
              if(ncomp_ch_transporte!=1)
                // correction ncomp_ch_transporte au lieu de dimension
                for (comp=0; comp<ncomp_ch_transporte; comp++)
                  for (i=0; i<dimension; i++)
                    {
                      gradient_elem(elem1, comp, i) +=
                        0.5*face_normales(num_face,i)*transporte(num_face,comp);
                      gradient_elem(elem2, comp, i) -=
                        0.5*face_normales(num_face,i)*transporte(num_face,comp);
                    }
              else
                for (i=0; i<dimension; i++)
                  {
                    gradient_elem(elem1, 0, i) +=
                      0.5*face_normales(fac,i)*transporte(fac);
                    gradient_elem(elem2, 0, i) +=
                      0.5*face_normales(fac,i)*transporte(fac);
                  }
            }
        }
      else
        {
          for (fac=num1; fac<num2; fac++)
            {
              elem1=face_voisins(fac,0);
              if(ncomp_ch_transporte==1)
                for (i=0; i<dimension; i++)
                  {
                    gradient_elem(elem1, 0, i) +=
                      face_normales(fac,i)*transporte(fac);
                  }
              else
                for (comp=0; comp<ncomp_ch_transporte; comp++)
                  {
                    // for (comp=0; comp<dimension; comp++)
                    // Attention le gradient est fonction du nombre de composant du champ transporte et nom de la dimension.
                    // pour la vitesse n_comp_transporte == diemnsion
                    // mais pour le k-eps n_comp = 2 en 0 on a k, en 1 on a eps.
                    // Cerr << "comp vaut " << comp << finl;
                    for (i=0; i<dimension; i++)
                      {
                        //  Cerr << "i vaut " << i << finl;
                        //                     Cerr << "face_normales(fac,i) " << face_normales(fac,i) << finl;
                        //                     Cerr << "transporte(fac,comp) " << transporte(fac,comp) << finl;
                        gradient_elem(elem1, comp, i) +=
                          face_normales(fac,i)* transporte(fac,comp);
                      }
                  }
            }
          // Fin pour les face reelles de bord
        }
    }

  for (fac=premiere_face_int; fac<nb_faces; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      if(ncomp_ch_transporte==1)
        for (i=0; i<dimension; i++)
          {
            gradient_elem(elem1, 0, i) +=
              face_normales(fac,i)*transporte(fac);
            if(elem2<nb_elem)
              gradient_elem(elem2, 0, i) -=
                face_normales(fac,i)*transporte(fac);
          }
      else
        {
          // correction ncomp_ch_transporte au lieu de dimension
          for (comp=0; comp<ncomp_ch_transporte; comp++)
            for (i=0; i<dimension; i++)
              {
                gradient_elem(elem1, comp, i) +=
                  face_normales(fac,i)*transporte(fac,comp);
                if(elem2<nb_elem)
                  gradient_elem(elem2, comp, i) -=
                    face_normales(fac,i)*transporte(fac,comp);
              }
        }
    }


  for ( elem=0; elem<nb_elem; elem++)
    for (comp=0; comp<ncomp_ch_transporte; comp++)
      for (i=0; i<dimension; i++)
        gradient_elem(elem,comp,i) /= volumes(elem);

  gradient_elem.echange_espace_virtuel();
  // On a les gradient_elem par elements

  // Boucle sur les faces
  //
  for (fac=0; fac< premiere_face_int; fac++)
    {
      for (comp=0; comp<ncomp_ch_transporte; comp++)
        for (i=0; i<dimension; i++)
          {
            elem1=face_voisins(fac,0);
            if (elem1 != -1)
              gradient(fac, comp, i) = gradient_elem(elem1, comp, i);
            else
              {
                elem1=face_voisins(fac,1);
                gradient(fac, comp, i) = gradient_elem(elem1, comp, i);
              }
          }

    } // fin du for faces

  for (; fac<nb_faces_; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      double vol1=volumes(elem1);
      double vol2=volumes(elem2);
      double voltot=vol1+vol2;
      for (comp=0; comp<ncomp_ch_transporte; comp++)
        for (i=0; i<dimension; i++)
          {
            double grad1=gradient_elem(elem1, comp, i);
            double grad2=gradient_elem(elem2, comp, i);
            gradient(fac, comp, i) =  (vol1*grad1 + vol2*grad2)/voltot;
          }
    } // fin du for faces

  gradient.echange_espace_virtuel();

  // maintenant on calcul la derivee seconde
  // stokage d'un gradient_elem par element.

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////
  // Boucle sur les faces
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          //          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          // *0.5 car les faces perio sont parcourues deux fois
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1=face_voisins(num_face,0);
              elem2=face_voisins(num_face,1);
              if(ncomp_ch_transporte!=1)
                // correction ncomp_ch_transporte au lieu de dimension
                for (comp=0; comp<ncomp_ch_transporte; comp++)
                  for (i=0; i<dimension; i++)
                    {
                      deriv_seconde_elem(elem1, comp, i) +=
                        0.5*face_normales(num_face,i)*gradient(num_face,comp,i);
                      deriv_seconde_elem(elem2, comp, i) -=
                        0.5*face_normales(num_face,i)*gradient(num_face,comp,i);
                    }
              else
                for (i=0; i<dimension; i++)
                  {
                    deriv_seconde_elem(elem1, 0, i) +=
                      0.5*face_normales(fac,i)*gradient(num_face,0,i);
                    deriv_seconde_elem(elem2, 0, i) +=
                      0.5*face_normales(fac,i)*gradient(num_face,0,i);
                  }
            }
        }
      else
        for (fac=num1; fac<num2; fac++)
          {
            elem1=face_voisins(fac,0);
            if(ncomp_ch_transporte==1)
              for (i=0; i<dimension; i++)
                {
                  deriv_seconde_elem(elem1, 0, i) +=
                    face_normales(fac,i)*gradient(fac,0,i);
                }
            else
              for (comp=0; comp<ncomp_ch_transporte; comp++)
                {
                  for (i=0; i<dimension; i++)
                    {
                      deriv_seconde_elem(elem1, comp, i) +=
                        face_normales(fac,i)*gradient(fac,comp,i);
                    }
                }
          }
    }

  for (fac=premiere_face_int; fac<nb_faces; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      if(ncomp_ch_transporte==1)
        for (i=0; i<dimension; i++)
          {
            deriv_seconde_elem(elem1, 0, i) +=
              face_normales(fac,i)*gradient(fac,0,i);
            if(elem2<nb_elem)
              deriv_seconde_elem(elem2, 0, i) -=
                face_normales(fac,i)*gradient(fac,0,i);
          }
      else
        {
          // correction ncomp_ch_transporte au lieu de dimension
          for (comp=0; comp<ncomp_ch_transporte; comp++)
            for (i=0; i<dimension; i++)
              {
                deriv_seconde_elem(elem1, comp, i) +=
                  face_normales(fac,i)*gradient(fac,comp,i);
                if(elem2<nb_elem)
                  deriv_seconde_elem(elem2, comp, i) -=
                    face_normales(fac,i)*gradient(fac,comp,i);
              }
        }
    }

  for ( elem=0; elem<nb_elem; elem++)
    for (comp=0; comp<ncomp_ch_transporte; comp++)
      for (i=0; i<dimension; i++)
        deriv_seconde_elem(elem,comp,i) /= volumes(elem);

  deriv_seconde_elem.echange_espace_virtuel();
  // On a les gradient_elem par elements

  // Boucle sur les faces
  //

  for (fac=0; fac< premiere_face_int; fac++)
    {
      for (comp=0; comp<ncomp_ch_transporte; comp++)
        for (i=0; i<dimension; i++)
          {
            elem1=face_voisins(fac,0);
            if (elem1 != -1)
              deriv_seconde(fac, comp, i) = deriv_seconde_elem(elem1, comp, i);
            else
              {
                elem1=face_voisins(fac,1);
                deriv_seconde(fac, comp, i) = deriv_seconde_elem(elem1, comp, i);
              }
          }

    } // fin du for faces

  for (; fac<nb_faces_; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      double vol1=volumes(elem1);
      double vol2=volumes(elem2);
      double voltot=vol1+vol2;
      for (comp=0; comp<ncomp_ch_transporte; comp++)
        for (i=0; i<dimension; i++)
          {
            double grad1=deriv_seconde_elem(elem1, comp, i);
            double grad2=deriv_seconde_elem(elem2, comp, i);
            deriv_seconde(fac, comp, i) = (vol1*grad1 + vol2*grad2)/voltot;
          }
    } // fin du for faces

  // Calcul de E
  double deriv,deriv1,nuturb;
  for (num_face=0; num_face<nb_faces; num_face++)
    {
      deriv = 0;
      if (dimension == 2)
        {
          deriv = deriv_seconde(num_face,0,1)*deriv_seconde(num_face,0,1)+deriv_seconde(num_face,1,0)*deriv_seconde(num_face,1,0) + deriv_seconde(num_face,0,0)*deriv_seconde(num_face,0,0)+deriv_seconde(num_face,1,1)*deriv_seconde(num_face,1,1);
        }
      else
        {
          deriv1 = deriv_seconde(num_face,1,0)+deriv_seconde(num_face,2,0);
          deriv1 *= deriv1;
          deriv += deriv1;

          deriv1 = deriv_seconde(num_face,0,1)+deriv_seconde(num_face,2,1);
          deriv1 *= deriv1;
          deriv += deriv1;

          deriv1 = deriv_seconde(num_face,0,2)+deriv_seconde(num_face,1,2);
          deriv1 *= deriv1;
          deriv += deriv1;
        }
      elem0 = face_voisins(num_face,0);
      elem1 = face_voisins(num_face,1);

      if (elem1!=-1)
        {
          nuturb = visco_turb(elem0)*volumes(elem0)+visco_turb(elem1)*volumes(elem1);
          nuturb /= volumes(elem0) + volumes(elem1);
        }
      else
        nuturb =  visco_turb(elem0);

      E(num_face) = 2.*visco*deriv*nuturb;

      //   Cerr << "nuturb " << nuturb << finl;
      //       Cerr << "visco  " << visco << finl;
      //       Cerr << "deriv_seconde(" << num_face << ",0,1)" << deriv_seconde(num_face,0,1) << finl;
      //  if ( deriv_seconde(num_face,0,1) > 100) {
      //         Cerr << " la vitesse " << vit(num_face,0) << " et " <<  vit(num_face,1) << finl;
      //       }
      //       Cerr << "Les valeurs de E(" << num_face << ") " << E(num_face) << finl;
    }
  return E;
}

DoubleTab& Modele_Jones_Launder_VEF::Calcul_F1( DoubleTab& F1, const Zone_dis& zone_dis) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  for (int num_face=0; num_face <nb_faces; num_face ++ )
    F1[num_face] = 1.;
  return F1;
}

DoubleTab& Modele_Jones_Launder_VEF::Calcul_F2( DoubleTab& F2, DoubleTab& D, const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,double visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  int num_face;
  double Re;

  for (num_face=0; num_face<nb_faces  ; num_face++)
    {
      if (K_eps_Bas_Re(num_face,1)>0)
        {
          Re = (K_eps_Bas_Re(num_face,0)*K_eps_Bas_Re(num_face,0))/(visco*K_eps_Bas_Re(num_face,1));
          F2[num_face] = 1. - (0.3*exp(-Re*Re));
        }
      else
        {
          F2[num_face] = 1.;
        }
    }
  return F2;
}
DoubleTab& Modele_Jones_Launder_VEF::Calcul_F2( DoubleTab& F2, DoubleTab& D, const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re, const DoubleTab& tab_visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  int num_face,elem0,elem1;
  double Re,nulam;

  for (num_face=0; num_face<nb_faces  ; num_face++)
    {
      elem0 = la_zone.face_voisins(num_face,0);
      elem1 = la_zone.face_voisins(num_face,1);
      if (elem1!=-1)
        {
          nulam = tab_visco(elem0)*la_zone.volumes(elem0)+tab_visco(elem1)*la_zone.volumes(elem1);
          nulam /= la_zone.volumes(elem0) + la_zone.volumes(elem1);
        }
      else
        nulam =  tab_visco(elem0);

      if (K_eps_Bas_Re(num_face,1)>0)
        {
          Re = (K_eps_Bas_Re(num_face,0)*K_eps_Bas_Re(num_face,0))/(nulam*K_eps_Bas_Re(num_face,1));
          F2[num_face] = 1. - (0.3*exp(-Re*Re));
        }
      else
        {
          F2[num_face] = 1.;
        }
    }
  return F2;
}


DoubleTab&  Modele_Jones_Launder_VEF::Calcul_Fmu( DoubleTab& Fmu,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,double visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  int num_face;
  double Re;
  Fmu = 0;

  for (num_face=0; num_face<nb_faces  ; num_face++)
    {
      if (K_eps_Bas_Re(num_face,1)>0)
        {
          Re = (K_eps_Bas_Re(num_face,0)*K_eps_Bas_Re(num_face,0))/(visco*K_eps_Bas_Re(num_face,1));
          Fmu[num_face] = exp(-2.5/(1.+Re/50));
        }
      else
        {
          Fmu[num_face] = 0.;
        }
    }
  return Fmu;
}


DoubleTab&  Modele_Jones_Launder_VEF::Calcul_Fmu( DoubleTab& Fmu,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,const DoubleTab& tab_visco ) const
{
  const Zone_VEF& la_zone = ref_cast(Zone_VEF,zone_dis.valeur());
  int nb_faces = la_zone.nb_faces();
  int num_face,elem0,elem1;
  double Re,nulam;
  Fmu = 0;

  for (num_face=0; num_face<nb_faces  ; num_face++)
    {
      elem0 = la_zone.face_voisins(num_face,0);
      elem1 = la_zone.face_voisins(num_face,1);
      if (elem1!=-1)
        {
          nulam = tab_visco(elem0)*la_zone.volumes(elem0)+tab_visco(elem1)*la_zone.volumes(elem1);
          nulam /= la_zone.volumes(elem0) + la_zone.volumes(elem1);
        }
      else
        nulam =  tab_visco(elem0);

      if (K_eps_Bas_Re(num_face,1)>0)
        {
          Re = (K_eps_Bas_Re(num_face,0)*K_eps_Bas_Re(num_face,0))/(nulam*K_eps_Bas_Re(num_face,1));
          Fmu[num_face] = exp(-2.5/(1.+Re/50.));
        }
      else
        {
          Fmu[num_face] = 0.;
        }
    }

  return Fmu;
}

void  Modele_Jones_Launder_VEF::mettre_a_jour(double temps)
{
  ;
}

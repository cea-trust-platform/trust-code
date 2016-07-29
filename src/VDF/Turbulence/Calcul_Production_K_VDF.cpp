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
// File:        Calcul_Production_K_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Calcul_Production_K_VDF.h>
#include <Transport_K_Eps.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Probleme_base.h>
#include <IntTrav.h>
#include <Entree_fluide_temperature_imposee.h>
#include <Entree_fluide_concentration_imposee.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Champ_Face.h>
#include <Zone_Cl_VDF.h>
#include <Fluide_Quasi_Compressible.h>
#include <Debog.h>
#include <DoubleTrav.h>

////////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Calcul_Production_K_VDF
//
/////////////////////////////////////////////////////////////////////////////
DoubleVect& Calcul_Production_K_VDF::
calculer_terme_production_K(const Zone_VDF& zone_VDF, const Zone_Cl_VDF& zone_Cl_VDF , DoubleVect& S,  const DoubleTab& K_eps,
                            const DoubleTab& vitesse,const Champ_Face& vit,  const DoubleTab& visco_turb )  const
{
  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntVect& orientation = zone_VDF.orientation();

  int elem;
  IntVect element(4);

  S = 0. ;

  // Apres la factorisation du calcul de Sij*Sij (v1.4.6) il est apparu que pour
  // le terme de production de K la contribution des parois ne doit pas etre
  // pris en compte, car K et Eps sont ensuite imposes par la loi de paroi,
  // Si l'on prend en compte la contribution de la paroi les viscosites
  // turbulentes a la paroi sont tres fortes, et les pas de temps tres faibles
  // Pour la v1.4.8, on revient a la modelisation de la 1.4.5 qui ne
  // n'ajoutait pas a Sij*Sij la contribution des parois
  //Dans la methode calcul_S_barre_sans_contrib_paroi, contribution_paroi
  //est fixe a 0 par defaut

  vit.calcul_S_barre_sans_contrib_paroi(vitesse,S,zone_Cl_VDF);

  double coef;

  Debog::verifier("Source_Transport_K_Eps_VDF_P0_VDF:: calculer_terme_production_K ",S);
  for ( elem=0; elem<nb_elem; elem++)
    {

      //P= 2*nut*Sij*Sij
      S(elem) *= visco_turb(elem) ;
      coef = 0. ;
      for (int i=0; i<Objet_U::dimension; i++)
        {
          coef +=  (vitesse[elem_faces(elem,i)]  - vitesse[elem_faces(elem,i+Objet_U::dimension)]) /zone_VDF.dim_elem(elem,orientation(elem_faces(elem,i)));
        }
      //Corrections pour prendre en compte la divergence de u
      //non nulle en Quasi-Compressible

      S(elem) += -(2./3.)*visco_turb(elem)*coef * coef;
      S(elem) += -(2./3.)*K_eps(elem,0) * coef ;

    }

  Debog::verifier("Source_Transport_K_Eps_VDF_P0_VDF:: calculer_terme_production_K ",S);
  return S ;
}

DoubleVect& Calcul_Production_K_VDF::
calculer_terme_production_K_Axi(const Zone_VDF& zone_VDF,
                                const Champ_Face& vitesse,
                                DoubleVect& P,
                                const DoubleTab& K_Eps,
                                const DoubleTab& visco_turb) const
{
  // P est discretise comme K et Eps i.e au centre des elements
  //
  // P(elem) = - (    Rey11*tau11 + Rey22*tau22 + Rey33*tau33
  //                    +  Rey12*(tau12+tau21) + Rey13*(tau13 + tau31)
  //                    +  Rey23*(tau23+tau32) )
  //
  //

  P= 0;

  int nb_elem = zone_VDF.nb_elem();
  int nb_aretes = zone_VDF.nb_aretes();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& Qdm = zone_VDF.Qdm();
  const DoubleTab& tau_diag = vitesse.tau_diag();
  const DoubleTab& tau_croises = vitesse.tau_croises();
  double d_visco_turb,k_elem,P_arete;

  // Calcul des composantes du tenseur de Reynolds a partir du tenseur GradU

  DoubleTrav Rey_diag(nb_elem,Objet_U::dimension);
  DoubleTrav Rey_croises(nb_aretes);

  Rey_croises = 0;
  int num_elem;
  for (num_elem=0; num_elem<nb_elem; num_elem++)
    {
      k_elem = K_Eps(num_elem,0);
      for (int i=0; i<Objet_U::dimension; i++)
        Rey_diag(num_elem,i) = -2*visco_turb(num_elem)*tau_diag(num_elem,i)
                               + 2./3.*k_elem;
    }

  // Boucle sur les aretes bord

  int ndeb,nfin,n_arete;
  ndeb = zone_VDF.premiere_arete_bord();
  nfin = ndeb + zone_VDF.nb_aretes_bord();
  int fac1,fac2,fac3,fac4;

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      fac3 = Qdm(n_arete,2);

      d_visco_turb = 0.5*(visco_turb(face_voisins(fac3,0))
                          + visco_turb(face_voisins(fac3,1)));
      Rey_croises(n_arete) = - d_visco_turb*(tau_croises(n_arete,0) + tau_croises(n_arete,1));

    }

  // Pas de boucle sur les aretes mixtes: les composantes croisees
  // du tenseur de Reynolds sont prises nulles sur les aretes mixtes

  // Boucle sur les aretes internes

  ndeb = zone_VDF.premiere_arete_interne();
  nfin = zone_VDF.nb_aretes();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);
      fac3=Qdm(n_arete,2);
      fac4=Qdm(n_arete,3);

      d_visco_turb = 0.25*(visco_turb(face_voisins(fac3,0)) + visco_turb(face_voisins(fac3,1))
                           + visco_turb(face_voisins(fac4,0)) + visco_turb(face_voisins(fac4,1)));
      Rey_croises(n_arete) = - d_visco_turb*(tau_croises(n_arete,0) + tau_croises(n_arete,1));
    }

  // Calcul du terme de production P

  for (num_elem=0; num_elem<nb_elem; num_elem++)
    {
      for (int i=0; i<Objet_U::dimension; i++)
        P(num_elem) -= Rey_diag(num_elem,i)*tau_diag(num_elem,i);
    }

  ndeb = zone_VDF.premiere_arete_bord();
  nfin = ndeb + zone_VDF.nb_aretes_bord();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      fac3=Qdm(n_arete,2);

      P_arete = - Rey_croises(n_arete)*(tau_croises(n_arete,0)+tau_croises(n_arete,1));
      P[face_voisins(fac3,0)] += 0.25*P_arete;
      P[face_voisins(fac3,1)] += 0.25*P_arete;
    }

  ndeb = zone_VDF.premiere_arete_interne();
  nfin = zone_VDF.nb_aretes();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);

      P_arete = - Rey_croises(n_arete)*(tau_croises(n_arete,0)+tau_croises(n_arete,1));
      P[face_voisins(fac1,0)] += 0.25*P_arete;
      P[face_voisins(fac1,1)] += 0.25*P_arete;
      P[face_voisins(fac2,0)] += 0.25*P_arete;
      P[face_voisins(fac2,1)] += 0.25*P_arete;
    }
  P.echange_espace_virtuel();
  return P;
}


DoubleTab& Calcul_Production_K_VDF::calculer_u_teta(const Zone_VDF& zone_VDF,
                                                    const Zone_Cl_VDF& zcl_VDF,
                                                    const DoubleTab& temper,
                                                    const DoubleTab& alpha_turb,
                                                    DoubleTab& u_teta) const
{
  //                                      ---->
  // On note u_teta le vecteur alpha_turb.gradT
  //
  // Sur chaque face on calcule la composante de u_teta normale a la face

  int nb_faces= zone_VDF.nb_faces();
  int n0,n1;
  double alpha,dist;
  int face;
  const IntTab& face_voisins = zone_VDF.face_voisins();
  u_teta = 0;

  // Traitement des faces internes


  int premiere_face_int=zone_VDF.premiere_face_int();
  nb_faces=zone_VDF.nb_faces();

  if (Objet_U::axi)

    for (face=premiere_face_int; face<nb_faces; face++)
      {
        n0 = face_voisins(face,0);
        n1 = face_voisins(face,1);
        dist = zone_VDF.dist_norm_axi(face);
        alpha = 0.5*(alpha_turb(n0)+alpha_turb(n1));
        u_teta[face] = alpha*(temper[n1] - temper[n0])/dist;
      }
  else

    for (face=premiere_face_int; face<nb_faces; face++)
      {
        n0 = face_voisins(face,0);
        n1 = face_voisins(face,1);
        dist = zone_VDF.dist_norm(face);
        alpha = 0.5*(alpha_turb(n0)+alpha_turb(n1));
        u_teta[face] = alpha*(temper[n1] - temper[n0])/dist;
      }

  // Traitement des conditions limites de type Entree_fluide_K_Eps_impose :

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zcl_VDF.les_conditions_limites(n_bord);

      if (sub_type(Entree_fluide_temperature_imposee,la_cl.valeur()) )
        {
          const Entree_fluide_temperature_imposee& la_cl_diri=ref_cast(Entree_fluide_temperature_imposee,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (face=ndeb; face<nfin; face++)
            {
              double T_imp = la_cl_diri.val_imp(face-ndeb);
              n0 = face_voisins(face,0);
              n1 = face_voisins(face,1);
              if (Objet_U::axi)
                dist = 2*zone_VDF.dist_norm_bord_axi(face);
              else
                dist = 2*zone_VDF.dist_norm_bord(face);
              if (n0 != -1)
                {
                  alpha = alpha_turb(n0);
                  u_teta[face] = alpha*(T_imp-temper[n0])/dist;
                }
              else   // n1 != -1
                {
                  alpha = alpha_turb(n1);
                  u_teta[face] = alpha*(temper[n1]-T_imp)/dist;
                }
            }
        }
    }
  return u_teta;
}

DoubleTab& Calcul_Production_K_VDF::calculer_u_conc(const Zone_VDF& zone_VDF,
                                                    const Zone_Cl_VDF& zcl_VDF,
                                                    const DoubleTab& conc,
                                                    const DoubleTab& alpha_turb,
                                                    DoubleTab& u_conc,int nb_consti) const

{
  //                                      ---->
  // On note u_conc le vecteur alpha_turb.gradC
  //
  // Sur chaque face on calcule la composante de u_conc normale a la face

  int nb_faces= zone_VDF.nb_faces();
  int n0,n1;
  double alpha,dist;
  int face;
  const IntTab& face_voisins = zone_VDF.face_voisins();
  u_conc = 0;

  // Traitement des faces internes


  int premiere_face_int=zone_VDF.premiere_face_int();
  nb_faces=zone_VDF.nb_faces();
  int k;

  if (Objet_U::axi)

    for (face=premiere_face_int; face<nb_faces; face++)
      {
        n0 = face_voisins(face,0);
        n1 = face_voisins(face,1);
        dist = zone_VDF.dist_norm_axi(face);
        alpha = 0.5*(alpha_turb(n0)+alpha_turb(n1));
        for (k=0; k<nb_consti; k++)
          u_conc(face,k) = alpha*(conc(n1,k) - conc(n0,k))/dist;
      }
  else

    for (face=premiere_face_int; face<nb_faces; face++)
      {
        n0 = face_voisins(face,0);
        n1 = face_voisins(face,1);
        dist = zone_VDF.dist_norm(face);
        alpha = 0.5*(alpha_turb(n0)+alpha_turb(n1));
        for (k=0; k<nb_consti; k++)
          u_conc(face,k) = alpha*(conc(n1,k) - conc(n0,k))/dist;
      }

  // Traitement des conditions limites de type Entree_fluide_K_Eps_impose :

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zcl_VDF.les_conditions_limites(n_bord);

      if ( sub_type(Entree_fluide_concentration_imposee,la_cl.valeur()) )
        {
          const Entree_fluide_concentration_imposee& la_cl_diri= ref_cast(Entree_fluide_concentration_imposee,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (face=ndeb; face<nfin; face++)
            {
              n0 = face_voisins(face,0);
              n1 = face_voisins(face,1);
              if (Objet_U::axi)
                dist = 2*zone_VDF.dist_norm_bord_axi(face);
              else
                dist = 2*zone_VDF.dist_norm_bord(face);
              if (n0 != -1)
                {
                  alpha = alpha_turb(n0);
                  for (k=0; k<nb_consti; k++)
                    u_conc(face,k) = alpha*(la_cl_diri.val_imp(face-ndeb,k)-conc(n0,k))/dist;
                }
              else   // n1 != -1
                {
                  alpha = alpha_turb(n1);
                  for (k=0; k<nb_consti; k++)
                    u_conc(face,k) = alpha*(conc(n1,k)-la_cl_diri.val_imp(face-ndeb,k))/dist;
                }
            }
        }
    }
  return u_conc;
}


DoubleVect& Calcul_Production_K_VDF::calculer_terme_destruction_K(const Zone_VDF& zone_VDF,
                                                                  const Zone_Cl_VDF& zcl_VDF,
                                                                  DoubleVect& G,const DoubleTab& temper,
                                                                  const DoubleTab& alpha_turb,
                                                                  double beta,const DoubleVect& gravite) const
{
  //
  // G est discretise comme K et Eps i.e au centre des elements
  //
  //                                       --> ----->
  // G(elem) = beta alpha_t(elem) G . gradT(elem)
  //

  int nb_elem = zone_VDF.nb_elem();
  int nb_faces= zone_VDF.nb_faces();
  DoubleTrav u_teta(nb_faces);
  const DoubleVect& porosite_face = zone_VDF.porosite_face();

  //                                      ---->
  // On note u_teta le vecteur alpha_turb.gradT
  //
  // Appel a la fonction qui calcule sur chaque face la composante
  // de u_teta normale a la face

  calculer_u_teta(zone_VDF,zcl_VDF,temper,alpha_turb,u_teta);

  //                                          ------> ----->
  // On calcule ensuite une valeur moyenne de gravite.u_teta sur chaque
  // element.

  G = 0;

  //                ------->  ------>
  // Calcul de beta.gravite . u_teta

  const Zone& la_zone=zone_VDF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();

  IntTrav numfa(nb_faces_elem);
  DoubleVect coef(Objet_U::dimension);
  const IntTab& les_elem_faces = zone_VDF.elem_faces();

  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int i=0; i<nb_faces_elem; i++)
        numfa[i] = les_elem_faces(elem,i);

      coef(0) = 0.5*(u_teta(numfa[0])*porosite_face(numfa[0])
                     + u_teta(numfa[Objet_U::dimension])*porosite_face(numfa[Objet_U::dimension]));
      coef(1) = 0.5*(u_teta(numfa[1])*porosite_face(numfa[1])
                     + u_teta(numfa[Objet_U::dimension+1])*porosite_face(numfa[Objet_U::dimension+1]));

      if (Objet_U::dimension ==2)
        G[elem] = beta*(gravite(0)*coef(0) + gravite(1)*coef(1));

      else if (Objet_U::dimension == 3)
        {
          coef(2) = 0.5*(u_teta(numfa[2])*porosite_face(numfa[2])
                         + u_teta(numfa[5])*porosite_face(numfa[5]));
          G[elem] = beta*(gravite(0)*coef(0) + gravite(1)*coef(1) + gravite(2)*coef(2));
        }

    }
  G.echange_espace_virtuel();
  return G;
}

DoubleVect& Calcul_Production_K_VDF::calculer_terme_destruction_K(const Zone_VDF& zone_VDF,
                                                                  const Zone_Cl_VDF& zcl_VDF,
                                                                  DoubleVect& G,const DoubleTab& temper,
                                                                  const DoubleTab& alpha_turb,
                                                                  const DoubleTab& beta,const DoubleVect& gravite) const
{
  //
  // G est discretise comme K et Eps i.e au centre des elements
  //
  //                                       --> ----->
  // G(elem) = beta(elem) alpha_t(elem) G . gradT(elem)
  //

  int nb_elem = zone_VDF.nb_elem();
  int nb_faces= zone_VDF.nb_faces();
  DoubleTrav u_teta(nb_faces);
  const DoubleVect& porosite_face = zone_VDF.porosite_face();

  //                                      ---->
  // On note u_teta le vecteur alpha_turb.gradT
  //
  // Appel a la fonction qui calcule sur chaque face la composante
  // de u_teta normale a la face

  calculer_u_teta(zone_VDF,zcl_VDF,temper,alpha_turb,u_teta);

  //                                          ------> ----->
  // On calcule ensuite une valeur moyenne de gravite.u_teta sur chaque
  // element.

  G = 0;

  //                ------->  ------>
  // Calcul de beta.gravite . u_teta

  const Zone& la_zone=zone_VDF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();
  IntTrav numfa(nb_faces_elem);
  const IntTab& les_elem_faces = zone_VDF.elem_faces();
  DoubleVect coef(Objet_U::dimension);

  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int i=0; i<nb_faces_elem; i++)
        numfa[i] = les_elem_faces(elem,i);

      coef(0) = 0.5*(u_teta(numfa[0])*porosite_face(numfa[0])
                     + u_teta(numfa[Objet_U::dimension])*porosite_face(numfa[Objet_U::dimension]));
      coef(1) = 0.5*(u_teta(numfa[1])*porosite_face(numfa[1])
                     + u_teta(numfa[Objet_U::dimension+1])*porosite_face(numfa[Objet_U::dimension+1]));

      if (Objet_U::dimension ==2)
        G[elem] = beta(elem)*(gravite(0)*coef(0) + gravite(1)*coef(1));

      else if (Objet_U::dimension == 3)
        {
          coef(2) = 0.5*(u_teta(numfa[2])*porosite_face(numfa[2])
                         + u_teta(numfa[5])*porosite_face(numfa[5]));
          G[elem] = beta(elem)*(gravite(0)*coef(0) + gravite(1)*coef(1) + gravite(2)*coef(2));
        }

    }
  G.echange_espace_virtuel();
  return G;
}

DoubleVect& Calcul_Production_K_VDF::calculer_terme_destruction_K(const Zone_VDF& zone_VDF,
                                                                  const Zone_Cl_VDF& zcl_VDF,
                                                                  DoubleVect& G,const DoubleTab& temper,
                                                                  const DoubleTab& alpha_turb,
                                                                  const DoubleVect& beta,
                                                                  const DoubleVect& gravite,
                                                                  int nb_consti) const
{
  //
  // G est discretise comme K et Eps i.e au centre des elements
  //
  // G(elem) est la somme sur les nb_consti de Gk(elem,k)
  //
  // avec
  //                                -> --->
  //      Gk(i,k) = beta_c(k).alpha_t(i).G.gradC(i,k)
  //

  int nb_elem = zone_VDF.nb_elem();
  int nb_faces= zone_VDF.nb_faces();
  DoubleTrav u_conc(nb_faces,nb_consti);
  const DoubleVect& porosite_face = zone_VDF.porosite_face();

  //         ------>                      ---->
  // On note u_conc le vecteur alpha_turb.gradC
  //
  // Appel a la fonction qui calcule sur chaque face la composante
  // de u_conc normale a la face

  calculer_u_conc(zone_VDF,zcl_VDF,temper,alpha_turb,u_conc,nb_consti);

  //                                          ------> ----->
  // On calcule ensuite une valeur moyenne de gravite.u_conc sur chaque
  // element.

  G = 0;

  //                ------->  ----->
  // Calcul de beta.gravite . u_conc

  const Zone& la_zone=zone_VDF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();
  IntTrav numfa(nb_faces_elem);
  const IntTab& les_elem_faces = zone_VDF.elem_faces();
  DoubleVect coef(Objet_U::dimension);
  int k;

  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int i=0; i<nb_faces_elem; i++)
        numfa[i] = les_elem_faces(elem,i);

      for (k=0; k<nb_consti; k++)
        {
          coef(0) = 0.5*(u_conc(numfa[0],k)*porosite_face(numfa[0])
                         + u_conc(numfa[Objet_U::dimension],k)*porosite_face(numfa[Objet_U::dimension]));
          coef(1) = 0.5*(u_conc(numfa[1],k)*porosite_face(numfa[1])
                         + u_conc(numfa[Objet_U::dimension+1],k)*porosite_face(numfa[Objet_U::dimension+1]));

          if (Objet_U::dimension ==2)
            G[elem] += beta(k)*(gravite(0)*coef(0) + gravite(1)*coef(1));
          else if (Objet_U::dimension == 3)
            {
              coef(2) = 0.5*(u_conc(numfa[2],k)*porosite_face(numfa[2])
                             + u_conc(numfa[5],k)*porosite_face(numfa[5]));
              G[elem] += beta(k)*(gravite(0)*coef(0) + gravite(1)*coef(1) + gravite(2)*coef(2));
            }
        }
    }
  G.echange_espace_virtuel();
  return G;
}

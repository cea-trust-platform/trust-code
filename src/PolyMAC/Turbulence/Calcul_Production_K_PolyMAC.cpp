/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Calcul_Production_K_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Turbulence
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Calcul_Production_K_PolyMAC.h>
#include <Transport_K_Eps.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Probleme_base.h>
#include <IntTrav.h>
#include <Entree_fluide_temperature_imposee.h>
#include <Entree_fluide_concentration_imposee.h>
#include <Champ_Uniforme.h>
#include <Zone_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Debog.h>
#include <DoubleTrav.h>

////////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Calcul_Production_K_PolyMAC
//
/////////////////////////////////////////////////////////////////////////////
DoubleVect& Calcul_Production_K_PolyMAC::
calculer_terme_production_K(const Zone_PolyMAC& zone_PolyMAC, const Zone_Cl_PolyMAC& zone_Cl_PolyMAC , DoubleVect& S,  const DoubleTab& K_eps,
                            const DoubleTab& vitesse,const Champ_Face_PolyMAC& vit,  const DoubleTab& visco_turb )  const
{
  IntVect element(4);
  S = 0. ;
  vit.calcul_S_barre(vitesse,S);

  Debog::verifier("Source_Transport_K_Eps_PolyMAC_P0_PolyMAC:: calculer_terme_production_K ",S);
  for (int elem = 0; elem < zone_PolyMAC.zone().nb_elem(); elem++)
    {
      S(elem) *= visco_turb(elem);

    }

  Debog::verifier("Source_Transport_K_Eps_PolyMAC_P0_PolyMAC:: calculer_terme_production_K ",S);
  return S;
}

DoubleTab& Calcul_Production_K_PolyMAC::calculer_u_teta(const Zone_PolyMAC& zone_PolyMAC,
                                                        const Zone_Cl_PolyMAC& zcl_PolyMAC,
                                                        const DoubleTab& temper,
                                                        const DoubleTab& alpha_turb,
                                                        DoubleTab& u_teta) const
{
  //                                      ---->
  // On note u_teta le vecteur alpha_turb.gradT
  //
  // Sur chaque face on calcule la composante de u_teta normale a la face

  int nb_faces= zone_PolyMAC.nb_faces();
  int n0,n1;
  double alpha,dist;
  int face;
  const IntTab& face_voisins = zone_PolyMAC.face_voisins();
  u_teta = 0;

  // Traitement des faces internes


  int premiere_face_int=zone_PolyMAC.premiere_face_int();
  nb_faces=zone_PolyMAC.nb_faces();



  for (face=premiere_face_int; face<nb_faces; face++)
    {
      n0 = face_voisins(face,0);
      n1 = face_voisins(face,1);
      dist = zone_PolyMAC.dist_norm(face);
      alpha = 0.5*(alpha_turb(n0)+alpha_turb(n1));
      u_teta[face] = alpha*(temper[n1] - temper[n0])/dist;
    }

  // Traitement des conditions limites de type Entree_fluide_K_Eps_impose :

  for (int n_bord=0; n_bord<zone_PolyMAC.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zcl_PolyMAC.les_conditions_limites(n_bord);

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

              dist = 2*zone_PolyMAC.dist_norm_bord(face);
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

DoubleTab& Calcul_Production_K_PolyMAC::calculer_u_conc(const Zone_PolyMAC& zone_PolyMAC,
                                                        const Zone_Cl_PolyMAC& zcl_PolyMAC,
                                                        const DoubleTab& conc,
                                                        const DoubleTab& alpha_turb,
                                                        DoubleTab& u_conc,int nb_consti) const

{
  //                                      ---->
  // On note u_conc le vecteur alpha_turb.gradC
  //
  // Sur chaque face on calcule la composante de u_conc normale a la face

  int nb_faces= zone_PolyMAC.nb_faces();
  int n0,n1;
  double alpha,dist;
  int face;
  const IntTab& face_voisins = zone_PolyMAC.face_voisins();
  u_conc = 0;

  // Traitement des faces internes


  int premiere_face_int=zone_PolyMAC.premiere_face_int();
  nb_faces=zone_PolyMAC.nb_faces();
  int k;



  for (face=premiere_face_int; face<nb_faces; face++)
    {
      n0 = face_voisins(face,0);
      n1 = face_voisins(face,1);
      dist = zone_PolyMAC.dist_norm(face);
      alpha = 0.5*(alpha_turb(n0)+alpha_turb(n1));
      for (k=0; k<nb_consti; k++)
        u_conc(face,k) = alpha*(conc(n1,k) - conc(n0,k))/dist;
    }

  // Traitement des conditions limites de type Entree_fluide_K_Eps_impose :

  for (int n_bord=0; n_bord<zone_PolyMAC.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zcl_PolyMAC.les_conditions_limites(n_bord);

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

              dist = 2*zone_PolyMAC.dist_norm_bord(face);
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


DoubleVect& Calcul_Production_K_PolyMAC::calculer_terme_destruction_K(const Zone_PolyMAC& zone_PolyMAC,
                                                                      const Zone_Cl_PolyMAC& zcl_PolyMAC,
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

  int nb_elem = zone_PolyMAC.nb_elem();
  int nb_faces= zone_PolyMAC.nb_faces();
  DoubleTrav u_teta(nb_faces);
  const DoubleVect& porosite_face = zone_PolyMAC.porosite_face();

  //                                      ---->
  // On note u_teta le vecteur alpha_turb.gradT
  //
  // Appel a la fonction qui calcule sur chaque face la composante
  // de u_teta normale a la face

  calculer_u_teta(zone_PolyMAC,zcl_PolyMAC,temper,alpha_turb,u_teta);

  //                                          ------> ----->
  // On calcule ensuite une valeur moyenne de gravite.u_teta sur chaque
  // element.

  G = 0;

  //                ------->  ------>
  // Calcul de beta.gravite . u_teta

  const Zone& la_zone=zone_PolyMAC.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();

  IntTrav numfa(nb_faces_elem);
  DoubleVect coef(Objet_U::dimension);
  const IntTab& les_elem_faces = zone_PolyMAC.elem_faces();

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

DoubleVect& Calcul_Production_K_PolyMAC::calculer_terme_destruction_K(const Zone_PolyMAC& zone_PolyMAC,
                                                                      const Zone_Cl_PolyMAC& zcl_PolyMAC,
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

  int nb_elem = zone_PolyMAC.nb_elem();
  int nb_faces= zone_PolyMAC.nb_faces();
  DoubleTrav u_teta(nb_faces);
  const DoubleVect& porosite_face = zone_PolyMAC.porosite_face();

  //                                      ---->
  // On note u_teta le vecteur alpha_turb.gradT
  //
  // Appel a la fonction qui calcule sur chaque face la composante
  // de u_teta normale a la face

  calculer_u_teta(zone_PolyMAC,zcl_PolyMAC,temper,alpha_turb,u_teta);

  //                                          ------> ----->
  // On calcule ensuite une valeur moyenne de gravite.u_teta sur chaque
  // element.

  G = 0;

  //                ------->  ------>
  // Calcul de beta.gravite . u_teta

  const Zone& la_zone=zone_PolyMAC.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();
  IntTrav numfa(nb_faces_elem);
  const IntTab& les_elem_faces = zone_PolyMAC.elem_faces();
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

DoubleVect& Calcul_Production_K_PolyMAC::calculer_terme_destruction_K(const Zone_PolyMAC& zone_PolyMAC,
                                                                      const Zone_Cl_PolyMAC& zcl_PolyMAC,
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

  int nb_elem = zone_PolyMAC.nb_elem();
  int nb_faces= zone_PolyMAC.nb_faces();
  DoubleTrav u_conc(nb_faces,nb_consti);
  const DoubleVect& porosite_face = zone_PolyMAC.porosite_face();

  //         ------>                      ---->
  // On note u_conc le vecteur alpha_turb.gradC
  //
  // Appel a la fonction qui calcule sur chaque face la composante
  // de u_conc normale a la face

  calculer_u_conc(zone_PolyMAC,zcl_PolyMAC,temper,alpha_turb,u_conc,nb_consti);

  //                                          ------> ----->
  // On calcule ensuite une valeur moyenne de gravite.u_conc sur chaque
  // element.

  G = 0;

  //                ------->  ----->
  // Calcul de beta.gravite . u_conc

  const Zone& la_zone=zone_PolyMAC.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();
  IntTrav numfa(nb_faces_elem);
  const IntTab& les_elem_faces = zone_PolyMAC.elem_faces();
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

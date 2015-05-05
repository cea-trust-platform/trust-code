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
// File:        Modele_Jones_Launder_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_Jones_Launder_VDF.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <IntTrav.h>
#include <Les_Cl.h>
#include <Champ_Face.h>

Implemente_instanciable(Modele_Jones_Launder_VDF,"Modele_Jones_Launder_VDF",Modele_Fonc_Bas_Reynolds_Base);

// printOn et readOn

Sortie& Modele_Jones_Launder_VDF::printOn(Sortie& s ) const
{
  return s;
}

Entree& Modele_Jones_Launder_VDF::readOn(Entree& is )
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

Entree& Modele_Jones_Launder_VDF::lire(const Motcle& , Entree& is)
{
  return is;
}
///////////////////////////////////////////////////////////////
//   Implementation des fonctions de la classe
///////////////////////////////////////////////////////////////

void  Modele_Jones_Launder_VDF::associer(const Zone_dis& zone_dis,
                                         const Zone_Cl_dis& zone_Cl_dis)
{
  //  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  //  const Zone_Cl_VDF& la_zone_Cl = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
}



DoubleTab& Modele_Jones_Launder_VDF::Calcul_D(DoubleTab& D,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
                                              const DoubleTab& vitesse,const DoubleTab& K_eps_Bas_Re, double visco ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& la_zone_Cl = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
  D = 0;
  //  const DoubleVect& volumes = la_zone.volumes();
  const DoubleVect& porosite_surf = la_zone.porosite_face();
  const DoubleVect& volume_entrelaces = la_zone.volumes_entrelaces();
  //  int nb_elem = la_zone.nb_elem();
  //  int nb_elem_tot = la_zone.nb_elem_tot();
  const Zone& zone=la_zone.zone();

  int nb_faces_elem = zone.nb_faces_elem();
  IntTrav numfa(nb_faces_elem);
  double coef;
  //  const IntTab& elem_faces = la_zone.elem_faces();
  const IntTab& face_voisins = la_zone.face_voisins();
  int nb_faces = la_zone.nb_faces();

  double gradk;
  int num_face,poly1,poly2,ori, ndeb, nfin;

  // Calcul de Gradient de racine de K.
  if (mp_min_vect(K_eps_Bas_Re)<0)
    {
      Cerr << "Il y'a des valeurs negatives dans les valeurs de K" << finl;
      Cerr << "dans Modele_Jones_Launder_VDF::Calcul_D" << finl;
      Cerr << "On arrete le calcul." << finl;
      exit();
    }
  // Boucle sur les bords pour traiter les conditions aux limites
  for (int n_bord=0; n_bord<la_zone.nb_front_Cl(); n_bord++)

    {
      const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);

      if ( sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet_entree_fluide& la_cl_typee = ref_cast(Dirichlet_entree_fluide, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              gradk = 0;
              poly1 =  face_voisins(num_face,0);
              if (poly1 != -1)
                {
                  // coef = 0.5;
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)*0.5;
                  gradk = ((sqrt(la_cl_typee.val_imp(num_face-ndeb,0)) - sqrt(K_eps_Bas_Re(poly1,0))))/la_zone.dist_norm_bord(num_face);
                  D[poly1] += 2*visco*(gradk*gradk)*coef;
                }
              else
                {
                  poly2 = face_voisins(num_face,1);
                  // coef = 0.5;
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)*0.5;
                  gradk = ((sqrt(K_eps_Bas_Re(poly2,0)) - sqrt(la_cl_typee.val_imp(num_face-ndeb,0))))/la_zone.dist_norm_bord(num_face);
                  //
                  D[poly2] += 2*visco*(gradk*gradk)*coef;
                }
            }
        }
      else if ( sub_type(Dirichlet_homogene,la_cl.valeur()) )
        {
          const Dirichlet_paroi_fixe& la_cl_typee = ref_cast(Dirichlet_paroi_fixe, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              gradk = 0;
              poly1 =  face_voisins(num_face,0);
              if (poly1 != -1)
                {
                  //  coef = 0.5;
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)*0.5;
                  gradk = ((sqrt(la_cl_typee.val_imp(num_face-ndeb,0)) - sqrt(K_eps_Bas_Re(poly1,0))))/la_zone.dist_norm_bord(num_face);
                  D[poly1] += 2*visco*(gradk*gradk)*coef;
                }
              else
                {
                  poly2 = face_voisins(num_face,1);
                  // coef = 0.5;
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)*0.5;
                  gradk = ((sqrt(K_eps_Bas_Re(poly2,0)) - sqrt(la_cl_typee.val_imp(num_face-ndeb,0))))/la_zone.dist_norm_bord(num_face);
                  D[poly2] += 2*visco*(gradk*gradk)*coef;
                }
            }
        }
      else if ( sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
        {
          const Dirichlet_paroi_defilante& la_cl_typee = ref_cast(Dirichlet_paroi_defilante, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              gradk = 0;
              poly1 =  face_voisins(num_face,0);
              if (poly1 != -1)
                {
                  //                  coef = 0.5;
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)*0.5;
                  gradk = ((sqrt(la_cl_typee.val_imp(num_face-ndeb,0)) - sqrt(K_eps_Bas_Re(poly1,0))))/la_zone.dist_norm_bord(num_face);
                  D[poly1] += 2*visco*(gradk*gradk)*coef;
                }
              else
                {
                  poly2 = face_voisins(num_face,1);
                  //  coef = 0.5;
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)*0.5;
                  gradk = ((sqrt(K_eps_Bas_Re(poly2,0)) - sqrt(la_cl_typee.val_imp(num_face-ndeb,0))))/la_zone.dist_norm_bord(num_face);
                  D[poly2] += 2*visco*(gradk*gradk)*coef;
                }
            }
        }
      else if (sub_type(Symetrie,la_cl.valeur()))
        ;
      else if ( (sub_type(Neumann,la_cl.valeur()))
                ||
                (sub_type(Neumann_homogene,la_cl.valeur()))
              )
        {
          // do nothing
          ;
        }
    }

  // Traitement des faces internes
  for (num_face=la_zone.premiere_face_int(); num_face<nb_faces; num_face++)
    {
      poly1 = face_voisins(num_face,0);
      poly2 = face_voisins(num_face,1);
      ori = la_zone.orientation(num_face);
      // coef = 0.5;

      coef = volume_entrelaces(num_face)*porosite_surf(num_face);

      //      Cerr << "K_eps_Bas_Re(poly2,0)=" << K_eps_Bas_Re(poly2,0) << " K_eps_Bas_Re(poly1,0)=" << K_eps_Bas_Re(poly1,0) << "  la_zone.xp(poly2,ori)=" << la_zone.xp(poly2,ori) << " la_zone.xp(poly1,ori)=" << la_zone.xp(poly1,ori) << finl;
      gradk =  (sqrt(K_eps_Bas_Re(poly2,0))-sqrt(K_eps_Bas_Re(poly1,0)))/(la_zone.xp(poly2,ori)- la_zone.xp(poly1,ori));
      D[poly1] += 2*visco*(gradk*gradk)*coef;
      D[poly2] += 2*visco*(gradk*gradk)*coef;
    }
  return D;
  // D abord sans le 1/3 2/3
}

// void Modele_Jones_Launder_VDF::associer_zones(const Zone_dis& zone_dis,
//                                                         const Zone_Cl_dis& zone_Cl_dis)
// {
//   la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
//   la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
// }


DoubleTab& Modele_Jones_Launder_VDF::Calcul_E(DoubleTab& E,const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& vit,const DoubleTab& K_eps_Bas_Re,double visco, const DoubleTab& visco_turb ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& la_zone_Cl = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
  E = 0;

  int nb_faces_tot = la_zone.nb_faces_tot();
  const IntTab& face_voisins = la_zone.face_voisins();
  int n_bord,poly1, poly2,fac=-1;

  DoubleTab derivee_premiere(0, Objet_U::dimension, Objet_U::dimension);
  la_zone.creer_tableau_faces(derivee_premiere);
  DoubleTab derivee_seconde(derivee_premiere);

  calcul_derivees_premieres_croisees(derivee_premiere,zone_dis,zone_Cl_dis,vit );
  calcul_derivees_secondes_croisees(derivee_seconde,zone_dis,zone_Cl_dis,derivee_premiere);

  // traitement par faces pour avoir la contribution par element de D
  double val2,val3,val4;
  for (n_bord=0; n_bord<la_zone.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (fac=ndeb; fac<nfin; fac++)
        {
          poly1 = face_voisins(fac,0);
          if (poly1 != -1)
            {
              if (Objet_U::dimension == 2 )
                {
                  val2 = derivee_seconde(fac,0,1)*derivee_seconde(fac,0,1);
                  val3 = derivee_seconde(fac,1,0)*derivee_seconde(fac,1,0);
                  E[poly1]+= 2*visco*visco_turb(poly1)*(val2+val3);
                }
              else
                {
                  val2 = derivee_seconde(fac,1,0)+derivee_seconde(fac,2,0); //d2v_dx2(fac)+d2w_dx2(fac);
                  val2 *= val2;
                  val3 = derivee_seconde(fac,0,1)+derivee_seconde(fac,2,1); //d2u_dy2(fac)+d2w_dy2(fac);
                  val3 *= val3;
                  val4 = derivee_seconde(fac,0,2)+derivee_seconde(fac,1,2); //d2u_dz2(fac)+d2v_dz2(fac);
                  val4 *= val4;

                  E[poly1] += 2*visco*visco_turb(poly1)*(val2+val3+val4);
                }
            }
          else
            {
              poly2 = face_voisins(fac,1);
              if (Objet_U::dimension == 2 )
                {
                  val2 = derivee_seconde(fac,0,1)*derivee_seconde(fac,0,1);
                  val3 = derivee_seconde(fac,1,0)*derivee_seconde(fac,1,0);
                  E[poly2]+= 2*visco*visco_turb(poly2)*(val2+val3);
                }
              else
                {
                  val2 = derivee_seconde(fac,1,0)+derivee_seconde(fac,2,0); //d2v_dx2(fac)+d2w_dx2(fac);
                  val2 *= val2;
                  val3 = derivee_seconde(fac,0,1)+derivee_seconde(fac,2,1); //d2u_dy2(fac)+d2w_dy2(fac);
                  val3 *= val3;
                  val4 = derivee_seconde(fac,0,2)+derivee_seconde(fac,1,2); //d2u_dz2(fac)+d2v_dz2(fac);
                  val4 *= val4;

                  E[poly2] += 2*visco*visco_turb(poly2)*(val2+val3+val4);
                }
            }
        }
    }

  // Traitement des faces internes
  for (; fac<nb_faces_tot; fac++)
    {
      poly1=face_voisins(fac,0);
      poly2=face_voisins(fac,1);
      if (Objet_U::dimension == 2 )
        {
          val2 = derivee_seconde(fac,0,1)*derivee_seconde(fac,0,1);
          val3 = derivee_seconde(fac,1,0)*derivee_seconde(fac,1,0);

          if (poly1>=0)
            E[poly1]+= 2*visco*visco_turb(poly1)*(val2+val3);
          if (poly2>=0)
            E[poly2]+= 2*visco*visco_turb(poly2)*(val2+val3);
        }
      else
        {
          val2 = derivee_seconde(fac,1,0)+derivee_seconde(fac,2,0); //d2v_dx2(fac)+d2w_dx2(fac);
          val2 *= val2;
          val3 = derivee_seconde(fac,0,1)+derivee_seconde(fac,2,1); //d2u_dy2(fac)+d2w_dy2(fac);
          val3 *= val3;
          val4 = derivee_seconde(fac,0,2)+derivee_seconde(fac,1,2); //d2u_dz2(fac)+d2v_dz2(fac);
          val4 *= val4;

          if (poly1>=0)
            E[poly1] += 2*visco*visco_turb(poly1)*(val2+val3+val4);
          if (poly2>=0)
            E[poly2] += 2*visco*visco_turb(poly2)*(val2+val3+val4);
        }
    }
  return E;
}

DoubleTab& Modele_Jones_Launder_VDF::calcul_derivees_premieres_croisees(DoubleTab& derivee_premiere, const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& vit ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& la_zone_Cl = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
  const Champ_Face& vitesse = ref_cast(Champ_Face,eq_hydraulique->inconnue().valeur());

  //  int nb_faces = la_zone.nb_faces();
  const IntTab& Qdm = la_zone.Qdm();
  const IntVect& orientation = la_zone.orientation();
  IntTrav num(4);
  int i,num_arete;
  double pond1,pond2;

  int signe,ori0,ori1;
  // DEBUT CALCUL DES DERIVEES PREMIERES

  if (Objet_U::dimension == 2)
    {
      DoubleVect coef(2);
      int ndeb,nfin;
      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees

      ndeb = la_zone.premiere_arete_interne();
      nfin = la_zone.nb_aretes_internes() + ndeb;

      for (num_arete =ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_premiere(num[0],ori0,ori1) +=  coef[0];
          derivee_premiere(num[1],ori0,ori1) +=  coef[0];
          derivee_premiere(num[2],ori1,ori0) +=  coef[1];
          derivee_premiere(num[3],ori1,ori0) +=  coef[1];
        }

      // Boucle sur les aretes_mixte
      ndeb = la_zone.premiere_arete_mixte();
      nfin = ndeb + la_zone.nb_aretes_mixtes();

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_premiere(num[0],ori0,ori1) +=  coef[0];
          derivee_premiere(num[1],ori0,ori1) +=  coef[0];
          derivee_premiere(num[2],ori1,ori0) +=  coef[1];
          derivee_premiere(num[3],ori1,ori0) +=  coef[1];
        }

      //*******************************
      //Prise en compte des CL
      //*******************************

      //*******************************
      //On parcourt les aretes bords
      //*******************************

      ndeb = la_zone.premiere_arete_bord();
      nfin = ndeb + la_zone.nb_aretes_bord();
      int n_type;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          n_type=la_zone_Cl.type_arete_bord(num_arete-ndeb);

          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          if (n_type == TypeAreteBordVDF::PERIO_PERIO)
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] =  0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);
              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
              derivee_premiere(num[3],ori1,ori0) +=  coef[1];
            }
          else
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              signe = num[3];

              pond1 = la_zone.face_normales(num[0],ori0);
              pond2 = la_zone.face_normales(num[1],ori0);

              double vit_imp = pond2*vitesse.val_imp_face_bord(num[0],ori1)+
                               vitesse.val_imp_face_bord(num[1],ori1)*pond1; // val tangentielle
              vit_imp /= pond1+pond2;
              //               double vit_imp = 0.5*(vitesse.val_imp_face_bord(num[0],ori1)+
              //                                     vitesse.val_imp_face_bord(num[1],ori1));                // val tangentielle
              coef[0] =  0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              ////coef[1] =  0.5*(vit_imp-vit(num[2]))/la_zone.dist_norm_bord(num[2])*signe;
              coef[1] =  0.5*(vit_imp-vit(num[2]))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
            }
        }


      //*******************************
      //On parcourt les aretes coins
      //*******************************

      ndeb = la_zone.premiere_arete_coin();
      nfin = ndeb + la_zone.nb_aretes_coin();

      int compt_coin=0;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          n_type=la_zone_Cl.type_arete_coin(num_arete-ndeb);
          //***************************************
          // Traitement des aretes coin perio-perio
          //***************************************

          if (n_type == TypeAreteCoinVDF::PERIO_PERIO) // arete de type periodicite-periodicite
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] =  0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);
              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
              derivee_premiere(num[3],ori1,ori0) +=  coef[1];
            }

          //***************************************
          // Traitement des aretes coin perio-paroi
          //***************************************
          else if (n_type == TypeAreteCoinVDF::PERIO_PAROI) // arete de type periodicite-paroi
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              signe = num[3];

              pond1 = la_zone.face_normales(num[0],ori0);
              pond2 = la_zone.face_normales(num[1],ori0);

              double vit_imp = pond2*vitesse.val_imp_face_bord(num[0],ori1)+
                               vitesse.val_imp_face_bord(num[1],ori1)*pond1; // val tangentielle
              vit_imp /= pond1+pond2;
              //               double vit_imp = 0.5*(vitesse.val_imp_face_bord(num[0],ori1)+
              //                                     vitesse.val_imp_face_bord(num[1],ori1));                // val tangentielle
              coef[0] =  0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(vit_imp-vit(num[2]))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
            }
          else
            {
              //Cerr << "Oh!!! Un coin!!!" << finl;
              compt_coin++;
            }

        }

      //     Cerr << "Il y a : " << compt_coin << " coins!!" << finl;
    }

  else if (Objet_U::dimension == 3)
    {
      DoubleVect coef(3);

      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees
      int ndeb = la_zone.premiere_arete_interne();
      int nfin = la_zone.nb_aretes_internes() + ndeb;

      for (num_arete =ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_premiere(num[0],ori0,ori1) +=  coef[0];
          derivee_premiere(num[1],ori0,ori1) +=  coef[0];
          derivee_premiere(num[2],ori1,ori0) +=  coef[1];
          derivee_premiere(num[3],ori1,ori0) +=  coef[1];
        }
      // Boucle sur les aretes_mixte
      ndeb = la_zone.premiere_arete_mixte();
      nfin = ndeb + la_zone.nb_aretes_mixtes();

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_premiere(num[0],ori0,ori1) +=  coef[0];
          derivee_premiere(num[1],ori0,ori1) +=  coef[0];
          derivee_premiere(num[2],ori1,ori0) +=  coef[1];
          derivee_premiere(num[3],ori1,ori0) +=  coef[1];
        }

      //*******************************
      //Prise en compte des CL
      //*******************************

      //*******************************
      //On parcourt les aretes bords
      //*******************************

      ndeb = la_zone.premiere_arete_bord();
      nfin = ndeb + la_zone.nb_aretes_bord();
      int n_type;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          n_type=la_zone_Cl.type_arete_bord(num_arete-ndeb);
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          if (n_type == TypeAreteBordVDF::PERIO_PERIO)
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              if (num[0]==num[1] || num[2]==num[3])
                {
                  Cerr << "2 bords avec une condition limite de periodicite ne sont separees que d'une maille !" << finl;
                  Cerr << "Cela n'est pas valide pour le Modele Jones Launder et son calcul de derivees croisees..." << finl;
                  exit();
                }
              coef[0] = 0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] = 0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);

              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
              derivee_premiere(num[3],ori1,ori0) +=  coef[1];
            }
          else
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              signe = num[3];

              //               double vit_imp = 0.5*(vitesse.val_imp_face_bord(num[0],ori1)+
              //                                     vitesse.val_imp_face_bord(num[1],ori1));                // val tangentielle
              pond1 = la_zone.face_normales(num[0],ori0);
              pond2 = la_zone.face_normales(num[1],ori0);

              double vit_imp = pond2*vitesse.val_imp_face_bord(num[0],ori1)+
                               vitesse.val_imp_face_bord(num[1],ori1)*pond1; // val tangentielle
              vit_imp /= pond1+pond2;

              coef[0] =  0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(vit_imp-vit(num[2]))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
            }
        }

      //*******************************
      //On parcourt les aretes coins
      //*******************************

      ndeb = la_zone.premiere_arete_coin();
      nfin = ndeb + la_zone.nb_aretes_coin();

      int compt_coin=0;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          n_type=la_zone_Cl.type_arete_coin(num_arete-ndeb);

          //***************************************
          // Traitement des aretes coin perio-perio
          //***************************************

          if (n_type == TypeAreteCoinVDF::PERIO_PERIO) // arete de type periodicite-periodicite
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] = 0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] = 0.5*(vit(num[3])-vit(num[2]))/la_zone.dist_face(num[2],num[3],ori0);

              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
              derivee_premiere(num[3],ori1,ori0) +=  coef[1];
            }

          //***************************************
          // Traitement des aretes coin perio-paroi
          //***************************************
          else if (n_type == TypeAreteCoinVDF::PERIO_PAROI) // arete de type periodicite-paroi
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              signe = num[3];

              pond1 = la_zone.face_normales(num[0],ori0);
              pond2 = la_zone.face_normales(num[1],ori0);

              double vit_imp = pond2*vitesse.val_imp_face_bord(num[0],ori1)+
                               vitesse.val_imp_face_bord(num[1],ori1)*pond1; // val tangentielle
              vit_imp /= pond1+pond2;
              //               double vit_imp = 0.5*(vitesse.val_imp_face_bord(num[0],ori1)+
              //                                     vitesse.val_imp_face_bord(num[1],ori1));                // val tangentielle

              coef[0] =  0.5*(vit(num[1])-vit(num[0]))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(vit_imp-vit(num[2]))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_premiere(num[0],ori0,ori1) +=  coef[0];
              derivee_premiere(num[1],ori0,ori1) +=  coef[0];
              derivee_premiere(num[2],ori1,ori0) +=  coef[1];
            }
          else
            {
              //Cerr << "Oh!!! Un coin!!!" << finl;
              compt_coin++;
            }
        }
      // Cerr << "Il y a : " << compt_coin << " coins!!" << finl;
    }
  derivee_premiere.echange_espace_virtuel();
  return derivee_premiere;
}

DoubleTab& Modele_Jones_Launder_VDF::calcul_derivees_secondes_croisees(DoubleTab& derivee_seconde, const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis, const DoubleTab& derivee_premiere ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& la_zone_Cl = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
  //  const Champ_Face& vitesse = ref_cast(Champ_Face,eq_hydraulique->inconnue().valeur());

  //  int nb_faces = la_zone.nb_faces();
  const IntTab& Qdm = la_zone.Qdm();
  const IntVect& orientation = la_zone.orientation();
  //  const int nb_cond_lim = la_zone_Cl.nb_cond_lim();
  IntTrav num(4);
  int i,num_arete,ori0,ori1;

  int ndeb=-10000,nfin=100000;//,signe;
  // DEBUT CALCUL DES DERIVEES SECONDES

  if (Objet_U::dimension == 2)
    {
      DoubleVect coef(2);

      ndeb = la_zone.premiere_arete_interne();
      nfin = la_zone.nb_aretes_internes() + ndeb;
      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees

      for (num_arete =ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_seconde(num[0],ori0,ori1) +=  coef[0];
          derivee_seconde(num[1],ori0,ori1) +=  coef[0];
          derivee_seconde(num[2],ori1,ori0) +=  coef[1];
          derivee_seconde(num[3],ori1,ori0) +=  coef[1];
        }

      // Boucle sur les aretes_mixte
      ndeb = la_zone.premiere_arete_mixte();
      nfin = ndeb + la_zone.nb_aretes_mixtes();

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_seconde(num[0],ori0,ori1) +=  coef[0];
          derivee_seconde(num[1],ori0,ori1) +=  coef[0];
          derivee_seconde(num[2],ori1,ori0) +=  coef[1];
          derivee_seconde(num[3],ori1,ori0) +=  coef[1];
        }

      //*******************************
      //Prise en compte des CL
      //*******************************

      //*******************************
      //On parcourt les aretes bords
      //*******************************
      ndeb = la_zone.premiere_arete_bord();
      nfin = ndeb + la_zone.nb_aretes_bord();
      int n_type;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          n_type=la_zone_Cl.type_arete_bord(num_arete-ndeb);

          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          if (n_type == TypeAreteBordVDF::PERIO_PERIO)
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] =  0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);
              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
              derivee_seconde(num[3],ori1,ori0) +=  coef[1];
            }
          else
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              // signe = num[3];

              // ///???????????????
              //               double  val_deriv_prem = 0.5*(vit.val_imp_face_bord(num[0],ori1)+
              //                                     vit.val_imp_face_bord(num[1],ori1));                // val tangentielle
              // ///???????????????
              coef[0] =  0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              //coef[1] =  0.5*(val_deriv_prem-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              derivee_seconde(num[2],ori1,ori0) =  2.*derivee_seconde(num[2],ori1,ori0);
            }
        }

      //*******************************
      //On parcourt les aretes coins
      //*******************************

      ndeb = la_zone.premiere_arete_coin();
      nfin = ndeb + la_zone.nb_aretes_coin();

      int compt_coin=0;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          n_type=la_zone_Cl.type_arete_coin(num_arete-ndeb);
          //***************************************
          // Traitement des aretes coin perio-perio
          //***************************************

          if (n_type == TypeAreteCoinVDF::PERIO_PERIO) // arete de type periodicite-periodicite
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] =  0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] =  0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);
              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
              derivee_seconde(num[3],ori1,ori0) +=  coef[1];
            }

          //***************************************
          // Traitement des aretes coin perio-paroi
          //***************************************
          else if (n_type == TypeAreteCoinVDF::PERIO_PAROI) // arete de type periodicite-paroi
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              //signe = num[3];

              // ///???????????????
              //               double  val_deriv_prem = 0.5*(vit.val_imp_face_bord(num[0],ori1)+
              //                                     vit.val_imp_face_bord(num[1],ori1));                // val tangentielle
              // ///???????????????
              coef[0] =  0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              //coef[1] =  0.5*(val_deriv_prem-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              //              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
              derivee_seconde(num[2],ori1,ori0) =  2.*derivee_seconde(num[2],ori1,ori0);
            }
          else
            {
              //Cerr << "Oh!!! Un coin!!!" << finl;
              compt_coin++;
            }

        }
      //  Cerr << "Il y a : " << compt_coin << " coins!!" << finl;
    }

  else if (Objet_U::dimension == 3)
    {
      //   Cerr << " tableau des derivee_premiere " << derivee_premiere << finl;
      DoubleVect coef(3);

      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees
      ndeb = la_zone.premiere_arete_interne();
      nfin = la_zone.nb_aretes_internes() + ndeb;

      for (num_arete =ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_seconde(num[0],ori0,ori1) +=  coef[0];
          derivee_seconde(num[1],ori0,ori1) +=  coef[0];
          derivee_seconde(num[2],ori1,ori0) +=  coef[1];
          derivee_seconde(num[3],ori1,ori0) +=  coef[1];
        }

      // Boucle sur les aretes_mixte
      ndeb = la_zone.premiere_arete_mixte();
      nfin = ndeb + la_zone.nb_aretes_mixtes();

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          ori0 = orientation(num[0]);
          ori1 = orientation(num[2]);
          coef[0] = 0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
          coef[1] = 0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);

          derivee_seconde(num[0],ori0,ori1) +=  coef[0];
          derivee_seconde(num[1],ori0,ori1) +=  coef[0];
          derivee_seconde(num[2],ori1,ori0) +=  coef[1];
          derivee_seconde(num[3],ori1,ori0) +=  coef[1];
        }

      //*******************************
      //Prise en compte des CL
      //*******************************

      //*******************************
      //On parcourt les aretes bords
      //*******************************

      ndeb = la_zone.premiere_arete_bord();
      nfin = ndeb + la_zone.nb_aretes_bord();
      int n_type;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          n_type=la_zone_Cl.type_arete_bord(num_arete-ndeb);
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          if (n_type == TypeAreteBordVDF::PERIO_PERIO)
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] = 0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] = 0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);

              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
              derivee_seconde(num[3],ori1,ori0) +=  coef[1];
            }
          else
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              //signe = num[3];

              // ///???????????????
              //               double  val_deriv_prem = 0.5*(vit.val_imp_face_bord(num[0],ori1)+
              //                                     vit.val_imp_face_bord(num[1],ori1));                // val tangentielle
              // ///???????????????

              coef[0] =  0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              //coef[1] =  0.5*(val_deriv_prem-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              //              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
              derivee_seconde(num[2],ori1,ori0) =  2.*derivee_seconde(num[2],ori1,ori0);
            }
        }

      //*******************************
      //On parcourt les aretes coins
      //*******************************

      ndeb = la_zone.premiere_arete_coin();
      nfin = ndeb + la_zone.nb_aretes_coin();

      int compt_coin=0;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          for (i=0; i<4; i++)
            num[i] = Qdm(num_arete,i);

          n_type=la_zone_Cl.type_arete_coin(num_arete-ndeb);
          //***************************************
          // Traitement des aretes coin perio-perio
          //***************************************
          if (n_type == TypeAreteCoinVDF::PERIO_PERIO) // arete de type periodicite-periodicite
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              coef[0] = 0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              coef[1] = 0.5*(derivee_premiere(num[3],ori1,ori0)-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_face(num[2],num[3],ori0);

              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
              derivee_seconde(num[3],ori1,ori0) +=  coef[1];
            }

          //***************************************
          // Traitement des aretes coin perio-paroi
          //***************************************
          else if (n_type == TypeAreteCoinVDF::PERIO_PAROI) // arete de type periodicite-paroi
            {
              ori0 = orientation(num[0]);
              ori1 = orientation(num[2]);
              //signe = num[3];

              // ///???????????????
              //               double  val_deriv_prem = 0.5*(vit.val_imp_face_bord(num[0],ori1)+
              //                                     vit.val_imp_face_bord(num[1],ori1));                // val tangentielle
              // ///???????????????

              coef[0] =  0.5*(derivee_premiere(num[1],ori0,ori1)-derivee_premiere(num[0],ori0,ori1))/la_zone.dist_face(num[0],num[1],ori1);
              //coef[1] =  0.5*(val_deriv_prem-derivee_premiere(num[2],ori1,ori0))/la_zone.dist_norm_bord(num[0])*signe;
              derivee_seconde(num[0],ori0,ori1) +=  coef[0];
              derivee_seconde(num[1],ori0,ori1) +=  coef[0];
              derivee_seconde(num[2],ori1,ori0) =  2.*derivee_seconde(num[2],ori1,ori0);
              //              derivee_seconde(num[2],ori1,ori0) +=  coef[1];
            }
          else
            {
              //Cerr << "Oh!!! Un coin!!!" << finl;
              compt_coin++;
            }
        }
      //  Cerr << "Il y a : " << compt_coin << " coins!!" << finl;
    }
  // Cerr << "derivee_seconde " << derivee_seconde << finl;
  derivee_seconde.echange_espace_virtuel();
  return derivee_seconde;
}

DoubleTab& Modele_Jones_Launder_VDF::Calcul_F1( DoubleTab& F1, const Zone_dis& zone_dis) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  int nb_elem = la_zone.nb_elem();
  for (int elem=0; elem <nb_elem; elem ++ )
    F1[elem] = 1.;
  return F1;
}

DoubleTab& Modele_Jones_Launder_VDF::Calcul_F2( DoubleTab& F2, DoubleTab& D, const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,double visco ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  int nb_elem = la_zone.nb_elem();
  double Re;
  int elem;
  //   Cerr << " Calc F2 " << finl;

  for (elem=0; elem< nb_elem ; elem++)
    {
      Re = K_eps_Bas_Re(elem,0)*K_eps_Bas_Re(elem,0)/K_eps_Bas_Re(elem,1)/visco;
      F2[elem] = 1. - (0.3*exp(-1*carre(Re)));
    }
  return F2;
}
DoubleTab& Modele_Jones_Launder_VDF::Calcul_F2( DoubleTab& F2, DoubleTab& D, const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re, const DoubleTab& tab_visco ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  int nb_elem = la_zone.nb_elem();
  DoubleTab Re(nb_elem);
  int elem;

  for (elem=0; elem< nb_elem ; elem++)
    {
      if (K_eps_Bas_Re(elem,1)>0)
        {
          Re(elem) = (K_eps_Bas_Re(elem,0)*K_eps_Bas_Re(elem,0))/(tab_visco(elem)*K_eps_Bas_Re(elem,1));
          F2[elem] = 1. - (0.3*exp(-1*carre(Re(elem))));
        }
      else
        {
          F2[elem] = 1.;
        }
    }
  return F2;
}


DoubleTab&  Modele_Jones_Launder_VDF::Calcul_Fmu( DoubleTab& Fmu,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,double visco ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  Fmu = 0;
  int nb_elem = la_zone.nb_elem();
  double Re;
  int elem;
  //  Cerr << " Calc Fmu " << finl;
  for (elem=0; elem< nb_elem ; elem++)
    {
      Re = K_eps_Bas_Re(elem,0)*K_eps_Bas_Re(elem,0)/K_eps_Bas_Re(elem,1)/visco;
      Fmu[elem] = exp(-2.5/(1.+Re/50.));
      //   Fmu[elem] = exp(-3.4/((1.+Re/50.)*(1.+Re/50.)));
    }
  return Fmu;
}

DoubleTab&  Modele_Jones_Launder_VDF::Calcul_Fmu( DoubleTab& Fmu,const Zone_dis& zone_dis,const DoubleTab& K_eps_Bas_Re,const DoubleTab& tab_visco ) const
{
  const Zone_VDF& la_zone = ref_cast(Zone_VDF,zone_dis.valeur());
  Fmu = 0;
  int nb_elem = la_zone.nb_elem();
  int elem;
  DoubleTab Re(nb_elem);
  for (elem=0; elem< nb_elem ; elem++)
    {
      Re(elem) = (K_eps_Bas_Re(elem,0)*K_eps_Bas_Re(elem,0))/(tab_visco(elem)*K_eps_Bas_Re(elem,1));
      //     Fmu[elem] = exp(-2.5/(1.+K_eps_Bas_Re(elem,0)*K_eps_Bas_Re(elem,0)/(tab_visco(elem)*K_eps_Bas_Re(elem,1))));
      Fmu[elem] = exp(-2.5/(1.+Re(elem)/50.));
      //  Fmu[elem] = exp(-3.4/((1.+Re(elem)/50.)*(1.+Re(elem)/50.)));

    }

  return Fmu;
}

void  Modele_Jones_Launder_VDF::mettre_a_jour(double temps)
{
  ;
}

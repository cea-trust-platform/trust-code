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
// File:        Turbulence_hyd_sous_maille_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/33
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_VDF.h>
#include <Les_Cl.h>
#include <Paroi_negligeable_VDF.h>
#include <DoubleTrav.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Equation_base.h>
#include <Zone_Cl_VDF.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_VDF,"Modele_turbulence_hyd_sous_maille_VDF",Mod_turb_hyd_ss_maille_VDF);

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_VDF::readOn(Entree& s )
{
  return Mod_turb_hyd_ss_maille_VDF::readOn(s) ;
}

void Turbulence_hyd_sous_maille_VDF::set_param(Param& param)
{
  Mod_turb_hyd_ss_maille_VDF::set_param(param);
  param.ajouter_non_std("formulation_a_nb_points",(this));
}

int Turbulence_hyd_sous_maille_VDF::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="formulation_a_nb_points")
    {
      is >> nb_points; // nb_points= 4 ou 6
      if ((nb_points != 4) && (nb_points != 6))
        {
          Cerr << "Read keyword is :" << mot << finl;
          Cerr << "Error while reading the subgrid model." << finl;
          Cerr << "You have indicated "<< nb_points << " points "<<finl;
          Cerr << "while this model is available only for 4 or 6 points" << finl;
          exit();
        }
      Cerr << "Structure fonction formulation at " <<  nb_points << " points" << finl;
      if (nb_points==4)
        {
          is >> dir1;
          is >> dir2;
          assert(dir1 != dir2);
          Cerr << "The wall is parallel to the plane (x_" << dir1 << ",x_" << dir2 << ")" << finl;
          if (((dir1 == 0)&&(dir2 == 1))||((dir1 == 1)&&(dir2 == 0)))
            dir3 = 2;
          if (((dir1 == 0)&&(dir2 == 2))||((dir1 == 2)&&(dir2 == 0)))
            dir3 = 1;
          if (((dir1 == 1)&&(dir2 == 2))||((dir1 == 2)&&(dir2 == 1)))
            dir3 = 0;
        }
      return 1;
    }
  else
    return Mod_turb_hyd_ss_maille_VDF::lire_motcle_non_standard(mot,is);
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_VDF
//
//////////////////////////////////////////////////////////////////////////////


Champ_Fonc& Turbulence_hyd_sous_maille_VDF::calculer_viscosite_turbulente()
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  int nb_poly = zone_VDF.zone().nb_elem();
  int nb_poly_tot = la_zone_VDF->zone().nb_elem_tot();

  F2.resize(nb_poly_tot);

  calculer_fonction_structure();

  if (visco_turb.size() != nb_poly)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }


  Debog::verifier("Turbulence_hyd_sous_maille_VDF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  for (int elem=0; elem<nb_poly; elem++)
    visco_turb[elem] = Csm1_*l_(elem)*sqrt(F2[elem]);

  Debog::verifier("Turbulence_hyd_sous_maille_VDF::calculer_viscosite_turbulente visco_turb 1",visco_turb);


  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Turbulence_hyd_sous_maille_VDF::calculer_energie_cinetique_turb()
{
  double temps = mon_equation->inconnue().temps();
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  int nb_poly = la_zone_VDF->zone().nb_elem();

  if (k.size() != nb_poly)
    {
      Cerr << "Size error for the array containing the values of the turbulent kinetic energy." << finl;
      exit();
    }

  for (int elem=0; elem<nb_poly; elem++)
    k[elem] = Csm2_*F2[elem];

  energie_cinetique_turb_.changer_temps(temps);
}

void Turbulence_hyd_sous_maille_VDF::calculer_fonction_structure()
{

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_poly = zone_VDF.zone().nb_elem();
  int nb_poly_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();
  //  int nb_face_entier = zone_VDF.nb_faces_internes();
  DoubleTrav F_Elem(nb_poly_tot,dimension);
  int num0,num1,num2,num3;
  int k1,k2;
  double diff1,diff2,aux;

  // Dans le tableau F_Elem on stocke les fonctions de structure dans
  // chacune des directions d'espace
  // F_Elem(num_elem,0)  Fonction de structure dans la direction X
  //                             au noeud num_elem
  // F_Elem(num_elem,1) Fonction de structure dans la direction Y
  // F_Elem(num_elem,2) Fonction de structure dans la direction Z
  //
  // Principe de calcul des Fonctions de structure directionnelles:
  //
  //              2
  // F_X = (Ug-Ud)  + 1/4[ F_X(arete_XY_1) + F_X(arete_XY_2 ) +
  //                       F_X(arete_XY_3) + F_X(arete_XY_4)]
  //
  // On va calculer 0.25*F_X(arete_XY) et le distribuer aux 4
  // elements adjacents

  double diff;


  if (dimension == 3)  //dimension == 3
    {
      int num_elem;

      //NYB :
      for (num_elem=0; num_elem<nb_poly; num_elem++)
        {

          diff = vitesse[elem_faces(num_elem,0)] - vitesse[elem_faces(num_elem,3)];
          F_Elem(num_elem,0) = diff*diff;
          diff = vitesse[elem_faces(num_elem,1)] - vitesse[elem_faces(num_elem,4)];
          F_Elem(num_elem,1) = diff*diff;
          diff = vitesse[elem_faces(num_elem,2)] - vitesse[elem_faces(num_elem,5)];
          F_Elem(num_elem,2) = diff*diff;
        }

      //*******************************
      // Traitement des aretes internes
      //*******************************
      int ndeb = zone_VDF.premiere_arete_interne();
      int nfin = ndeb + zone_VDF.nb_aretes_internes();
      int num_arete;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          num3 = Qdm(num_arete,3);

          aux = vitesse[num1]-vitesse[num0];
          diff1= 0.25*aux*aux;
          aux = vitesse[num3]-vitesse[num2];
          diff2= 0.25*aux*aux;
          k1 = orientation(num0);
          k2 = orientation(num2);

          F_Elem(face_voisins(num0,0),k2) += diff1;
          F_Elem(face_voisins(num0,1),k2) += diff1;
          F_Elem(face_voisins(num1,0),k2) += diff1;
          F_Elem(face_voisins(num1,1),k2) += diff1;
          F_Elem(face_voisins(num2,0),k1) += diff2;
          F_Elem(face_voisins(num2,1),k1) += diff2;
          F_Elem(face_voisins(num3,0),k1) += diff2;
          F_Elem(face_voisins(num3,1),k1) += diff2;

        }

      //*******************************
      // Traitement des aretes mixtes
      //*******************************
      ndeb = zone_VDF.premiere_arete_mixte();
      nfin = ndeb + zone_VDF.nb_aretes_mixtes();

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          num3 = Qdm(num_arete,3);

          aux = vitesse[num1]-vitesse[num0];
          diff1= 0.25*aux*aux;
          aux = vitesse[num3]-vitesse[num2];
          diff2= 0.25*aux*aux;
          k1 = orientation(num0);
          k2 = orientation(num2);

          //int num_elem;
          num_elem = face_voisins(num0,0);
          if (num_elem != -1) F_Elem(num_elem,k2) += diff1;
          num_elem = face_voisins(num0,1);
          if (num_elem != -1) F_Elem(num_elem,k2) += diff1;
          num_elem = face_voisins(num1,0);
          if (num_elem != -1) F_Elem(num_elem,k2) += diff1;
          num_elem = face_voisins(num1,1);
          if (num_elem != -1) F_Elem(num_elem,k2) += diff1;
          num_elem = face_voisins(num2,0);
          if (num_elem != -1) F_Elem(num_elem,k1) += diff2;
          num_elem = face_voisins(num2,1);
          if (num_elem != -1) F_Elem(num_elem,k1) += diff2;
          num_elem = face_voisins(num3,0);
          if (num_elem != -1) F_Elem(num_elem,k1) += diff2;
          num_elem = face_voisins(num3,1);
          if (num_elem != -1) F_Elem(num_elem,k1) += diff2;
        }

      //*******************************
      //Prise en compte des CL
      //*******************************
      const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
      //      const int nb_cond_lim = zone_Cl_VDF.nb_cond_lim();
      //      int indic_perio =0;

      //*******************************
      //On parcourt les aretes bords
      //*******************************

      ndeb = zone_VDF.premiere_arete_bord();
      nfin = ndeb + zone_VDF.nb_aretes_bord();
      int n_type;

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          n_type=zone_Cl_VDF.type_arete_bord(num_arete-ndeb);

          //**********************************
          // Traitement des aretes bords periodiques
          //**********************************

          if (n_type == TypeAreteBordVDF::PERIO_PERIO) // arete bord de type periodicite
            {

              num0 = Qdm(num_arete,0);
              num1 = Qdm(num_arete,1);
              num2 = Qdm(num_arete,2);
              num3 = Qdm(num_arete,3);

              aux = vitesse[num1]-vitesse[num0];
              diff1= 0.5*0.25*aux*aux;
              aux = vitesse[num3]-vitesse[num2];
              diff2= 0.5*0.25*aux*aux;
              k1 = orientation(num0);
              k2 = orientation(num2);

              // On multiplie par 0.5 du fait de la periodicite, sinon on ajoute deux fois ce qu il faut
              // aux elements ayant des faces de periodicite

              F_Elem(face_voisins(num0,0),k2) += diff1;
              F_Elem(face_voisins(num0,1),k2) += diff1;
              F_Elem(face_voisins(num1,0),k2) += diff1;
              F_Elem(face_voisins(num1,1),k2) += diff1;
              F_Elem(face_voisins(num2,0),k1) += diff2;
              F_Elem(face_voisins(num2,1),k1) += diff2;
              F_Elem(face_voisins(num3,0),k1) += diff2;
              F_Elem(face_voisins(num3,1),k1) += diff2;
            }

          //**********************************
          // Traitement des aretes bords de paroi
          //**********************************
          // Calcul de nu_t a la paroi
          // Dans la formulation en nb_points = 4, on ne tient pas compte de cette composante
          if (n_type == TypeAreteBordVDF::PAROI_PAROI)  // arete bord de type paroi
            {
              num0 = Qdm(num_arete,0);
              num1 = Qdm(num_arete,1);
              num2 = Qdm(num_arete,2);

              aux = vitesse[num1]-vitesse[num0];
              diff1= 0.25*aux*aux;
              //aux = vitesse[num3]-vitesse[num2];
              //diff2= 0.25*aux*aux;
              k1 = orientation(num0);
              k2 = orientation(num2);

              num_elem = face_voisins(num0,0);
              if (num_elem==-1)
                num_elem = face_voisins(num0,1);
              F_Elem(num_elem,k2) += diff1;

              num_elem = face_voisins(num1,0);
              if (num_elem==-1)
                num_elem = face_voisins(num1,1);
              F_Elem(num_elem,k2) += diff1;
            }
        }


      //*******************************
      //On parcourt les aretes coins
      //*******************************

      ndeb = zone_VDF.premiere_arete_coin();
      nfin = ndeb + zone_VDF.nb_aretes_coin();

      for (num_arete=ndeb; num_arete<nfin; num_arete++)
        {
          n_type=zone_Cl_VDF.type_arete_coin(num_arete-ndeb);
          //***************************************
          // Traitement des aretes coin perio-perio
          //***************************************

          if (n_type == TypeAreteCoinVDF::PERIO_PERIO) // arete de type periodicite-periodicite
            {

              num0 = Qdm(num_arete,0);
              num1 = Qdm(num_arete,1);
              num2 = Qdm(num_arete,2);
              num3 = Qdm(num_arete,3);

              aux = vitesse[num1]-vitesse[num0];
              diff1= 0.25*0.25*aux*aux;
              aux = vitesse[num3]-vitesse[num2];
              diff2= 0.25*0.25*aux*aux;
              k1 = orientation(num0);
              k2 = orientation(num2);

              // On multiplie par 0.25 du fait de la double periodicite, sinon on ajoute 4 fois ce qu il faut
              // aux elements ayant des faces de periodicite

              F_Elem(face_voisins(num0,0),k2) += diff1;
              F_Elem(face_voisins(num0,1),k2) += diff1;
              F_Elem(face_voisins(num1,0),k2) += diff1;
              F_Elem(face_voisins(num1,1),k2) += diff1;
              F_Elem(face_voisins(num2,0),k1) += diff2;
              F_Elem(face_voisins(num2,1),k1) += diff2;
              F_Elem(face_voisins(num3,0),k1) += diff2;
              F_Elem(face_voisins(num3,1),k1) += diff2;
            }

          //***************************************
          // Traitement des aretes coin perio-paroi
          //***************************************
          if (n_type == TypeAreteCoinVDF::PERIO_PAROI) // arete de type periodicite-paroi
            {
              num0 = Qdm(num_arete,0);
              num1 = Qdm(num_arete,1);
              num2 = Qdm(num_arete,2);

              // On multiplie par 0.5 du fait de la periodicite, sinon on ajoute deux fois ce qu il faut
              // aux elements ayant des faces de periodicite

              aux = vitesse[num1]-vitesse[num0];
              diff1= 0.5*0.25*aux*aux;
              //aux = vitesse[num3]-vitesse[num2];
              //diff2= 0.5*0.25*aux*aux;
              k1 = orientation(num0);
              k2 = orientation(num2);

              num_elem = face_voisins(num0,0);
              if (num_elem==-1)
                num_elem = face_voisins(num0,1);
              F_Elem(num_elem,k2) += diff1;

              num_elem = face_voisins(num1,0);
              if (num_elem==-1)
                num_elem = face_voisins(num1,1);
              F_Elem(num_elem,k2) += diff1;
            }
        }


      // Calcul de la Fonction de structure a partir de ses
      // composantes directionnelles

      double un_tiers = 1./3.;
      double deux_tiers = 2./3.;
      double delta_C;
      double un_demi = 1./2.;

      if (nb_points == 6)
        {

          for (num_elem=0; num_elem<nb_poly; num_elem++)
            {
              delta_C = l_(num_elem);
              F2[num_elem]  = un_tiers*(F_Elem(num_elem,0)*pow(delta_C/zone_VDF.dim_elem(num_elem,0),deux_tiers)
                                        + F_Elem(num_elem,1)*pow(delta_C/zone_VDF.dim_elem(num_elem,1),deux_tiers)
                                        + F_Elem(num_elem,2)*pow(delta_C/zone_VDF.dim_elem(num_elem,2),deux_tiers));

            }
        }
      else
        {
          // alors nb_points == 4
          // on ne tient pas compte de la composante perpendiculaire au plan (dir1,dir2) de la FST!!!
          for (num_elem=0; num_elem<nb_poly; num_elem++)
            {
              delta_C = l_(num_elem);
              F2[num_elem]  = un_demi*(F_Elem(num_elem,dir1)*pow(delta_C/zone_VDF.dim_elem(num_elem,dir1),deux_tiers)
                                       + F_Elem(num_elem,dir2)*pow(delta_C/zone_VDF.dim_elem(num_elem,dir2),deux_tiers));
            }
        }

      // On traite les bords pour completer la fonction de structure
      // sur les  mailles de bord

      int num_face;
      int elem,n0,n1;

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type

          const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
          if (sub_type(Dirichlet_entree_fluide,la_cl.valeur()) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                if ( (n0 = face_voisins(num_face,0)) != -1)
                  {
                    elem = zone_VDF.elem_voisin(n0,num_face,0);
                    F2[n0] = F2[elem] ;
                  }
                else
                  {
                    n1 = face_voisins(num_face,1);
                    elem = zone_VDF.elem_voisin(n1,num_face,1);
                    F2[n1] = F2[elem] ;
                  }
            }
          else if (sub_type(Neumann_sortie_libre,la_cl.valeur()) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                if ( (n0 = face_voisins(num_face,0)) != -1)
                  {
                    elem = zone_VDF.elem_voisin(n0,num_face,0);
                    F2[n0] = F2[elem] ;
                  }
                else
                  {
                    n1 = face_voisins(num_face,1);
                    elem = zone_VDF.elem_voisin(n1,num_face,1);
                    F2[n1] = F2[elem] ;
                  }
            }
          else if ( (sub_type(Symetrie,la_cl.valeur())) || (sub_type(Periodique,la_cl.valeur())) )
            {
              ;
            }
          else if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur())) )
            {
              ;
            }
          else
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                if ( (n0 = face_voisins(num_face,0)) != -1)
                  F2[n0] = 0;
                else
                  {
                    n1 = face_voisins(num_face,1);
                    F2[n1] = 0;
                  }
            }
        }
    }
  else
    {
      Cerr << "The structure fonction subgrid model can be used" << finl;
      Cerr << "only for dimesnion 3." << finl;
      exit();
    }

}


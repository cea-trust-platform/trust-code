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
// File:        Turbulence_hyd_sous_maille_axi_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Axi/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_axi_VDF.h>
#include <Periodique.h>
#include <Dirichlet_entree_fluide.h>
#include <Neumann_sortie_libre.h>
#include <Symetrie.h>
#include <DoubleTrav.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Equation_base.h>

Implemente_instanciable(Turbulence_hyd_sous_maille_axi_VDF,"Modele_turbulence_hyd_sous_maille_axi_VDF",Turbulence_hyd_sous_maille_VDF);

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_axi_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_axi_VDF::readOn(Entree& s )
{
  ////return Mod_turb_hyd_ss_maille::readOn(s) ;
  return Turbulence_hyd_sous_maille_VDF::readOn(s);

}


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_axi_VDF
//
//////////////////////////////////////////////////////////////////////////////


Champ_Fonc& Turbulence_hyd_sous_maille_axi_VDF::calculer_viscosite_turbulente()
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  static const double Csm1 = CSM1;
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  int nb_poly = zone_VDF.zone().nb_elem();
  int nb_poly_tot = la_zone_VDF->zone().nb_elem_tot();
  int numfa[6];
  double delta_C_axi ;
  double h_x,h_y,h_z;
  double un_tiers   = 1./3.;


  F2.resize(nb_poly_tot);
  calculer_fonction_structure();

  if (visco_turb.size() != nb_poly)
    {
      Cerr << "erreur dans la taille du DoubleTab valeurs de la viscosite" << finl;
      exit();
    }

  Debog::verifier("Turbulence_hyd_sous_maille_axi_VDF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  for (int elem=0; elem<nb_poly; elem++)
    {
      for (int i=0; i<6; i++)
        numfa[i] = elem_faces(elem,i);
      h_x = zone_VDF.dist_face_axi(numfa[0],numfa[3],0);
      h_y = zone_VDF.dist_face_axi(numfa[1],numfa[4],1);
      h_z = zone_VDF.dist_face_axi(numfa[2],numfa[5],2);
      // filter width by bardina
      // delta_C_axi = sqrt(h_x*h_x + h_y*h_y + h_z*h_z) ;
      // filter lesieur
      delta_C_axi = pow(h_x*h_y*h_z,un_tiers) ;
      visco_turb[elem] = Csm1*delta_C_axi*sqrt(F2[elem]);
    }

  Debog::verifier("Turbulence_hyd_sous_maille_axi_VDF::calculer_viscosite_turbulente visco_turb 1",visco_turb);


  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Turbulence_hyd_sous_maille_axi_VDF::calculer_fonction_structure()
{
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_poly = zone_VDF.zone().nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();
  DoubleTrav F_Elem(nb_poly,dimension);
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
      for (num_elem=0; num_elem<nb_poly; num_elem++)
        {
          diff = vitesse[elem_faces(num_elem,0)] - vitesse[elem_faces(num_elem,3)];
          F_Elem(num_elem,0) = diff*diff;
          diff = vitesse[elem_faces(num_elem,1)] - vitesse[elem_faces(num_elem,4)];
          F_Elem(num_elem,1) = diff*diff;
          diff = vitesse[elem_faces(num_elem,2)] - vitesse[elem_faces(num_elem,5)];
          F_Elem(num_elem,2) = diff*diff;
        }

      int ndeb0 = zone_VDF.premiere_arete_interne();
      int nfin0 = ndeb0 + zone_VDF.nb_aretes_internes();
      int num_arete0;

      for (num_arete0=ndeb0; num_arete0<nfin0; num_arete0++)
        {
          num0 = Qdm(num_arete0,0);
          num1 = Qdm(num_arete0,1);
          num2 = Qdm(num_arete0,2);
          num3 = Qdm(num_arete0,3);

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

      ndeb0 = zone_VDF.premiere_arete_mixte();
      nfin0 = ndeb0 + zone_VDF.nb_aretes_mixtes();

      for (num_arete0=ndeb0; num_arete0<nfin0; num_arete0++)
        {
          num0 = Qdm(num_arete0,0);
          num1 = Qdm(num_arete0,1);
          num2 = Qdm(num_arete0,2);
          num3 = Qdm(num_arete0,3);

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

      // ATTENTION!!!!!!!!!!!  Modifs periodicite      DEBUT
      //
      // Les aretes bords sont considerees comme des faces internes
      // par modification du tableau Qdm ( dans Zone_VDF.cpp )


      const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
      const int nb_cond_lim = zone_Cl_VDF.nb_cond_lim();

      for(int i=0; i<nb_cond_lim; i++)
        {
          const Cond_lim_base& cl = la_zone_Cl_VDF->les_conditions_limites(i).valeur();

          // Cerr << "les_conditions_limites(i).valeur() : " << cl << finl;

          if (sub_type(Periodique, cl))
            {
              //            const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();

              int ndeb = zone_VDF.premiere_arete_bord();
              int nfin = ndeb + zone_VDF.nb_aretes_bord();
              int num_arete;

              for (num_arete=ndeb; num_arete<nfin; num_arete++)
                {
                  int n_type=zone_Cl_VDF.type_arete_bord(num_arete-ndeb);

                  if (n_type == TypeAreteBordVDF::PERIO_PERIO) // arete de type periodicite
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

                }
            }
        }
      // ATTENTION!!!!!!!!!!!  Modifs periodicite       FIN


      // Calcul de la Fonction de structure a partir de ses
      // composantes directionnelles

      double un_tiers   = 1./3.;
      double deux_tiers = 2./3.;
      double delta_C_axi;
      double h_x,h_y,h_z;
      int numfa[6];

      for (num_elem=0; num_elem<nb_poly; num_elem++)
        {
          for (int i=0; i<6; i++)
            numfa[i] = elem_faces(num_elem,i);
          h_x = zone_VDF.dist_face_axi(numfa[0],numfa[3],0);
          h_y = zone_VDF.dist_face_axi(numfa[1],numfa[4],1);
          h_z = zone_VDF.dist_face_axi(numfa[2],numfa[5],2);
          // filter width by bardina
          // delta_C_axi = sqrt(h_x*h_x + h_y*h_y + h_z*h_z) ;
          // filter lesieur
          delta_C_axi = pow(h_x*h_y*h_z,un_tiers) ;
          F2[num_elem]  = un_tiers*(  F_Elem(num_elem,0)*pow(delta_C_axi/h_x,deux_tiers)
                                      + F_Elem(num_elem,1)*pow(delta_C_axi/h_y,deux_tiers)
                                      + F_Elem(num_elem,2)*pow(delta_C_axi/h_z,deux_tiers));
        }


      // On traite les bords pour completer la fonction de structure
      // sur les  mailles de bord
      // Rq: il est inutile de completer la fonction de structure
      // sur les mailles de bord qui correspondent aux conditions limites
      // de paroi puisque la viscosite turbulente est calculee a partir des
      // lois de paroi sur ces mailles et non a partir de la fonction de
      // structure

      int num_face;
      int elem,n0,n1;

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type

          const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
          if (sub_type(Dirichlet_entree_fluide,la_cl.valeur()) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb0 = le_bord.num_premiere_face();
              nfin0 = ndeb0 + le_bord.nb_faces();
              for (num_face=ndeb0; num_face<nfin0; num_face++)
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
              ndeb0 = le_bord.num_premiere_face();
              nfin0 = ndeb0 + le_bord.nb_faces();
              for (num_face=ndeb0; num_face<nfin0; num_face++)
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
          else if (sub_type(Symetrie,la_cl.valeur()) )
            {
              ;
            }
          else
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb0 = le_bord.num_premiere_face();
              nfin0 = ndeb0 + le_bord.nb_faces();
              for (num_face=ndeb0; num_face<nfin0; num_face++)
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
      Cerr << "Le modele sous maille fonction de structure" << finl;
      Cerr << "est utilisable uniquement en dimension 3" << finl;
      exit();
    }
}
void Turbulence_hyd_sous_maille_axi_VDF::calculer_longueurs_caracteristiques()
{
  //Ne fait rien
  //La longueur de filtre n'est pas utilisee pour calculer l'energie cinetique
  //de sous maille pour ce type de modele et n est pas utilisee non plus pour
  //calculer la viscosite turbulente
}


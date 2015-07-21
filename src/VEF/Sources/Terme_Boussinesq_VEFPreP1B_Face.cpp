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
// File:        Terme_Boussinesq_VEFPreP1B_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Boussinesq_VEFPreP1B_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Navier_Stokes_std.h>
#include <Zone_VEF_PreP1b.h>
#include <Domaine.h>
#include <Synonyme_info.h>

extern double calculer_coef_som(int elem, int& nb_face_diri, ArrOfInt& indice_diri, const Zone_Cl_VEF& zcl, const Zone_VEF& zone_VEF);

Implemente_instanciable(Terme_Boussinesq_VEFPreP1B_Face,"Boussinesq_VEFPreP1B_P1NC",Terme_Boussinesq_VEF_Face);
Add_synonym(Terme_Boussinesq_VEFPreP1B_Face,"Boussinesq_temperature_VEFPreP1B_P1NC");
Add_synonym(Terme_Boussinesq_VEFPreP1B_Face,"Boussinesq_concentration_VEFPreP1B_P1NC");

//// printOn
Sortie& Terme_Boussinesq_VEFPreP1B_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_VEFPreP1B_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

DoubleTab& Terme_Boussinesq_VEFPreP1B_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b, la_zone_VEF.valeur());
  // Si seulement support P0 on appelle en VEF
  if (zone_VEF.get_alphaE() && !zone_VEF.get_alphaS() && !zone_VEF.get_alphaA())
    return Terme_Boussinesq_VEF_Face::ajouter(resu);

  const DoubleVect& volumes = zone_VEF.volumes();
  const DoubleVect& porosite_surf = zone_VEF.porosite_face();
  const Champ_Inc& le_scalaire = equation_scalaire().inconnue();
  const DoubleVect& g = gravite().valeurs();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  const IntTab& elem_sommets = zone_VEF.zone().les_elems();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& coord_sommets=zone_VEF.zone().domaine().les_sommets();

  // Verifie la validite de T0:
  check();

  int nbpts=-1; // nombre de points d'integration

  if(dimension==2)
    {
      nbpts=3; // ordre 2
    }
  else if(dimension==3)
    {
      nbpts=4; // ordre 2
    }
  else
    assert(0);

  // On remplit les Poids et les coord_bary :
  ArrOfDouble Poids(nbpts);
  DoubleTab coord_bary(nbpts,dimension+1); // lambda_i des points
  if(dimension==2)
    {
      double tiers=0.333333333333333;
      Poids(0)=tiers;
      Poids(1)=Poids(2)=Poids(0);

      coord_bary(0,0)=0.5;
      coord_bary(0,1)=0.;
      coord_bary(0,2)=0.5;

      coord_bary(1,0)=0.;
      coord_bary(1,1)=0.5;
      coord_bary(1,2)=0.5;

      coord_bary(2,0)=0.5;
      coord_bary(2,1)=0.5;
      coord_bary(2,2)=0.;
    }
  else if(dimension==3)
    {
      Poids(0)=0.25;
      Poids(1)=Poids(2)=Poids(3)=Poids(0);

      double a = 0.5854101966249685;
      double b = 0.1381966011250105;

      coord_bary(0,0)=a;
      coord_bary(0,1)=b;
      coord_bary(0,2)=b;
      coord_bary(0,3)=b;

      coord_bary(1,0)=b;
      coord_bary(1,1)=a;
      coord_bary(1,2)=b;
      coord_bary(1,3)=b;

      coord_bary(2,1)=b;
      coord_bary(2,0)=b;
      coord_bary(2,2)=a;
      coord_bary(2,3)=b;

      coord_bary(3,2)=b;
      coord_bary(3,0)=b;
      coord_bary(3,1)=b;
      coord_bary(3,3)=a;
    }
  else
    assert(0);

  int nb_comp = le_scalaire.valeur().nb_comp(); // Vaut 1 si temperature, nb_constituents si concentration
  IntVect les_polygones(nbpts);
  DoubleTab les_positions(nbpts,dimension);
  DoubleTab valeurs_scalaire(nbpts,nb_comp);
  DoubleTab valeurs_beta(nbpts,nb_comp);
  DoubleVect valeurs_Psi(nbpts);
  ArrOfDouble somme(dimension);
  ArrOfDouble a0(dimension),a0a1(dimension),a0a2(dimension),a0a3(dimension);

  // Extension possible des volumes de controle:
  int modif_traitement_diri=( sub_type(Zone_VEF_PreP1b,zone_VEF) ? ref_cast(Zone_VEF_PreP1b,zone_VEF).get_modif_div_face_dirichlet() : 0);
  modif_traitement_diri = 0; // Forcee a 0 car ne marche pas d'apres essais Ulrich&Thomas
  int nb_face_diri=0;
  ArrOfInt indice_diri(dimension+1);

  // Boucle sur les elements:
  int nb_elem_tot=zone_VEF.nb_elem_tot();
  for (int elem=0; elem<nb_elem_tot; elem++)
    {
      if (modif_traitement_diri)
        calculer_coef_som(elem,nb_face_diri,indice_diri,zone_Cl_VEF,zone_VEF);

      double volume=volumes(elem);
      for (int i=0; i<nbpts; i++)
        les_polygones(i)=elem;

      //On remplit la matrice de changement d'element  :
      const int som_glob = elem_sommets(elem,0);
      for (int dim=0; dim<dimension; dim++)
        a0(dim)=coord_sommets(som_glob,dim);

      const int som_glob1 = elem_sommets(elem,1);
      for (int dim=0; dim<dimension; dim++)
        a0a1(dim)=coord_sommets(som_glob1,dim)-a0(dim);

      const int som_glob2 = elem_sommets(elem,2);
      for (int dim=0; dim<dimension; dim++)
        a0a2(dim)=coord_sommets(som_glob2,dim)-a0(dim);

      if(dimension == 3)
        {
          const int som_glob3 = elem_sommets(elem,3);
          for (int dim=0; dim<dimension; dim++)
            a0a3(dim)=coord_sommets(som_glob3,dim)-a0(dim);
        }

      //On remplit les_positions :
      if(dimension == 2)
        {
          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                les_positions(pt,dim)=a0(dim)
                                      +coord_bary(pt,1)* a0a1(dim)
                                      +coord_bary(pt,2)* a0a2(dim);
            }
        }
      else if(dimension == 3)
        {
          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                les_positions(pt,dim)=a0(dim)
                                      +coord_bary(pt,1)* a0a1(dim)
                                      +coord_bary(pt,2)* a0a2(dim)
                                      +coord_bary(pt,3)* a0a3(dim);
            }
        }
      else
        assert(0);

      // Calcul du terme source aux points d'integration :
      le_scalaire.valeur().valeur_aux_elems(les_positions,les_polygones,valeurs_scalaire);
      beta().valeur().valeur_aux_elems(les_positions,les_polygones,valeurs_beta);

      // Boucle sur les faces de l'element:
      for(int face=0; face<=dimension; face++)
        {
          int num_face=elem_faces(elem, face);

          // Calcul des Psi aux points d'integration :
          for (int pt=0; pt<nbpts; pt++)
            // valeurs_Psi(pt)=(1-dimension*coord_bary(pt,face))*porosite_surf(face); // Fixed bug
            valeurs_Psi(pt)=(1-dimension*coord_bary(pt,face))*porosite_surf(num_face);

          // Integration sur les nbpts points:
          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                {
                  double contrib=0;
                  for (int comp=0; comp<nb_comp; comp++)
                    contrib+=Poids(pt)*volume*(Scalaire0(comp)-valeurs_scalaire(pt,comp))*valeurs_beta(pt,comp)*g(dim)*valeurs_Psi(pt);
                  resu(num_face,dim)+=contrib;
                  somme(dim)+=contrib;
                  if (modif_traitement_diri)
                    {
                      for (int fdiri=0; fdiri<nb_face_diri; fdiri++)
                        {
                          int indice=indice_diri[fdiri];
                          int facel = elem_faces(elem,indice);
                          if (num_face==facel)
                            {
                              resu(facel,dim)-=contrib;
                              double contrib2=contrib/(dimension+1-nb_face_diri);
                              for (int f2=0; f2<dimension+1; f2++)
                                {
                                  int face2=elem_faces(elem,f2);
                                  resu(face2,dim)+=contrib2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  {
    // modif pour periodique
    for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
            int nb_faces_bord=le_bord.nb_faces();
            IntVect fait(nb_faces_bord);
            fait = 0;
            for (int ind_face=0; ind_face<nb_faces_bord; ind_face++)
              {
                if (fait(ind_face) == 0)
                  {
                    int ind_face_associee = la_cl_perio.face_associee(ind_face);
                    fait(ind_face) = 1;
                    fait(ind_face_associee) = 1;
                    int face = le_bord.num_face(ind_face);
                    int face_associee = le_bord.num_face(ind_face_associee);
                    for (int dim=0; dim<dimension; dim++)
                      resu(face, dim)=(resu(face_associee, dim)+=resu(face, dim));
                  }// if fait
              }// for face
          }// sub_type Perio
      }
  }

  // Verification:
  if (0)
    {
      Cout <<"Integrale : (";
      for (int dim=0; dim<dimension; ++dim)
        {
          Cout<<somme(dim);
          if (dim<(dimension-1))
            Cout<<",";
        }
      Cout<<")"<<finl;
      Process::exit();
    }
  return resu;
}

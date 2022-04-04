/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Terme_Source_Qdm_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Qdm_VEF_Face.h>
#include <Champ_Uniforme.h>
#include <Domaine.h>
#include <Zone_Cl_dis.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Periodique.h>
#include <BilanQdmVEF.h>
#include <Equation_base.h>
#include <Champ_Inc.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Zone_VEF_PreP1b.h>

extern double calculer_coef_som(int elem, int& nb_face_diri, ArrOfInt& indice_diri, const Zone_Cl_VEF& zcl, const Zone_VEF& zone_VEF);
Implemente_instanciable(Terme_Source_Qdm_VEF_Face,"Source_Qdm_VEF_P1NC",Source_base);



//// printOn
//

Sortie& Terme_Source_Qdm_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Qdm_VEF_Face::readOn(Entree& s )
{
  s >> la_source;
  if (la_source->nb_comp() != dimension)
    {
      Cerr << "Erreur a la lecture du terme source de type " << que_suis_je() << finl;
      Cerr << "le champ source doit avoir " << dimension << " composantes" << finl;
      exit();
    }
  return s ;
}

void Terme_Source_Qdm_VEF_Face::associer_pb(const Probleme_base& )
{
  ;
}

void Terme_Source_Qdm_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                               const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF, zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
}


DoubleTab& Terme_Source_Qdm_VEF_Face::ajouter(DoubleTab& resu) const
{
  int nb_comp=resu.line_size();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zone_Cl_VEF.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& elem_sommets = zone_VEF.zone().les_elems();
  const DoubleVect& volumes = zone_VEF.volumes();
  const DoubleTab& coord_sommets=zone_VEF.zone().domaine().les_sommets();
  ArrOfDouble a0(dimension),a0a1(dimension),a0a2(dimension), a0a3(dimension);
  int nb_elem_tot=zone_VEF.nb_elem_tot();
  double volume;
  DoubleTab resu_sauv(resu);
  resu = 0.;
  const int nbpts = (dimension==2) ? 7 : 15;


  // On remplit les Poids et les coord_bary :

  ArrOfDouble Poids(nbpts);
  DoubleTab coord_bary(nbpts,dimension+1); // lambda_i des points

  double tiers=0.333333333333333;
  if(dimension==2)
    {
      Poids[0]=0.225;
      Poids[1]=Poids[2]=Poids[3]=0.125939180544827;
      Poids[4]=Poids[5]=Poids[6]=0.132394152788506;

      coord_bary(0,0)=coord_bary(0,1)=coord_bary(0,2)=tiers;

      coord_bary(1,0)=0.797426985353087;
      coord_bary(1,1)=coord_bary(1,2)=0.101286507323456;
      coord_bary(2,1)=0.797426985353087;
      coord_bary(2,0)=coord_bary(2,2)=0.101286507323456;
      coord_bary(3,2)=0.797426985353087;
      coord_bary(3,0)=coord_bary(3,1)=0.101286507323456;

      coord_bary(4,0)=0.059715871789770;
      coord_bary(4,1)=coord_bary(4,2)=0.470142064105115;
      coord_bary(5,1)=0.059715871789770;
      coord_bary(5,0)=coord_bary(5,2)=0.470142064105115;
      coord_bary(6,2)=0.059715871789770;
      coord_bary(6,0)=coord_bary(6,1)=0.470142064105115;
    }
  else if(dimension==3)
    {
      Poids[0]=0.030283678097089;
      Poids[1]=Poids[2]=Poids[3]=Poids[4]=0.006026785714286;
      Poids[5]=Poids[6]=Poids[7]=Poids[8]=0.011645249086029;
      Poids[9]=Poids[10]=Poids[11]=
                           Poids[12]=Poids[13]=Poids[14]=0.010949141561386;
      // Pour que la somme soit 1.
      Poids*=6;

      coord_bary(0,0)=coord_bary(0,1)=coord_bary(0,2)
                                      =coord_bary(0,3)=0.250000000000000;

      coord_bary(1,0)=0.;
      coord_bary(1,1)=coord_bary(1,2)=coord_bary(1,3)=tiers;
      coord_bary(2,1)=0.;
      coord_bary(2,0)=coord_bary(2,2)=coord_bary(2,3)=tiers;
      coord_bary(3,2)=0.;
      coord_bary(3,0)=coord_bary(3,1)=coord_bary(3,3)=tiers;
      coord_bary(4,3)=0.;
      coord_bary(4,0)=coord_bary(4,1)=coord_bary(4,2)=tiers;


      coord_bary(5,0)=0.727272727272727;
      coord_bary(5,1)=coord_bary(5,2)=coord_bary(5,3)=0.090909090909091;
      coord_bary(6,1)=0.727272727272727;
      coord_bary(6,0)=coord_bary(6,2)=coord_bary(6,3)=0.090909090909091;
      coord_bary(7,2)=0.727272727272727;
      coord_bary(7,0)=coord_bary(7,1)=coord_bary(7,3)=0.090909090909091;
      coord_bary(8,3)=0.727272727272727;
      coord_bary(8,0)=coord_bary(8,1)=coord_bary(8,2)=0.090909090909091;

      coord_bary(9,0)=coord_bary(9,1)=0.066550153573664;
      coord_bary(9,2)=coord_bary(9,3)=0.433449846426336;

      coord_bary(10,0)=coord_bary(10,2)=0.066550153573664;
      coord_bary(10,1)=coord_bary(10,3)=0.433449846426336;

      coord_bary(11,0)=coord_bary(11,3)=0.066550153573664;
      coord_bary(11,1)=coord_bary(11,2)=0.433449846426336;

      coord_bary(12,1)=coord_bary(12,2)=0.066550153573664;
      coord_bary(12,0)=coord_bary(12,3)=0.433449846426336;

      coord_bary(13,1)=coord_bary(13,3)=0.066550153573664;
      coord_bary(13,0)=coord_bary(13,2)=0.433449846426336;

      coord_bary(14,2)=coord_bary(14,3)=0.066550153573664;
      coord_bary(14,0)=coord_bary(14,1)=0.433449846426336;
    }
  else
    assert(0);
  IntVect les_polygones(nbpts);
  DoubleTab les_positions(nbpts, dimension);
  DoubleTab valeurs_source(nbpts,dimension);
  DoubleTab valeurs_Psi(nbpts,dimension);
  ArrOfDouble somme(dimension);
  int nb_face_diri=0;
  ArrOfInt indice_diri(dimension+1);
  int modif_traitement_diri=0;
  if (sub_type(Zone_VEF_PreP1b,zone_VEF))
    modif_traitement_diri=ref_cast(Zone_VEF_PreP1b,zone_VEF).get_modif_div_face_dirichlet();
  for (int elem=0; elem<nb_elem_tot; elem++)
    {
      if (modif_traitement_diri)
        calculer_coef_som(elem,nb_face_diri,indice_diri,zone_Cl_VEF,zone_VEF);
      volume=volumes(elem);
      for (int i=0; i<nbpts; i++)
        les_polygones(i)=elem;

      //On remplit la matrice de changement d'element  :
      const int som_glob = elem_sommets(elem,0);
      for (int dim=0; dim<dimension; dim++)
        a0[dim]=coord_sommets(som_glob,dim);

      const int som_glob1 = elem_sommets(elem,1);
      for (int dim=0; dim<dimension; dim++)
        a0a1[dim]=coord_sommets(som_glob1,dim)-a0[dim];

      const int som_glob2 = elem_sommets(elem,2);
      for (int dim=0; dim<dimension; dim++)
        a0a2[dim]=coord_sommets(som_glob2,dim)-a0[dim];

      if(dimension == 3)
        {
          const int som_glob3 = elem_sommets(elem,3);
          for (int dim=0; dim<dimension; dim++)
            a0a3[dim]=coord_sommets(som_glob3,dim)-a0[dim];
        }

      //On remplit les_positions :
      if(dimension == 2)
        {
          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                les_positions(pt,dim)=a0[dim]
                                      +coord_bary(pt,1)* a0a1[dim]
                                      +coord_bary(pt,2)* a0a2[dim];
            }
        }
      else if(dimension == 3)
        {
          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                les_positions(pt,dim)=a0[dim]
                                      +coord_bary(pt,1)* a0a1[dim]
                                      +coord_bary(pt,2)* a0a2[dim]
                                      +coord_bary(pt,3)* a0a3[dim];
            }
        }
      else
        assert(0);

      // Calcul du terme source aux points d'integration :
      la_source.valeur().valeur_aux_elems(les_positions,les_polygones,
                                          valeurs_source);
      for(int face=0; face<=dimension; face++)
        {
          int num_face=elem_faces(elem, face);

          // Calcul des Psi aux points d'integration :

          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                valeurs_Psi(pt, dim)=1-dimension*coord_bary(pt,face);
            }
          // Integration!!

          for (int pt=0; pt<nbpts; pt++)
            {
              for (int dim=0; dim<dimension; dim++)
                {
                  double contrib=Poids[pt]*volume
                                 *valeurs_source(pt,dim)*valeurs_Psi(pt,dim);
                  resu(num_face, dim)+=contrib;
                  somme[dim]+=contrib;

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
                              //                              Cerr<<num_face<<" "<<elem<<" la "<< f2<<" "<<fdiri<<" "<<nb_face_diri<<" dim "<< dim<<finl;
                              int face2=elem_faces(elem,f2);
                              resu(face2,dim)+=contrib2;
                            }
                        }
                    }

                }
            }
        }
    }
  {
    // modif pour periodic
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
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        resu(face, comp)=(resu(face_associee, comp)+=resu(face, comp));
                      }
                  }// if fait
              }// for face
          }// sub_type Perio
      }
  }
  ArrOfDouble tab_bilan(nb_comp);
  BilanQdmVEF::bilan_qdm(resu, zone_Cl_VEF, tab_bilan);
  /*
    if (equation().schema_temps().limpr())
    {
    for (int comp=0; comp<nb_comp; comp++)
    {
    Cout << "Apres Source bilan["<<comp<<"]="<< bilan[comp]<<finl;
    }
    }*/

  BilanQdmVEF::bilan_energie(resu, equation().inconnue().valeurs(), zone_Cl_VEF, tab_bilan);
  /*
    if (equation().schema_temps().limpr())
    {
    for (int comp=0; comp<nb_comp; comp++)
    {
    Cout << "Apres Source bilan energie ["<<comp<<"]="<< bilan[comp]<<finl;
    }
    }*/
  resu += resu_sauv;
  return resu;
}

DoubleTab& Terme_Source_Qdm_VEF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Terme_Source_Qdm_VEF_Face::mettre_a_jour(double temps)
{
  la_source->mettre_a_jour(temps);
}


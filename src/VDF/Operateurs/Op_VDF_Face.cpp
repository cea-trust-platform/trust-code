/****************************************************************************
* Copyright (c) 2018, CEA
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
// File:        Op_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_VDF_Face.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Champ_Inc.h>


inline int face_bord_amont2(const Zone_VDF& la_zone, int num_face, int k, int i)
{
  int ori = la_zone.orientation(num_face);
  int elem = la_zone.face_voisins(num_face,0);
  if(elem != -1)
    {
      int face = la_zone.elem_faces(elem, k+i*Objet_U::dimension);
      int elem_bis = la_zone.face_voisins(face,i);
      if (elem_bis != -1)
        elem = la_zone.elem_faces(elem_bis, ori+Objet_U::dimension);
      else
        elem = -1;
    }
  else
    {
      Process::exit();
    }
  return elem;
}


void Op_VDF_Face::dimensionner(const Zone_VDF& la_zone,
                               const Zone_Cl_VDF& la_zone_cl,
                               Matrice_Morse& la_matrice) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.
  int ndeb = 0;
  int nfin = la_zone.nb_faces_tot();
  int dimension = Objet_U::dimension;

  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  int nb_comp = champ_inconnue.nb_dim() == 2 ? champ_inconnue.dimension(1) : 1;

  la_matrice.dimensionner(nfin*nb_comp,nfin*nb_comp,0);

  DoubleVect& coeff = la_matrice.get_set_coeff();
  coeff=0;
  IntVect rang_voisin(nfin);
  rang_voisin=1;
  const IntVect& orientation=la_zone.orientation();

  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      int ori=orientation(num_face);
      int face1=la_zone.face_amont_princ(num_face,0);
      int face2=la_zone.face_amont_princ(num_face,1);
      int face3=la_zone.face_bord_amont(num_face,(ori+1)%dimension,0);
      int face4=la_zone.face_bord_amont(num_face,(ori+1)%dimension,1);
      if(face1 > -1)
        (rang_voisin(num_face))++;
      if(face2 > -1)
        (rang_voisin(num_face))++;
      if(face3 > -1)
        (rang_voisin(num_face))++;
      if(face4 > -1)
        (rang_voisin(num_face))++;
      if (dimension==3)
        {
          int face5=la_zone.face_bord_amont(num_face,(ori+2)%dimension,0);
          int face6=la_zone.face_bord_amont(num_face,(ori+2)%dimension,1);

          if(face5 > -1)
            (rang_voisin(num_face))++;
          if(face6 > -1)
            (rang_voisin(num_face))++;
        }
    }
  //Cerr<<"nbre max de voisin "<<max(rang_voisin)<<" "<<rang_voisin(0)<<finl;

  //Cerr<<"nbre max de voisin "<<max(rang_voisin)<<" "<<rang_voisin(0)<<finl;
  // on dimensionne tab1 et tab2 au nombre de faces

  IntVect& tab1=la_matrice.get_set_tab1();
  IntVect& tab2=la_matrice.get_set_tab2();
  tab1(0)=1;
  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      for (int k=0; k< nb_comp; k++)
        tab1(num_face*nb_comp+1+k)=rang_voisin(num_face)+tab1(num_face*nb_comp+k);
    }

  la_matrice.dimensionner(nfin*nb_comp,tab1(nfin*nb_comp)-1);

  for (int num_face=ndeb; num_face<nfin; num_face++)
    {
      int ori=orientation(num_face);
      int face1=la_zone.face_amont_princ(num_face,0);
      int face2=la_zone.face_amont_princ(num_face,1);
      int face3=la_zone.face_bord_amont(num_face,(ori+1)%dimension,0);
      int face4=la_zone.face_bord_amont(num_face,(ori+1)%dimension,1);

      int cpt=tab1[num_face]-1;
      tab2[cpt]=num_face+1;
      cpt++;

      if(face1 > -1)
        {
          tab2[cpt]=face1+1;
          cpt++;
        }
      if(face2 > -1)
        {
          tab2[cpt]=face2+1;
          cpt++;
        }
      if(face3 > -1)
        {
          tab2[cpt]=face3+1;
          cpt++;
        }
      if(face4 > -1)
        {
          tab2[cpt]=face4+1;
          cpt++;
        }
      if (dimension==3)
        {
          int face5=la_zone.face_bord_amont(num_face,(ori+2)%dimension,0);
          int face6=la_zone.face_bord_amont(num_face,(ori+2)%dimension,1);

          if(face5 > -1)
            {
              tab2[cpt]=face5+1;
              cpt++;
            }
          if(face6 > -1)
            {
              tab2[cpt]=face6+1;
              cpt++;
            }
        }
    }

  // on traite la condition de periodicite
  // en effet ce n'est pas sur que les faces de frontiere soient les bonnes
  // plus precisement a droite on a la face de gauche et non celle de droite
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  const IntTab& faces_voisins=la_zone.face_voisins();
  const IntTab& elem_faces=la_zone.elem_faces();
  for (int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb_p = la_front_dis.num_premiere_face();
          int nfaces = la_front_dis.nb_faces();
          int nfin_p=ndeb_p+nfaces;

          //fait = 0;
          for (int num_face=ndeb_p; num_face<nfin_p; num_face++)
            {
              int ori=orientation(num_face);

              {
                //  fait[ind_face_local] = 1;
                //fait[la_cl_perio.face_associee(ind_face_local)] = 1;
                //int face1=la_zone.face_amont_princ(num_face,0);
                //int face2=la_zone.face_amont_princ(num_face,1);
                int face3=la_zone.face_bord_amont(num_face,(ori+1)%dimension,0);
                int face4=la_zone.face_bord_amont(num_face,(ori+1)%dimension,1);

                int face1b=elem_faces(faces_voisins(num_face,1),ori);

                int cpt=tab1[num_face]-1;
                cpt+=3;
                if (face1b!=num_face)
                  {
                    // on recalcule fac3 fac4
                    int face3n=face_bord_amont2(la_zone,num_face,(ori+1)%dimension,0);
                    int face4n=face_bord_amont2(la_zone,num_face,(ori+1)%dimension,1);

                    if (face3!=-1)
                      {
                        //int face3n=la_cl_perio.face_associee(face3-ndeb_p)+ndeb_p;
                        // on remplace face3 pae face3n dans tab2
                        assert(tab2[cpt]==face3+1);
                        tab2[cpt]=face3n+1;
                        cpt++;
                      }
                    if (face4!=-1)
                      {
                        //int face4n=la_cl_perio.face_associee(face4-ndeb_p)+ndeb_p;
                        // on remplace face4 pae face4n dans tab2
                        assert(tab2[cpt]==face4+1);
                        tab2[cpt]=face4n+1;
                        cpt++;
                      }

                    if (dimension==3)
                      {
                        int face5=la_zone.face_bord_amont(num_face,(ori+2)%dimension,0);
                        int face6=la_zone.face_bord_amont(num_face,(ori+2)%dimension,1);
                        int face5n=face_bord_amont2(la_zone,num_face,(ori+2)%dimension,0);
                        int face6n=face_bord_amont2(la_zone,num_face,(ori+2)%dimension,1);

                        if (face5!=-1)
                          {
                            //int face5n=la_cl_perio.face_associee(face5-ndeb_p)+ndeb_p;
                            // on remplace face4 pae face4n dans tab2
                            assert(tab2[cpt]==face5+1);
                            tab2[cpt]=face5n+1;
                            cpt++;
                          }
                        if (face6!=-1)
                          {
                            //int face6n=la_cl_perio.face_associee(face6-ndeb_p)+ndeb_p;
                            // on remplace face4 pae face4n dans tab2
                            assert(tab2[cpt]==face6+1);
                            tab2[cpt]=face6n+1;
                            cpt++;
                          }
                      }
                  }
              }
            }
        }
    }
  /*
    coeff=1;
    Cout<<"iciMM"<<finl;
    la_matrice.imprimer_formatte(Cout);
    coeff=0;
    Cerr<<la_zone.xv()<<finl;
  */
}

void Op_VDF_Face::modifier_pour_Cl(const Zone_VDF& la_zone,
                                   const Zone_Cl_VDF& la_zone_cl,
                                   Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  //  Cerr << " dans modifier_pour_cl " << finl;
  int  face, i, k,nbvois;
  //  int nfin = la_zone.nb_faces();
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();

  IntVect& tab1=la_matrice.get_set_tab1();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  const IntVect& orientation=la_zone.orientation();
  int nb_comp = 1;

  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  if (champ_inconnue.nb_dim() == 2) nb_comp = champ_inconnue.dimension(1);

  // Prise en compte des conditions de type periodicite

  for (i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];

      if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& la_cl_Dirichlet = ref_cast(Dirichlet,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int numdeb = la_front_dis.num_premiere_face();
          int nfaces = la_front_dis.nb_faces();

          for (face=numdeb; face < (numdeb + nfaces); face++)
            {
              for (int comp=0; comp<nb_comp; comp++)
                {
                  int idiag = tab1[face*nb_comp+comp]-1;
                  coeff[idiag]=1;

                  // pour les voisins

                  nbvois = tab1[face*nb_comp+1+comp] - tab1[face*nb_comp+comp];

                  for (k=1; k < nbvois; k++)
                    {
                      coeff[idiag+k]=0;
                    }
                  // pour le second membre
                  // Correction erreur (10/99):
                  // WEC : correction numero de face
                  int ori = orientation(face);
                  secmem[face*nb_comp+comp] = la_cl_Dirichlet.val_imp(face-numdeb,ori);
                }
            }
        }
      if (sub_type(Symetrie,la_cl.valeur()))
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int numdeb = la_front_dis.num_premiere_face();
          int nfaces = la_front_dis.nb_faces();

          for (face=numdeb; face < numdeb + nfaces; face++)
            {
              for (int comp=0; comp<nb_comp; comp++)
                {
                  int idiag = tab1[face*nb_comp+comp]-1;
                  coeff[idiag]=1;

                  // pour les voisins

                  nbvois = tab1[face*nb_comp+1+comp] - tab1[face*nb_comp+comp];

                  for (k=1; k < nbvois; k++)
                    {
                      coeff[idiag+k]=0;
                    }
                  secmem[face*nb_comp+comp] = 0;
                }
            }
        }
      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          const Dirichlet_homogene& la_cl_Dirichlet_homogene = ref_cast(Dirichlet_homogene,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int numdeb = la_front_dis.num_premiere_face();
          int nfaces = la_front_dis.nb_faces();

          for (face=numdeb; face < numdeb + nfaces; face++)
            {
              for (int comp=0; comp<nb_comp; comp++)
                {
                  int idiag = tab1[face*nb_comp+comp]-1;
                  coeff[idiag]=1;

                  // pour les voisins

                  nbvois = tab1[face*nb_comp+1+comp] - tab1[face*nb_comp+comp];

                  for (k=1; k < nbvois; k++)
                    {
                      coeff[idiag+k]=0;
                    }

                  // pour le second membre
                  // Correction erreur (10/99) :
                  // WEC : correction numero de face
                  int ori = orientation(face);
                  secmem[face*nb_comp+comp] = la_cl_Dirichlet_homogene.val_imp(face-numdeb,ori);
                }
            }
        }
    }
}


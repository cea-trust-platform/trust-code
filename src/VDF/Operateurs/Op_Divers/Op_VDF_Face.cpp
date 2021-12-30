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
// File:        Op_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Divers
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Dirichlet_homogene.h>
#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Op_VDF_Face.h>
#include <Zone_Cl_VDF.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Symetrie.h>

void Op_VDF_Face::dimensionner(const Zone_VDF& la_zone, const Zone_Cl_VDF& la_zone_cl, Matrice_Morse& la_matrice) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.

  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  const int ndeb = 0, nfin = la_zone.nb_faces_tot(), dimension = Objet_U::dimension, nb_comp = champ_inconnue.line_size();
  const IntVect& orientation = la_zone.orientation();

  la_matrice.dimensionner(nfin*nb_comp,nfin*nb_comp,0);

  IntVect& tab1 = la_matrice.get_set_tab1(), &tab2 = la_matrice.get_set_tab2();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  coeff = 0;
  IntVect rang_voisin(nfin);
  rang_voisin = 1;

  for (int num_face = ndeb; num_face < nfin; num_face++)
    {
      const int ori = orientation(num_face), face1 = la_zone.face_amont_princ(num_face,0), face2 = la_zone.face_amont_princ(num_face,1),
                face3 = la_zone.face_bord_amont(num_face,(ori+1)%dimension,0), face4 = la_zone.face_bord_amont(num_face,(ori+1)%dimension,1);

      if(face1 > -1) (rang_voisin(num_face))++;
      if(face2 > -1) (rang_voisin(num_face))++;
      if(face3 > -1) (rang_voisin(num_face))++;
      if(face4 > -1) (rang_voisin(num_face))++;

      if (dimension==3)
        {
          const int face5 = la_zone.face_bord_amont(num_face,(ori+2)%dimension,0), face6 = la_zone.face_bord_amont(num_face,(ori+2)%dimension,1);
          if(face5 > -1) (rang_voisin(num_face))++;
          if(face6 > -1) (rang_voisin(num_face))++;
        }
    }

  // on balaye les faces pour dimensionner tab1 et tab2
  tab1(0) = 1;
  for (int num_face = ndeb; num_face < nfin; num_face++) for (int k = 0; k < nb_comp; k++) tab1(num_face*nb_comp+1+k) = rang_voisin(num_face) + tab1(num_face*nb_comp+k);

  la_matrice.dimensionner(nfin*nb_comp,tab1(nfin*nb_comp)-1);

  for (int num_face = ndeb; num_face < nfin; num_face++)
    {
      const int ori = orientation(num_face), face1 = la_zone.face_amont_princ(num_face,0), face2 = la_zone.face_amont_princ(num_face,1),
                face3 = la_zone.face_bord_amont(num_face,(ori+1)%dimension,0), face4 = la_zone.face_bord_amont(num_face,(ori+1)%dimension,1);

      int cpt = tab1[num_face]-1;
      tab2[cpt] = num_face+1;
      cpt++;

      dimensionner_(face1,cpt,tab2);
      dimensionner_(face2,cpt,tab2);
      dimensionner_(face3,cpt,tab2);
      dimensionner_(face4,cpt,tab2);

      if (dimension == 3)
        {
          const int face5 = la_zone.face_bord_amont(num_face,(ori+2)%dimension,0), face6 = la_zone.face_bord_amont(num_face,(ori+2)%dimension,1);
          dimensionner_(face5,cpt,tab2);
          dimensionner_(face6,cpt,tab2);
        }
    }

  // on traite la condition de periodicite : en effet ce n'est pas sur que les faces de frontiere soient les bonnes
  // plus precisement a droite on a la face de gauche et non celle de droite
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  const IntTab& faces_voisins = la_zone.face_voisins(), &elem_faces = la_zone.elem_faces();

  for (int i = 0; i < les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int ndeb_p = la_front_dis.num_premiere_face(), nfaces = la_front_dis.nb_faces(), nfin_p = ndeb_p + nfaces;

          for (int num_face = ndeb_p; num_face < nfin_p; num_face++)
            {
              const int ori = orientation(num_face);
              {
                //int face1=la_zone.face_amont_princ(num_face,0);
                //int face2=la_zone.face_amont_princ(num_face,1);
                const int face3 = la_zone.face_bord_amont(num_face,(ori+1)%dimension,0), face4 = la_zone.face_bord_amont(num_face,(ori+1)%dimension,1);
                const int face1b = elem_faces(faces_voisins(num_face,1),ori);

                int cpt = tab1[num_face]-1;
                cpt += 3;
                if (face1b != num_face)
                  {
                    // on recalcule fac3 fac4
                    const int face3n = face_bord_amont2(la_zone,num_face,(ori+1)%dimension,0), face4n = face_bord_amont2(la_zone,num_face,(ori+1)%dimension,1);
                    dimensionner_(face3,face3n,cpt,tab2);
                    dimensionner_(face4,face4n,cpt,tab2);

                    if (dimension == 3)
                      {
                        const int face5 = la_zone.face_bord_amont(num_face,(ori+2)%dimension,0), face6 = la_zone.face_bord_amont(num_face,(ori+2)%dimension,1),
                                  face5n = face_bord_amont2(la_zone,num_face,(ori+2)%dimension,0), face6n = face_bord_amont2(la_zone,num_face,(ori+2)%dimension,1);

                        dimensionner_(face5,face5n,cpt,tab2);
                        dimensionner_(face6,face6n,cpt,tab2);
                      }
                  }
              }
            }
        }
    }
  // la_matrice.imprimer_formatte(Cout);
}

void Op_VDF_Face::modifier_pour_Cl_(const int face, const int comp, const int nb_comp, Matrice_Morse& la_matrice) const
{
  IntVect& tab1=la_matrice.get_set_tab1();
  DoubleVect& coeff = la_matrice.get_set_coeff();

  const int idiag = tab1[face*nb_comp+comp]-1;
  coeff[idiag] = 1;

  // pour les voisins
  const int nbvois = tab1[face*nb_comp+1+comp] - tab1[face*nb_comp+comp];
  for (int k = 1; k < nbvois; k++) coeff[idiag+k] = 0;
}

void Op_VDF_Face::modifier_pour_Cl(const Zone_VDF& la_zone, const Zone_Cl_VDF& la_zone_cl, Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  const IntVect& orientation = la_zone.orientation();
  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  const int nb_comp = champ_inconnue.line_size();

  // Prise en compte des conditions de type periodicite
  for (int i = 0; i < les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int numdeb = la_front_dis.num_premiere_face(), nfaces = la_front_dis.nb_faces();

      if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& la_cl_Dirichlet = ref_cast(Dirichlet,la_cl.valeur());

          for (int face = numdeb; face < (numdeb + nfaces); face++)
            for (int comp = 0; comp < nb_comp; comp++)
              {
                modifier_pour_Cl_(face,comp,nb_comp,la_matrice);
                // pour le second membre [Correction erreur (10/99) : WEC : correction numero de face]
                const int ori = orientation(face);
                secmem[face*nb_comp+comp] = la_cl_Dirichlet.val_imp(face-numdeb,ori);
              }
        }

      if (sub_type(Symetrie,la_cl.valeur()))
        {
          for (int face = numdeb; face < numdeb + nfaces; face++)
            for (int comp=0; comp<nb_comp; comp++)
              {
                modifier_pour_Cl_(face,comp,nb_comp,la_matrice);
                secmem[face*nb_comp+comp] = 0;
              }
        }

      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          const Dirichlet_homogene& la_cl_Dirichlet_homogene = ref_cast(Dirichlet_homogene,la_cl.valeur());

          for (int face = numdeb; face < numdeb + nfaces; face++)
            for (int comp = 0; comp < nb_comp; comp++)
              {
                modifier_pour_Cl_(face,comp,nb_comp,la_matrice);
                // pour le second membre [Correction erreur (10/99) : WEC : correction numero de face]
                const int ori = orientation(face);
                secmem[face*nb_comp+comp] = la_cl_Dirichlet_homogene.val_imp(face-numdeb,ori);
              }
        }
    }
}

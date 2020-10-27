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
// File:        Op_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs/Conv_iterateur
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_CoviMAC_Elem.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Periodique.h>
#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Champ_Inc.h>
#include <Matrix_tools.h>
#include <Array_tools.h>

void Op_CoviMAC_Elem::dimensionner(const Zone_CoviMAC& la_zone,
                                   const Zone_Cl_CoviMAC& la_zone_cl,
                                   Matrice_Morse& la_matrice) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des elements.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.

  int num_face,face,k;
  int n1 = la_zone.zone().nb_elem_tot(), n2 = la_zone.nb_faces_tot();
  int elem1,elem2, i;
  const IntTab& face_voisins = la_zone.face_voisins();
  //  const DoubleVect& face_surfaces = la_zone.face_surfaces();
  //  const DoubleVect& volumes_entrelaces = la_zone.volumes_entrelaces();
  //  const DoubleVect& porosite_face = la_zone.porosite_face();
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  int nb_comp = 1;

  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  if (champ_inconnue.nb_dim() == 2) nb_comp = champ_inconnue.dimension(1);
  //Cerr << "nb_compo de Op_CoviMAC_Elem::dimensionner" << nb_comp << finl;
  //Cerr << " nombre d'elements de Op_CoviMAC_Elem::dimensionner" << n1 << finl;

  la_matrice.dimensionner((n1 + n2) * nb_comp, (n1 + n2) * nb_comp, 0);

  IntVect& tab1=la_matrice.get_set_tab1();
  IntVect& tab2=la_matrice.get_set_tab2();

  int ndeb = la_zone.premiere_face_int();
  int nfin = la_zone.nb_faces();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  coeff=0;

  IntVect rang_voisin(n1*nb_comp);
  rang_voisin=1;

  for (num_face=ndeb; num_face<nfin; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);
      (rang_voisin(elem2))++;
      (rang_voisin(elem1))++;
    }

  // Prise en compte des conditions de type periodicite

  for (i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int numdeb = la_front_dis.num_premiere_face();
          int nfaces = la_front_dis.nb_faces();
          int ind_face_global;
          IntVect fait(nfaces);
          fait = 0;
          for (face=0; face<nfaces; face++)
            {
              if (fait[face] == 0)
                {
                  fait[face] = 1;
                  fait[la_cl_perio.face_associee(face)] = 1;
                  ind_face_global = face+numdeb;

                  elem1 = face_voisins(ind_face_global,0);
                  elem2 = face_voisins(ind_face_global,1);

                  (rang_voisin(elem2))++;
                  (rang_voisin(elem1))++;
                }
            }
        }
    }

  // on balaye les elements pour dimensionner tab1 et tab2

  tab1(0)=1;
  for(i=0; i<n1; i++)
    for (k=0; k< nb_comp; k++)
      tab1(i*nb_comp+1+k)=rang_voisin(i) +  tab1(i*nb_comp+k);
  for(i = n1; i < n1 + n2; i++)
    for (k = 0; k < nb_comp; k++)
      tab1(i*nb_comp+1+k)=tab1(i*nb_comp+k);

  //  Cerr << " dimension de la matrice " << n1*nb_comp
  //    << "   " << tab1(n1*nb_comp)-1 <<finl;
  la_matrice.dimensionner((n1 + n2) * nb_comp,tab1(n1*nb_comp)-1);

  for(i=0; i<n1*nb_comp; i++)
    {
      tab2[tab1[i]-1]=i+1;
      rang_voisin[i]=tab1[i];
    }

  // on traite les faces internes pour les voisins

  for (num_face=ndeb; num_face<nfin; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (k=0; k<nb_comp; k++)
        {
          tab2[rang_voisin[elem2*nb_comp+k]]=elem1*nb_comp+1+k;
          rang_voisin[elem2*nb_comp+k]++;

          tab2[rang_voisin[elem1*nb_comp+k]]=elem2*nb_comp+1+k;
          rang_voisin[elem1*nb_comp+k]++;
        }
    }
  // Cerr << "tab2 = " << tab2 << finl;
  // on traite la condition de periodicite
  for (i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      if (sub_type(Periodique,la_cl.valeur()) )
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int numdeb = la_front_dis.num_premiere_face();
          int nfaces = la_front_dis.nb_faces();
          int ind_face_local;
          IntVect fait(nfaces);
          fait = 0;
          for (ind_face_local=0; ind_face_local<nfaces; ind_face_local++)
            {
              //              ind_face_local = num_face - ndeb;
              if (fait[ind_face_local] == 0)
                {
                  num_face = numdeb + ind_face_local;
                  fait[ind_face_local] = 1;
                  fait[la_cl_perio.face_associee(ind_face_local)] = 1;

                  elem1 = face_voisins(num_face,0);
                  elem2 = face_voisins(num_face,1);

                  for (k=0; k<nb_comp; k++)
                    {
                      tab2[rang_voisin[elem2*nb_comp+k]]=elem1*nb_comp+1+k;
                      rang_voisin[elem2*nb_comp+k]++;

                      tab2[rang_voisin[elem1*nb_comp+k]]=elem2*nb_comp+1+k;
                      rang_voisin[elem1*nb_comp+k]++;
                    }
                }
            }
        }
    }
  // Cerr << "tab2 = " << tab2 << finl;
}

void Op_CoviMAC_Elem::dimensionner_bloc_vitesse(const Zone_CoviMAC& la_zone,
                                                const Zone_Cl_CoviMAC& la_zone_cl,
                                                Matrice_Morse& matrice) const
{

  const int nb_faces_tot = la_zone.nb_faces_tot(), nb_elem_tot = la_zone.nb_elem_tot();
  int e;
  IntTab stencyl(0,2);
  stencyl.set_smart_resize(1);
  const IntTab& f_e = la_zone.face_voisins();

  for (int f = 0; f < la_zone.nb_faces(); f++) for (int i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      stencyl.append_line(e, f);

  tableau_trier_retirer_doublons(stencyl);
  Matrix_tools::allocate_morse_matrix(nb_elem_tot, nb_faces_tot + nb_elem_tot * Objet_U::dimension, stencyl, matrice);

}

void Op_CoviMAC_Elem:: modifier_pour_Cl(const Zone_CoviMAC& la_zone,
                                        const Zone_Cl_CoviMAC& la_zone_cl,
                                        Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.

  //  int nfin = la_zone.nb_faces();
  //  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  //  const IntVect& orientation=la_zone.orientation();

  // Prise en compte des conditions de type periodicite
  //Cerr << "dans Op_CoviMAC_Elem:: modifier_pour_Cl" << finl;

}

/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Matrix_tools.h>
#include <Op_VDF_Elem.h>
#include <Domaine_Cl_VDF.h>
#include <Array_tools.h>
#include <Periodique.h>

#include <Domaine_VDF.h>

void Op_VDF_Elem::dimensionner(const Domaine_VDF& le_dom, const Domaine_Cl_VDF& le_dom_cl, Matrice_Morse& la_matrice) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de la convection, de la diffusion pour le cas des elements.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.

  const IntTab& face_voisins = le_dom.face_voisins();
  const Conds_lim& les_cl = le_dom_cl.les_conditions_limites();
  const DoubleTab& champ_inconnue = le_dom_cl.equation().inconnue().valeurs();
  const int n1 = le_dom.domaine().nb_elem_tot(), nb_comp = champ_inconnue.line_size();

  la_matrice.dimensionner(n1*nb_comp, n1*nb_comp, 0);

  IntVect& tab1 = la_matrice.get_set_tab1(), &tab2 = la_matrice.get_set_tab2();
  DoubleVect& coeff = la_matrice.get_set_coeff();

  const int ndeb = le_dom.premiere_face_int(), nfin = le_dom.nb_faces();
  coeff = 0;

  IntVect rang_voisin(n1*nb_comp);
  rang_voisin = 1;

  for (int num_face = ndeb; num_face < nfin; num_face++)
    {
      const int elem1 = face_voisins(num_face,0), elem2 = face_voisins(num_face,1);
      (rang_voisin(elem2))++;
      (rang_voisin(elem1))++;
    }

  // Prise en compte des conditions de type periodicite
  for (const auto& itr : les_cl)
    {
      const Cond_lim& la_cl = itr;
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int numdeb = la_front_dis.num_premiere_face(), nfaces = la_front_dis.nb_faces();
          int ind_face_global;
          IntVect fait(nfaces);
          fait = 0;
          for (int face = 0; face < nfaces; face++)
            {
              if (fait[face] == 0)
                {
                  fait[face] = 1;
                  fait[la_cl_perio.face_associee(face)] = 1;
                  ind_face_global = face+numdeb;

                  const int elem1 = face_voisins(ind_face_global,0), elem2 = face_voisins(ind_face_global,1);

                  (rang_voisin(elem2))++;
                  (rang_voisin(elem1))++;
                }
            }
        }
    }

  // on balaye les elements pour dimensionner tab1 et tab2
  tab1(0) = 1;
  for(int i = 0; i < n1; i++)
    for (int k = 0; k < nb_comp; k++) tab1(i*nb_comp+1+k) = rang_voisin(i) +  tab1(i*nb_comp+k);

  la_matrice.dimensionner(n1*nb_comp,tab1(n1*nb_comp)-1);

  for (int i = 0; i < n1*nb_comp; i++)
    {
      tab2[tab1[i]-1] = i+1;
      rang_voisin[i] = tab1[i];
    }

  // on traite les faces internes pour les voisins
  for (int num_face = ndeb; num_face < nfin; num_face++)
    {
      const int elem1 = face_voisins(num_face,0), elem2 = face_voisins(num_face,1);
      for (int k = 0; k < nb_comp; k++)
        {
          tab2[rang_voisin[elem2*nb_comp+k]] = elem1*nb_comp+1+k;
          rang_voisin[elem2*nb_comp+k]++;

          tab2[rang_voisin[elem1*nb_comp+k]] = elem2*nb_comp+1+k;
          rang_voisin[elem1*nb_comp+k]++;
        }
    }

  // on traite la condition de periodicite
  for (int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      if (sub_type(Periodique,la_cl.valeur()) )
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int numdeb = la_front_dis.num_premiere_face(), nfaces = la_front_dis.nb_faces();
          IntVect fait(nfaces);
          fait = 0;
          for (int ind_face_local = 0; ind_face_local < nfaces; ind_face_local++)
            {
              if (fait[ind_face_local] == 0)
                {
                  const int num_face = numdeb + ind_face_local;
                  fait[ind_face_local] = 1;
                  fait[la_cl_perio.face_associee(ind_face_local)] = 1;

                  const int elem1 = face_voisins(num_face,0), elem2 = face_voisins(num_face,1);

                  for (int k = 0; k < nb_comp; k++)
                    {
                      tab2[rang_voisin[elem2*nb_comp+k]] = elem1*nb_comp+1+k;
                      rang_voisin[elem2*nb_comp+k]++;

                      tab2[rang_voisin[elem1*nb_comp+k]] = elem2*nb_comp+1+k;
                      rang_voisin[elem1*nb_comp+k]++;
                    }
                }
            }
        }
    }
}

void Op_VDF_Elem:: modifier_pour_Cl(const Domaine_VDF& , const Domaine_Cl_VDF& , Matrice_Morse& , DoubleTab& ) const { /* Do nothing */ }

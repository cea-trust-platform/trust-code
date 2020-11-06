/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Appliquer_cl.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_VF.h>
#include <Zone_Cl_dis_base.h>
#include <Matrice_Morse.h>
#include <DoubleTab.h>
#include <Equation_base.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>

#include <Symetrie.h>

void modifier_pour_Cl_je_ne_sais_pas_ou_factoriser_cela(const Zone_dis_base& la_zone,
                                                        const Zone_Cl_dis_base& la_zone_cl,
                                                        Matrice_Morse& la_matrice, DoubleTab& inco, DoubleTab& secmem)
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();
  const IntVect& tab1=la_matrice.get_tab1();
  const IntVect& tab2=la_matrice.get_tab2();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  int nb_comp = 1;
  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  if (champ_inconnue.nb_dim() == 2) nb_comp = champ_inconnue.dimension(1);
  ArrOfDouble normale(nb_comp);
  int size = les_cl.size();
  for (int i=0; i<size; i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& la_cl_Dirichlet = ref_cast(Dirichlet,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const Zone_VF& ma_zone_VF =  ref_cast(Zone_VF, la_zone);
          int nfaces = la_front_dis.nb_faces();
          int numdeb = la_front_dis.num_premiere_face();
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int k = tab1[face*nb_comp+comp] - 1; k < tab1[face*nb_comp+1+comp] - 1; k++)
                    coeff[k] = (tab2[k] - 1 == face);
                  // pour le second membre
                  for (int j = secmem[face*nb_comp+comp] = 0; j < Objet_U::dimension; j++)
                    secmem[face*nb_comp+comp] += la_cl_Dirichlet.val_imp(face-numdeb,j) * ma_zone_VF.face_normales(face, j) / ma_zone_VF.face_surfaces(face);
                  secmem[face*nb_comp+comp] -= inco[face*nb_comp+comp];
                  // if (nb_comp == 1)
                  //   secmem(face) = la_cl_Dirichlet.val_imp(ind_face,0);
                  // else
                  //   secmem(face,comp)= la_cl_Dirichlet.val_imp(ind_face,comp);
                }
            }
        }
      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          const Dirichlet_homogene& la_cl_Dirichlet_homogene = ref_cast(Dirichlet_homogene,la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          const Zone_VF& ma_zone_VF =  ref_cast(Zone_VF, la_zone);
          int nfaces = la_front_dis.nb_faces();
          int numdeb = la_front_dis.num_premiere_face();
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int k = tab1[face*nb_comp+comp] - 1; k < tab1[face*nb_comp+1+comp] - 1; k++)
                    coeff[k] = (tab2[k] - 1 == face);
                  // pour le second membre
                  for (int j = secmem[face*nb_comp+comp] = 0; j < Objet_U::dimension; j++)
                    secmem[face*nb_comp+comp] += la_cl_Dirichlet_homogene.val_imp(face-numdeb,j) * ma_zone_VF.face_normales(face, j) / ma_zone_VF.face_surfaces(face);
                  secmem[face*nb_comp+comp] -= inco[face*nb_comp+comp];
                  // if (nb_comp == 1)
                  //   secmem(face) = la_cl_Dirichlet_homogene.val_imp(ind_face,0);
                  // else
                  //   secmem(face,comp)= la_cl_Dirichlet_homogene.val_imp(ind_face,comp);
                }
            }
        }
      if (sub_type(Symetrie,la_cl.valeur()))
        {
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nfaces = la_front_dis.nb_faces();
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int k = tab1[face*nb_comp+comp] - 1; k < tab1[face*nb_comp+1+comp] - 1; k++)
                    coeff[k] = (tab2[k] - 1 == face);
                  // pour le second membre
                  secmem[face*nb_comp+comp] = 0;
                }
            }
        }
#if 0
      if (sub_type(Symetrie,la_cl.valeur()))
        if (la_zone_cl.equation().inconnue().valeur().nature_du_champ()==vectoriel)
          {
            abort();
            const IntVect& tab2=la_matrice.tab2_;
            const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
            const DoubleTab& face_normales = la_zone.face_normales();
            int nfaces = la_front_dis.nb_faces_tot();
            ArrOfDouble somme(la_matrice.nb_colonnes()); // On dimensionne au plus grand
            for (int ind_face=0; ind_face < nfaces; ind_face++)
              {
                int face = la_front_dis.num_face(ind_face);
                double max_coef=0;
                int ind_max=-1;
                double n2=0;
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    normale[comp]=face_normales(face,comp);
                    if (dabs(normale[comp])>dabs(max_coef))
                      {
                        max_coef=normale[comp];
                        ind_max=comp;
                      }
                    n2+=normale[comp]*normale[comp];
                  }
                normale/=sqrt(n2);
                max_coef=normale[ind_max];

                // On commence par recalculer secmem=secmem-A *present pour pouvoir modifier A (on en profite pour projeter)
                int nb_coeff_ligne=tab1[face*nb_comp+1] - tab1[face*nb_comp];
                for (int k=0; k<nb_coeff_ligne; k++)
                  {
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        int j=tab2[tab1[face*nb_comp+comp]-1+k]-1;
                        //assert(j!=(face*nb_comp+comp));
                        //if ((j>=(face*nb_comp))&&(j<(face*nb_comp+nb_comp)))
                        const    double& coef_ij=la_matrice(face*nb_comp+comp,j);
                        int face2=j/nb_comp;
                        int comp2=j-face2*nb_comp;
                        secmem(face,comp)-=coef_ij*champ_inconnue(face2,comp2);
                      }
                  }
                double somme_b=0;

                for (int comp=0; comp<nb_comp; comp++)
                  somme_b+=secmem(face,comp)*normale[comp];

                // on retire secmem.n n
                for (int comp=0; comp<nb_comp; comp++)
                  secmem(face,comp)-=somme_b*normale[comp];

                // on doit remettre la meme diagonale partout on prend la moyenne
                double ref=0;
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j0=face*nb_comp+comp;
                    ref+=la_matrice(j0,j0);
                  }
                ref/=nb_comp;

                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j0=face*nb_comp+comp;
                    double rap=ref/la_matrice(j0,j0);
                    for (int k=0; k<nb_coeff_ligne; k++)
                      {
                        int j=tab2[tab1[j0]-1+k]-1;
                        la_matrice(j0,j)*=rap;
                      }
                    assert(est_egal(la_matrice(j0,j0),ref));
                  }
                // on annule tous les coef extra diagonaux du bloc
                //
                for (int k=1; k<nb_coeff_ligne; k++)
                  {
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        int j=tab2[tab1[face*nb_comp+comp]-1+k]-1;
                        assert(j!=(face*nb_comp+comp));
                        if ((j>=(face*nb_comp))&&(j<(face*nb_comp+nb_comp)))
                          la_matrice(face*nb_comp+comp,j)=0;
                      }
                  }

                // pour les blocs extra diagonaux on assure que Aij.ni=0
                //ArrOfDouble somme(nb_coeff_ligne);
                for (int k=0; k<nb_coeff_ligne; k++)
                  {
                    somme(k)=0;
                    int j=tab2[tab1[face*nb_comp]-1+k]-1;

                    // le coeff j doit exister sur les nb_comp lignes
                    double dsomme=0;
                    for (int comp=0; comp<nb_comp; comp++)
                      dsomme+=la_matrice(face*nb_comp+comp,j)*normale[comp];

                    // on retire somme ni

                    for (int comp=0; comp<nb_comp; comp++)
                      // on modifie que les coefficients ne faisant pas intervenir u(face,comp)
                      if ((j<(face*nb_comp))||(j>=(face*nb_comp+nb_comp)))
                        la_matrice(face*nb_comp+comp,j)-=(dsomme)*normale[comp];
                  }
                // Finalement on recalcule secmem=secmem+A*champ_inconnue (A a ete beaucoup modiife)
                for (int k=0; k<nb_coeff_ligne; k++)
                  {
                    for (int comp=0; comp<nb_comp; comp++)
                      {
                        int j=tab2[tab1[face*nb_comp+comp]-1+k]-1;
                        int face2=j/nb_comp;
                        int comp2=j-face2*nb_comp;
                        const double& coef_ij=la_matrice(face*nb_comp+comp,j);
                        secmem(face,comp)+=coef_ij*champ_inconnue(face2,comp2);
                      }
                  }
                {
                  // verification
                  double somme_c=0;
                  for (int comp=0; comp<nb_comp; comp++)
                    somme_c+=secmem(face,comp)*normale[comp];
                  // on retire secmem.n n
                  for (int comp=0; comp<nb_comp; comp++)
                    secmem(face,comp)-=somme_c*normale[comp];
                }
              }
          }
#endif
    }
}

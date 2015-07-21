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
// File:        Champ_val_tot_sur_vol_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_val_tot_sur_vol_VEF.h>
#include <Sous_Zone.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Dirichlet.h>

Implemente_instanciable(Champ_val_tot_sur_vol_VEF,"Valeur_totale_sur_volume_VEF",Champ_val_tot_sur_vol_base);


Sortie& Champ_val_tot_sur_vol_VEF::printOn(Sortie& os) const
{
  Champ_val_tot_sur_vol_base::printOn(os);
  return os;
}

Entree& Champ_val_tot_sur_vol_VEF::readOn(Entree& is)
{
  Champ_val_tot_sur_vol_base::readOn(is);
  return is;
}

DoubleVect& Champ_val_tot_sur_vol_VEF::eval_contrib_loc(const Zone_dis_base& zdis,const Zone_Cl_dis_base& zcldis,DoubleVect& vol_glob_pond)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF,zdis);
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF,zcldis);
  const int nb_elem = zvef.nb_elem();
  int size_vol = les_sous_zones.size()+1;
  vol_glob_pond.resize(size_vol);

  const int& nb_faces = zvef.nb_faces();
  const int& nb_fac_el = zvef.zone().nb_faces_elem();
  const IntTab& elem_faces = zvef.elem_faces();
  const DoubleVect& vol_entrelaces = zvef.volumes_entrelaces();
  const DoubleVect& vol_entrelaces_Cl =  zclvef.volumes_entrelaces_Cl();
  const ArrOfInt& faces_doubles = zvef.faces_doubles();
  const DoubleVect& por_face = zvef.porosite_face();
  int prem_face_std = zvef.premiere_face_std();
  int face_g,face_marq;
  double fac_pond,vol_entrelace;

  int cpt=1;
  IntVect face_fait(nb_faces);

  LIST_CURSEUR(REF(Sous_Zone)) curseur1 = les_sous_zones;
  while(curseur1)
    {
      const Sous_Zone& sz = curseur1.valeur().valeur();
      int size_sz = sz.nb_elem_tot();
      face_fait = 0;
      int el,elem0,elem1,elem_test;

      for (int elem=0; elem<size_sz; elem++)
        {
          el = sz(elem);
          //On ne retient ques les elements reels
          if (el<nb_elem)
            {
              for (int fac=0; fac<nb_fac_el; fac++)
                {
                  face_g = elem_faces(el,fac);

                  if (!face_fait(face_g))
                    {

                      //Parmi les deux elements voisins de la face traitee on retient
                      //celui qui n est pas l element courant dans la sous zone
                      elem0 = zvef.face_voisins(face_g,0);
                      elem1 = zvef.face_voisins(face_g,1);
                      if (elem0==el)
                        elem_test = elem1;
                      else
                        elem_test = elem0;

                      //Quatre situations possibles
                      //-elem_test=-1 condition limite
                      //-elem_test est aussi dans la sous zone courante
                      //-elem_test est dans une autre sous zone
                      //-elem_test est dans la partie par defaut du domaine (pas dans une sous zone)

                      int ok_trouve_loc = 0;
                      //Recherche si elem_test est dans la sous zone courante
                      //(1 si element reel-2 si virtuel-0 sinon)
                      //ok_trouve_loc fixe a 1 si elem_test dans la sous zone courante 0 sinon
                      for (int poly=0; poly<size_sz; poly++)
                        {
                          if (elem_test==sz(poly))
                            {
                              ok_trouve_loc = 1;
                              if (elem_test>nb_elem-1)
                                ok_trouve_loc = 2;
                              break;
                            }
                        }

                      //elem_test est dans la sous zone courante (en element reel) ou est en condition limite
                      if ((ok_trouve_loc==1) || (elem_test==-1))
                        fac_pond = 1.;
                      //elem_test est dans une autre sous zone ou dans la partie par defaut
                      //ou dans la sous zone courante mais en element virtuel
                      else
                        fac_pond = 0.5;

                      if (face_g<prem_face_std)
                        vol_entrelace = vol_entrelaces_Cl(face_g);
                      else
                        vol_entrelace =  vol_entrelaces(face_g);

                      vol_glob_pond(cpt) += fac_pond*vol_entrelace*por_face(face_g);
                      face_fait(face_g) = 1;
                    }
                }
            }
        }

      //on retire les contribs vol_entrelaces_Cl pour les faces de Dirichlet car la puissance
      //attribuee sur ces vol_entrelaces_Cl n'est pas effective au cours du calcul
      for (int n_bord=0; n_bord<zvef.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zclvef.les_conditions_limites(n_bord);
          if (sub_type(Dirichlet,la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int face;
              int num1 = 0;
              int num2 = le_bord.nb_faces();
              for (int ind_face=num1; ind_face<num2; ind_face++)
                {
                  face = le_bord.num_face(ind_face);
                  if (face_fait(face)==1)
                    vol_glob_pond(cpt) -= vol_entrelaces_Cl(face)*por_face(face);
                }
            }
        }

      cpt++;
      ++curseur1;
    }

  for (int num_face=0; num_face<prem_face_std; num_face++)
    {
      face_marq = faces_doubles(num_face);
      double contrib_double = double(face_marq);
      vol_glob_pond(0) += (1.-0.5*contrib_double)*vol_entrelaces_Cl(num_face)*por_face(num_face);
    }


  //on retire les contribs vol_entrelaces_Cl pour les faces de Dirichlet car la puissance
  //attribuee sur ces vol_entrelaces_Cl n'est pas effective au cours du calcul
  for (int n_bord=0; n_bord<zvef.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zclvef.les_conditions_limites(n_bord);
      if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int face;
          int num1 = 0;
          int num2 = le_bord.nb_faces();
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              face_marq = faces_doubles(face);
              double contrib_double = double(face_marq);
              vol_glob_pond(0) -= (1.-0.5*contrib_double)*vol_entrelaces_Cl(face)*por_face(face);
            }
        }
    }

  for (int num_face=prem_face_std; num_face<nb_faces; num_face++)
    {
      face_marq = faces_doubles(num_face);
      double contrib_double = double(face_marq);
      vol_glob_pond(0) += (1.-0.5*contrib_double)*vol_entrelaces(num_face)*por_face(num_face);
    }
  vol_glob_pond(0) = mp_sum(vol_glob_pond(0));

  for (int i=1; i<size_vol; i++)
    {
      vol_glob_pond(i) = mp_sum(vol_glob_pond(i));
      vol_glob_pond(0) -= vol_glob_pond(i);
    }

  return vol_glob_pond;
}

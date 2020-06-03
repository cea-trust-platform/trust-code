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
// File:        Discretisation_tools.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Discretisation_tools.h>
#include <DoubleTrav.h>
#include <Champ_base.h>
#include <Zone_VF.h>
#include <Debog.h>
#include <Check_espace_virtuel.h>
#include <cmath>

void Discretisation_tools::nodes_to_cells(const Champ_base& Hn,  Champ_base& He)
{
  const DoubleTab& tabHn=Hn.valeurs();
  DoubleTab& tabHe=He.valeurs();
  const Zone_dis_base& zone_dis_base=He.zone_dis_base();

  assert(tabHe.dimension_tot(0)==zone_dis_base.nb_elem_tot());
  assert(tabHn.dimension_tot(0)==zone_dis_base.nb_som_tot());

  const IntTab& les_elems=zone_dis_base.zone().les_elems();
  int nb_som_elem=les_elems.dimension(1);
  int nb_elem=zone_dis_base.nb_elem();

  tabHe=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      for (int s=0; s<nb_som_elem; s++)
        {
          int sglob=les_elems(ele,s);
          tabHe(ele)+=tabHn(sglob);
        }
    }
  double inv_nb_som_elem=1./(nb_som_elem);
  tabHe*=inv_nb_som_elem;
  tabHe.echange_espace_virtuel();
}
void Discretisation_tools::cells_to_nodes(const Champ_base& He,  Champ_base& Hn)
{

  DoubleTab& tabHn=Hn.valeurs();
  const DoubleTab& tabHe=He.valeurs();
  Debog::verifier("elno entreee",tabHe);
  assert_espace_virtuel_vect(tabHe);
  const Zone_dis_base& zone_dis_base=He.zone_dis_base();

  assert(tabHe.dimension_tot(0)==zone_dis_base.nb_elem_tot());
  assert(tabHn.dimension_tot(0)==zone_dis_base.nb_som_tot());

  const IntTab& les_elems=zone_dis_base.zone().les_elems();
  const DoubleVect& volumes=ref_cast(Zone_VF,zone_dis_base).volumes();

  tabHn=0;
  DoubleTrav vsom(tabHn.dimension_tot(0));
  int nb_som_elem=les_elems.dimension(1);
  int nb_elem_tot=zone_dis_base.nb_elem_tot();
  int nb_dim=tabHe.nb_dim();
  if (nb_dim==1)
    {
      for (int ele=0; ele<nb_elem_tot; ele++)
        {
          for (int s=0; s<nb_som_elem; s++)
            {
              int sglob=les_elems(ele,s);
              tabHn(sglob)+=tabHe(ele)*volumes(ele);
              vsom(sglob)+=volumes(ele);
            }
        }
      for (int som=0; som<zone_dis_base.nb_som(); som++)
        {
          tabHn(som)/=vsom(som);
        }
    }
  else
    {
      int nb_comp=tabHn.dimension(1);
      for (int ele=0; ele<nb_elem_tot; ele++)
        {
          for (int s=0; s<nb_som_elem; s++)
            {
              int sglob=les_elems(ele,s);
              for (int comp=0; comp<nb_comp; comp++)
                tabHn(sglob,comp)+=tabHe(ele,comp)*volumes(ele);
              vsom(sglob)+=volumes(ele);
            }
        }
      for (int som=0; som<zone_dis_base.nb_som(); som++)
        {
          for (int comp=0; comp<nb_comp; comp++)
            tabHn(som,comp)/=vsom(som);
        }
    }
  tabHn.echange_espace_virtuel();
  Debog::verifier("elno sortie",tabHn);
}
void Discretisation_tools::faces_to_cells(const Champ_base& Hf,  Champ_base& He)
{
  const DoubleTab& tabHf=Hf.valeurs();
  DoubleTab& tabHe=He.valeurs();
  const Zone_dis_base& zone_dis_base=He.zone_dis_base();
  const Zone_VF& zone_vf= ref_cast(Zone_VF,zone_dis_base);
  assert(tabHe.dimension_tot(0)==zone_dis_base.nb_elem_tot());
  assert(tabHf.dimension_tot(0)==zone_vf.nb_faces_tot());

  const IntTab& elem_faces=zone_vf.elem_faces();
  int nb_face_elem=elem_faces.dimension(1);
  int nb_elem=zone_dis_base.nb_elem();

  tabHe=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      for (int s=0; s<nb_face_elem; s++)
        {

          tabHe(ele)+=tabHf(elem_faces(ele,s));
        }
    }
  double inv_nb_face_elem=1./(nb_face_elem);
  tabHe*=inv_nb_face_elem;
  tabHe.echange_espace_virtuel();
}
void Discretisation_tools::cells_to_faces(const Champ_base& He,  Champ_base& Hf)
{

  DoubleTab& tabHf=Hf.valeurs();
  const DoubleTab& tabHe=He.valeurs();
  Debog::verifier("element_face entreee",tabHe);
  assert_espace_virtuel_vect(tabHe);
  const Zone_dis_base& zone_dis_base=He.zone_dis_base();

  const Zone_VF& zone_vf= ref_cast(Zone_VF,zone_dis_base);
  // en realite on fait P1B vers face
  //assert(tabHe.dimension_tot(0)==zone_dis_base.nb_elem_tot());
  assert(tabHf.dimension_tot(0)==zone_vf.nb_faces_tot());

  const IntTab& elem_faces=zone_vf.elem_faces();
  const DoubleVect& volumes=zone_vf.volumes();
  const DoubleVect& volumes_entrelaces=zone_vf.volumes_entrelaces();



  tabHf=0;
  int nb_face_elem=elem_faces.dimension(1);
  int nb_elem_tot=zone_dis_base.nb_elem_tot();
  int nb_dim=tabHe.nb_dim();


  double coeffb=nb_face_elem;
  double coeffi=coeffb;
  if (zone_vf.que_suis_je()=="Zone_VDF")
    {
      coeffb=1;
      coeffi=2;
    }

  if (nb_dim==1)
    {
      DoubleTab vol_tot(tabHf);
      for (int ele=0; ele<nb_elem_tot; ele++)
        for (int s=0; s<nb_face_elem; s++)
          {
            tabHf(elem_faces(ele,s))+=tabHe(ele)*volumes(ele);
            vol_tot(elem_faces(ele,s)) += volumes(ele);
          }
      for (int f = 0; f < zone_vf.nb_faces(); f++)
        tabHf(f) /= vol_tot(f);
    }
  else if (nb_dim==2 && tabHe.dimension(1) == 1) //one more case
    {
      for (int ele=0; ele<nb_elem_tot; ele++)
        {
          for (int s=0; s<nb_face_elem; s++)
            {

              tabHf(elem_faces(ele,s))+=tabHe(ele, 0)*volumes(ele);
              //	      Cerr<<elem_faces(ele,s)<<" "<<tabHe(ele)*volumes(ele)<<" "<<tabHf(elem_faces(ele,s))<<finl;
            }
        }
      for (int f=0; f<zone_vf.premiere_face_int(); f++)
        tabHf(f)/=volumes_entrelaces(f)*coeffb;
      for (int f=zone_vf.premiere_face_int(); f<zone_vf.nb_faces(); f++)
        tabHf(f)/=volumes_entrelaces(f)*coeffi;
    }
  else
    {

      if (tabHf.nb_dim()==1)
        {
          assert(coeffi==2);
          assert(coeffb==1);
          // VDF
          //abort();
          for (int ele=0; ele<nb_elem_tot; ele++)
            {
              for (int s=0; s<nb_face_elem; s++)
                {
                  int face=elem_faces(ele,s);
                  for (int r = 0; r < zone_vf.dimension; r++)
                    {
                      // Change of basis N.K.N, with N the normal of the face, and K the tensorial coefficient to get the value of the diffusivity
                      // on the direction of the surface normal.
                      double normOnSurf = zone_vf.face_normales(face, r) / zone_vf.face_surfaces(face);
                      tabHf(face) += tabHe(ele, r) * normOnSurf * normOnSurf * volumes(ele);
                    }
                }
            }
          for (int f=0; f<zone_vf.premiere_face_int(); f++)
            tabHf(f)/=volumes_entrelaces(f)*coeffb;
          for (int f=zone_vf.premiere_face_int(); f<zone_vf.nb_faces(); f++)
            tabHf(f)/=volumes_entrelaces(f)*coeffi;
        }
      else
        {
          //abort();
          int nb_comp=tabHf.dimension(1);
          for (int ele=0; ele<nb_elem_tot; ele++)
            {
              for (int s=0; s<nb_face_elem; s++)
                {
                  int face=elem_faces(ele,s);
                  for (int comp=0; comp<nb_comp; comp++)
                    tabHf(face,comp)+=tabHe(ele,comp)*volumes(ele);
                }
            }

          for (int f=0; f<zone_vf.premiere_face_int(); f++)
            for (int comp=0; comp<nb_comp; comp++)
              tabHf(f,comp)/=volumes_entrelaces(f)*coeffb;
          for (int f=zone_vf.premiere_face_int(); f<zone_vf.nb_faces(); f++)
            for (int comp=0; comp<nb_comp; comp++)
              tabHf(f,comp)/=volumes_entrelaces(f)*coeffi;

        }
    }
  tabHf.echange_espace_virtuel();
//  Cerr<<min_array(tabHe)<<" elem  "<<max_array(tabHe)<<finl;
//  Cerr<<min_array(tabHf)<<" face  "<<max_array(tabHf)<<finl;

  Debog::verifier("element_face sortie",tabHf);
}




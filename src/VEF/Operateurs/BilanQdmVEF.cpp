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
// File:        BilanQdmVEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <BilanQdmVEF.h>
#include <Zone_Cl_VEF.h>
#include <Zone_VEF.h>
#include <Front_VF.h>
#include <Les_Cl.h>

void BilanQdmVEF::bilan_qdm(const DoubleTab& dudt, const Zone_Cl_VEF& zone_Cl_VEF, ArrOfDouble& bilan)
{
  int nb_bord=zone_Cl_VEF.nb_cond_lim();
  int face;
  int nb_comp=bilan.size_array();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int nb_faces=le_bord.nb_faces();
      int num2 = num1 + nb_faces;
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int face_associee;
          IntVect fait(nb_faces);
          fait = 0;

          for (face=num1; face<num2; face++)
            {
              if (fait(face-num1) == 0)
                {
                  fait(face-num1) = 1;
                  face_associee=la_cl_perio.face_associee(face-num1);
                  fait(face_associee) = 1;
                  for (int comp=0; comp<nb_comp; comp++)
                    {
                      if(nb_comp==1)
                        bilan[comp]+=dudt(face);
                      else
                        bilan[comp]+=dudt(face,comp);
                    }
                }// if fait
            }// for face
        }// sub_type Perio
      else if ( (sub_type(Dirichlet,la_cl.valeur())) ||
                (sub_type(Dirichlet_homogene,la_cl.valeur())) )
        {
          //           for (face=num1; face<num2; face++)
          //             {
          //               if(nb_comp==1)
          //                 {
          //                   dudt(face)=0;
          //                 }
          //               else
          //                 for (int comp=0; comp<nb_comp; comp++)
          //                   {
          //                     dudt(face,comp)=0.;
          //                   }
          //             }
        }
      else
        {
          for (face=num1; face<num2; face++)
            {
              for (int comp=0; comp<nb_comp; comp++)
                {
                  if(nb_comp==1)
                    bilan[comp]+=dudt(face);
                  else
                    bilan[comp]+=dudt(face,comp);
                }
            }// for face
        }
    }// for nbord
  {
    const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zone_Cl_VEF.zone_dis().valeur());
    const int prems=zone_VEF.premiere_face_int();
    const int nb_faces=zone_VEF.nb_faces_tot();
    for(face=prems; face<nb_faces; face++)
      {
        for (int comp=0; comp<nb_comp; comp++)
          {
            if(nb_comp==1)
              bilan[comp]+=dudt(face);
            else
              bilan[comp]+=dudt(face,comp);
          }
      }
  }
}

void BilanQdmVEF::bilan_energie(const DoubleTab& dudt,
                                const DoubleTab& u,
                                const Zone_Cl_VEF& zone_Cl_VEF,
                                ArrOfDouble& bilan)
{
  int nb_bord=zone_Cl_VEF.nb_cond_lim();
  int face;
  int nb_comp=bilan.size_array();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int nb_faces=le_bord.nb_faces();
      int num2 = num1 + nb_faces;
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int face_associee;
          IntVect fait(nb_faces);
          fait = 0;

          for (face=num1; face<num2; face++)
            {
              if (fait(face-num1) == 0)
                {
                  fait(face-num1) = 1;
                  face_associee=la_cl_perio.face_associee(face-num1);
                  fait(face_associee) = 1;
                  for (int comp=0; comp<nb_comp; comp++)
                    {
                      if(nb_comp==1)
                        bilan[comp]+=dudt(face)*u(face);
                      else
                        bilan[comp]+=dudt(face,comp)*u(face,comp);
                    }
                }// if fait
            }// for face
        }// sub_type Perio
      else if ( (sub_type(Dirichlet,la_cl.valeur())) ||
                (sub_type(Dirichlet_homogene,la_cl.valeur())) )
        {
          //           for (face=num1; face<num2; face++)
          //             {
          //               if(nb_comp==1)
          //                 {
          //                   dudt(face)=0;
          //                 }
          //               else
          //                 for (int comp=0; comp<nb_comp; comp++)
          //                   {
          //                     dudt(face,comp)=0.;
          //                   }
          //             }
        }
      else
        {
          for (face=num1; face<num2; face++)
            {
              for (int comp=0; comp<nb_comp; comp++)
                {
                  if(nb_comp==1)
                    bilan[comp]+=dudt(face)*u(face);
                  else
                    bilan[comp]+=dudt(face,comp)*u(face,comp);
                }
            }// for face
        }
    }// for nbord
  {
    const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zone_Cl_VEF.zone_dis().valeur());
    const int prems=zone_VEF.premiere_face_int();
    const int nb_faces=zone_VEF.nb_faces_tot();
    for(face=prems; face<nb_faces; face++)
      {
        for (int comp=0; comp<nb_comp; comp++)
          {
            if(nb_comp==1)
              bilan[comp]+=dudt(face)*u(face);
            else
              bilan[comp]+=dudt(face,comp)*u(face,comp);
          }
      }
  }
}
